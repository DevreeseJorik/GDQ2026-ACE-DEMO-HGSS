#!/usr/bin/env python3
"""
Packager for heartgold_soulsilver scripts.

Usage:
  python3 packager.py [--config packager.yaml]

Config (YAML) example:
game: heartgold_soulsilver
entries:
  - name: boot_hijack_hook
  - name: inject_custom_palette
  - name: load_sprite_from_narc

Each entry may optionally specify bin_name or dump_name to override defaults.
"""
import argparse
import re
import sys
from pathlib import Path

try:
    import yaml
except Exception:
    print("Missing PyYAML. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(2)


START_ADDRESS = 0x23C4000

POKE_COUNT = 30
BOX_COUNT = 18

POKE_SIZE = 0x88
BOX_PADDING = 0x10

# original box data
BOX_SIZE = POKE_COUNT * POKE_SIZE + BOX_PADDING
PACKED_SIZE = BOX_COUNT * BOX_SIZE

# unpacked data
UNPACKED_POKE_SIZE = POKE_SIZE - 1
UNPACKED_BOX_SIZE = POKE_COUNT * UNPACKED_POKE_SIZE + BOX_PADDING
UNPACKED_SIZE = BOX_COUNT * UNPACKED_BOX_SIZE


def error(msg):
    print("ERROR:", msg, file=sys.stderr)
    sys.exit(1)


def find_address_in_dump(dump_path):
    txt = dump_path.read_text(errors="ignore")
    # find all hex-like tokens of 6-8 chars
    tokens = re.findall(r'\b0x[0-9A-Fa-f]{6,8}\b|\b[0-9A-Fa-f]{6,8}\b', txt)
    candidates = []
    for t in tokens:
        if t.startswith(("0x", "0X")):
            v = int(t, 16)
        else:
            v = int(t, 16)
        candidates.append(v)
    # prefer an address that lies within the RAW region (with box padding)
    for v in candidates:
        if START_ADDRESS <= v < START_ADDRESS + UNPACKED_SIZE:
            return v
    # fallback: if any candidate equals START_ADDRESS exactly
    for v in candidates:
        if v == START_ADDRESS:
            return v
    # otherwise return first candidate (best-effort) or None
    return candidates[0] if candidates else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--config", "-c", default="packager.yaml")
    args = ap.parse_args()

    cfg_path = Path(args.config)
    if not cfg_path.exists():
        error(f"config not found: {cfg_path}")

    cfg = yaml.safe_load(cfg_path.read_text())

    if not cfg or "game" not in cfg or "entries" not in cfg:
        error("config must contain 'game' and 'entries' keys")

    game = cfg["game"]
    entries = cfg["entries"]
    out_root = Path("/home/project/out") / game

    unpacked = bytearray(UNPACKED_SIZE)
    occupied = []  # list of (start, end, name)

    for e in entries:
        if isinstance(e, dict):
            name = e.get("name")
            bin_name = e.get("bin_name", name)
            dump_name = e.get("dump_name", name)
        else:
            name = e
            bin_name = name
            dump_name = name

        if not name:
            error("entry missing name in config")

        bin_path = out_root / name / "bin" / f"{bin_name}.bin"
        dump_path = out_root / name / "dump" / f"{dump_name}.dump"

        if not bin_path.exists():
            error(f"bin file not found for '{name}': {bin_path}")
        if not dump_path.exists():
            error(f"dump file not found for '{name}': {dump_path}")

        addr = find_address_in_dump(dump_path)
        if addr is None:
            error(f"could not find address in dump: {dump_path}")

        if addr < 0 or addr > 0xFFFFFFFF:
            error(f"invalid address {addr:#x} parsed from {dump_path}")

        offset = addr - START_ADDRESS
        if offset < 0 or offset >= UNPACKED_SIZE:
            error(f"address {addr:#x} from {dump_path} is outside raw memory range [{START_ADDRESS:#x},{START_ADDRESS+RAW_REGION_SIZE:#x})")

        data = bin_path.read_bytes()
        end = offset + len(data)
        if end > UNPACKED_SIZE:
            error(f"binary {bin_path} (len {len(data)}) would overflow raw memory at offset {offset:#x}")

        for s, epos, owner in occupied:
            if not (end <= s or offset >= epos):
                error(f"binary {bin_path} overlaps with previously placed {owner} (ranges {offset:#x}-{end:#x} vs {s:#x}-{epos:#x})")
        occupied.append((offset, end, bin_path))

        unpacked[offset:end] = data

    out_dir = Path("packager")
    out_dir.mkdir(exist_ok=True)

    unpacked_path = out_dir / "unpacked.bin"
    unpacked_path.write_bytes(bytes(unpacked))
    print(f"Wrote unpacked memory: {unpacked_path} ({UNPACKED_SIZE} bytes)")

    # create packed memory by iterating box->poke, inserting 0x03 after 4 bytes each pokemon,
    # and adding BOX_PADDING bytes after each box.
    packed = bytearray(PACKED_SIZE)
    src = 0
    dst = 0
    
    for box in range(BOX_COUNT):
        for poke in range(POKE_COUNT):
            packed[dst:dst + 4] = unpacked[src:src + 4] # copy pid
            src += 4
            dst += 4
            
            packed[dst] = 0x03 # set checking for decryption to false
            # do not increment src here, as this byte is not present in unpacked data
            dst += 1

            rem = UNPACKED_POKE_SIZE - 4 # remaining bytes after pid
            packed[dst:dst + rem] = unpacked[src:src + rem]
            src += rem
            dst += rem

        packed[dst:dst + BOX_PADDING] = bytes(0x00 for _ in range(BOX_PADDING))
        dst += BOX_PADDING

    if dst != PACKED_SIZE:
        error(f"internal size mismatch after packing: wrote {dst} bytes, expected {PACKED_SIZE}")

    packed_path = out_dir / "packed.bin"
    packed_path.write_bytes(bytes(packed))
    print(f"Wrote packed memory with box padding: {packed_path} ({PACKED_SIZE} bytes)")

    return 0


if __name__ == "__main__":
    sys.exit(main())