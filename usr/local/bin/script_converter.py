#!/usr/bin/env python3

import argparse
import json
import re
import struct
from pathlib import Path
from typing import Dict, List, Tuple, Optional

base_path = Path(__file__).parent.resolve()

class CommandConverter:
    """
    Converts textual event commands into binary representation.

    - Loads a JSON file of [{ "id": <u16>, "name": <str> }, ...]
    - Maps command names to ids
    - Parses lines like:
        EvCmdNop u8:0x10 u16:42
      into:
        <u16:cmd_id> <u8:param0> <u16:param1>
    """

    def __init__(self):
        self.commands = self._load_json(base_path / "commands.json")
        self.name_to_id: Dict[str, int] = {entry["name"]: int(entry["id"], 0) for entry in self.commands}

    def _load_json(self, path: Path):
        with path.open("r", encoding="utf-8") as f:
            return json.load(f)

    def _parse_param_token(self, token: str) -> bytes:
        """
        Parse a token like 'u8:0x10', 'u16:42', 'u32:0b1010'.

        Returns bytes in little-endian.
        """
        try:
            type_part, value_part = token.split(":", 1)
        except ValueError:
            raise ValueError(f"Invalid param token (missing ':'): {token!r}")

        type_part = type_part.strip()
        value_part = value_part.strip()

        if not type_part.startswith("u"):
            raise ValueError(f"Param type must start with 'u': {type_part!r}")

        try:
            bit_size = int(type_part[1:], 10)
        except ValueError:
            raise ValueError(f"Invalid bit size in param: {type_part!r}")

        if bit_size not in (8, 16, 32):
            raise ValueError(
                f"Unsupported bit size {bit_size} in param {token!r} "
                f"(only 8/16/32 supported)."
            )

        try:
            value = int(value_part, 0)
        except ValueError:
            raise ValueError(f"Invalid param value: {value_part!r} in {token!r}")

        max_value = (1 << bit_size) - 1
        if not (0 <= value <= max_value):
            raise ValueError(
                f"Value {value} out of range for u{bit_size} in {token!r}"
            )

        byte_len = bit_size // 8
        return value.to_bytes(byte_len, "little")

    def encode_line(self, line: str) -> bytes:
        """
        Convert a single line like:
            EvCmdNop u8:0x10
        into:
            <u16:cmd_id> <u8:0x10>

        Empty or comment-only lines return b"".
        """
        stripped = line.strip()
        if not stripped:
            return b""
        if stripped.startswith("#") or stripped.startswith("//"):
            return b""

        parts = stripped.split()
        cmd_name = parts[0]
        try:
            cmd_id = self.name_to_id[cmd_name]
        except KeyError:
            raise ValueError(f"Unknown command name: {cmd_name!r}")

        out = bytearray()

        out.extend(struct.pack("<H", cmd_id))
        for token in parts[1:]:
            out.extend(self._parse_param_token(token))

        return bytes(out)

class EventPacker:
    """
    Packs a directory of event_<index> files into a single script blob:

        [u16 offsets[numEvents]] [u16 FD13] [event_0 bytes][event_1 bytes]...

    where offsets[i] is the offset (relative to start of the blob) of event i.
    """

    OFFSETS_TERMINATOR = 0xFD13

    def __init__(self, converter: CommandConverter):
        self.converter = converter

    def _collect_event_files(self, directory: Path) -> Dict[int, Path]:
        """
        Find files named event_<index>* in the directory and map index -> Path.

        Matches e.g.:
            event_0
            event_0.txt
            event_12.anything

        Index is parsed from the first integer after 'event_'.
        """
        mapping: Dict[int, Path] = {}
        pattern = re.compile(r"^event_(?:0x[0-9A-Fa-f]+|\d+)$")
        for path in directory.iterdir():
            if not path.is_file():
                continue
            match = pattern.match(path.name)
            if match:
                idx = int(match.group(0).split("_", 1)[1], 0)
                mapping[idx] = path
        if not mapping:
            raise ValueError(f"No event_<index> files found in {directory!s}.")
        return mapping

    def _parse_event_file(self, path: Path) -> bytes:
        """
        Parse a single event file into binary by converting each line.
        """
        out = bytearray()
        with path.open("r", encoding="utf-8") as f:
            for line in f:
                encoded = self.converter.encode_line(line)
                out.extend(encoded)
        return bytes(out)

    def build_script_blob(self, directory: Path) -> Tuple[bytes, int]:
        """
        Given a directory of event_<index> files, build the script blob and
        return (blob, num_events).

        Ensures that all indices from 0..maxIndex exist. Missing indices are
        filled with a default event;
        """
        directory = directory.resolve()
        if not directory.is_dir():
            raise ValueError(f"{directory!s} is not a directory.")

        files = self._collect_event_files(directory)
        max_index = max(files.keys())
        num_events = max_index + 1

        event_binaries: List[bytes] = []
        for idx in range(num_events):
            if idx in files:
                file = files[idx]
            else:
                file = base_path / "default_script.txt"
            event_bytes = self._parse_event_file(file)
            event_binaries.append(event_bytes)

        header_size = num_events * 4 + 2
        offsets: List[int] = []

        current_offset = header_size
        for i, event_bytes in enumerate(event_binaries):
            offsets.append(current_offset - 4 * (i+1))
            current_offset += len(event_bytes)

        blob = bytearray()
        for off in offsets:
            blob.extend(struct.pack("<I", off))

        blob.extend(struct.pack("<H", self.OFFSETS_TERMINATOR))

        for event_bytes in event_binaries:
            blob.extend(event_bytes)

        return bytes(blob), num_events


class ScriptPacker:
    HEADER_STRUCT = struct.Struct("<4H")

    def __init__(self, event_packer: EventPacker):
        self.event_packer = event_packer

    def _collect_script_dirs(self, base_dir: Path) -> List[Tuple[int, Path]]:
        pattern = re.compile(r"^script_(?:0x[0-9A-Fa-f]+|\d+)$")
        out: List[Tuple[int, Path]] = []

        for path in base_dir.iterdir():
            if not path.is_dir():
                continue

            match = pattern.match(path.name)
            if match:
                script_id = int(match.group(0).split("_", 1)[1] , 0)
                out.append((script_id, path))

        return sorted(out, key=lambda x: x[0])

    def pack_all_scripts(self, base_dir: Path, write_to=None) -> bytes:
        scripts = self._collect_script_dirs(base_dir)
        if not scripts:
            raise ValueError("No script_<id> directories found.")

        blobs: List[Tuple[int, bytes]] = []
        for script_id, path in scripts:
            blob, _ = self.event_packer.build_script_blob(path)
            blobs.append((script_id, blob))

        header_count = len(blobs) + 1
        header_block_size = header_count * self.HEADER_STRUCT.size

        offset = header_block_size
        headers = []

        for script_id, blob in blobs:
            exists = 1
            size = len(blob)
            headers.append((exists, script_id, size, offset))
            offset += size

        headers.append((0, 0, 0, 0))

        out = bytearray()

        for h in headers:
            out.extend(self.HEADER_STRUCT.pack(*h))

        for _, blob in blobs:
            out.extend(blob)

        if write_to:
            Path(write_to).write_bytes(out)

        return bytes(out)
