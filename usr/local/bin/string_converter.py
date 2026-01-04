#!/usr/bin/env python3

import json
from pathlib import Path
import struct

class CharConverter:
    def __init__(self):
        base_path = Path(__file__).parent.resolve()
        data = self._load_json(base_path / "char_map.json")
        self.char_map = data["characters"]
        self.function_keys = data["function_keys"]
        self.rev_char_map = {v: k for k, v in self.char_map.items()}
        self.rev_function_keys = {v.strip(r"{}"): k for k, v in self.function_keys.items()}

    def _load_json(self, path):
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)

    def _parse_control_seq(self, text, i):
        end = text.find("}", i)
        if end == -1:
            raise ValueError("Unclosed control sequence starting at position {}".format(i))
        inner = text[i + 1:end]
        parts = [p.strip() for p in inner.split(",")]

        if len(parts) == 1:
            parts.append(0)

        key = self.rev_function_keys.get(parts[0])
        ctrl_prefix = 0xFFFE

        return [ctrl_prefix, int(key, 16)] + [int(v,0) & 0xFFFF for v in parts[1:]], end + 1

    def ascii_to_codes(self, text):
        codes = []
        i = 0
        n = len(text)
        while i < n:
            if text[i] == "{":
                ctrl_codes, next_i = self._parse_control_seq(text, i)
                codes.extend(ctrl_codes)
                i = next_i
                continue
            c = text[i]
            hexcode = self.rev_char_map.get(c)
            if hexcode is None:
                raise ValueError(f"Character not in map: {c!r}")
            codes.append(int(hexcode, 16))
            i += 1

        if not codes or codes[len(codes)-1] != 0xFFFF:
            codes.append(0xFFFF)

        return codes

    def codes_to_ascii(self, codes):
        out = []
        i = 0
        n = len(codes)
        while i < n:
            code = codes[i]
            if code == 0xFFFE and i + 2 < n:
                ctrl_type = codes[i + 1]
                ctrl_value = codes[i + 2]
                type_hex = f"{ctrl_type:04X}"
                name = self.function_keys.get(type_hex)
                if name is None:
                    out.append("'Invalid'")
                    i += 3
                    continue
                name = name.strip("{}")
                out.append(f"{{{name}, {ctrl_value}}}")
                i += 3
                continue
            hexword = f"{code:04X}"
            out.append(self.char_map.get(hexword, "'Invalid'"))
            i += 1
        return "".join(out)

    def codes_to_bytes_le(self, codes):
        out = bytearray()
        for code in codes:
            out.extend(code.to_bytes(2, "little"))
        return bytes(out)

    def codes_to_bytes_be(self, codes):
        out = bytearray()
        for code in codes:
            out.extend(code.to_bytes(2, "big"))
        return bytes(out)

    def codes_to_hex(self, codes, big_endian=False):
        if big_endian:
            return "".join(f"{code:04X}" for code in codes)
        else:
            return "".join(
                f"{code:04X}"[2:] + f"{code:04X}"[:2]
                for code in codes
            )

    def convert_to_ingame(self, text, big_endian=False, output_type=str):
        codes = self.ascii_to_codes(text)

        if output_type is str:
            return self.codes_to_hex(codes, big_endian=big_endian)

        return (
            self.codes_to_bytes_be(codes)
            if big_endian else
            self.codes_to_bytes_le(codes)
        )

    def convert_to_ascii(self, data_bytes):
        if not isinstance(data_bytes, (bytes, bytearray)):
            raise ValueError("convert_to_ascii expects raw bytes.")

        if len(data_bytes) % 2 != 0:
            raise ValueError("Binary length must be even.")

        codes = [
            int.from_bytes(data_bytes[i:i+2], "little")
            for i in range(0, len(data_bytes), 2)
        ]

        return self.codes_to_ascii(codes)

class CustomMessagePacker:
    HEADER_STRUCT = struct.Struct("<6H")

    def __init__(self, converter: CharConverter = CharConverter()):
        self.converter = converter

    def pack_messages(self, entries, write_to=None):
        headers = []
        strings = []

        for entry in entries:
            codes = self.converter.ascii_to_codes(entry["string"])
            raw = self.converter.codes_to_bytes_le(codes)
            strings.append(raw)

        header_count = len(entries) + 1
        header_block_size = header_count * self.HEADER_STRUCT.size
        offset = header_block_size

        for entry, raw in zip(entries, strings):
            h_type = entry["type"]
            h_exists = 1
            h_dataId = entry.get("dataId", 0)
            h_strId = entry.get("strId", 0)
            h_size = len(raw)
            h_offset = offset

            headers.append((h_type, h_exists, h_dataId, h_strId, h_size, h_offset))
            offset += h_size

        headers.append((0, 0, 0, 0, 0, 0))

        binary = bytearray()

        for h in headers:
            binary.extend(self.HEADER_STRUCT.pack(*h))

        for raw in strings:
            binary.extend(raw)

        if write_to:
            Path(write_to).write_bytes(binary)

        return bytes(binary)

if __name__ == "__main__":
    conv = CharConverter()

    print("Choose conversion mode:")
    print("1) ascii → ingame")
    print("2) ingame → ascii")
    mode = input("Enter 1 or 2: ").strip()

    raw = input("Enter data: ").strip()

    if mode == "1":
        endian = input("Big endian output? (y/n): ").strip().lower()
        big = (endian == "y")

        out_type = input("Output as (1) hex string or (2) binary repr: ").strip()
        out_type = str if out_type == "1" else bytes

        result = conv.convert_to_ingame(raw, big_endian=big, output_type=out_type)
        print("In-game encoding:", result)

    elif mode == "2":
        hex_clean = "".join(c for c in raw if c in "0123456789ABCDEFabcdef")
        data_bytes = bytes.fromhex(hex_clean)

        result = conv.convert_to_ascii(data_bytes)
        print("ASCII:", result)

    else:
        print("Invalid mode.")
