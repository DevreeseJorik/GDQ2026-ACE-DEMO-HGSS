#!/usr/bin/env python3

"""
Packager for heartgold_soulsilver scripts.

Usage:
  python3 packager.py [--config packager.yaml]

Config (YAML) example:
entries:
  - name: filename

  - name: bin_name
    bin_path: path/to/file.bin
    address: 0x23C8800

  - name: raw_bytes
    bytes: [0x00, 0x10]
    address: 0x2358000

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

    tokens = re.findall(r'\b0x[0-9A-Fa-f]{6,8}\b|\b[0-9A-Fa-f]{6,8}\b', txt)
    candidates = []
    for t in tokens:
        if t.startswith(("0x", "0X")):
            v = int(t, 16)
        else:
            v = int(t, 16)
        candidates.append(v)

    for v in candidates:
        if START_ADDRESS <= v < START_ADDRESS + UNPACKED_SIZE:
            return v

    for v in candidates:
        if v == START_ADDRESS:
            return v

    return candidates[0] if candidates else None

def autoresolve_path(path, name=None, root_path=None, _type="bin"):
    if path is not None:
        path = Path(path)
    elif not None in [name, root_path, _type]:
        path = Path(f"{root_path}/{_type}s/{name}.{_type}")

    if not path.exists():
        error(f"file not found: {path}")

    return path

def parse_entry(entry, root_path, previous_parsed_entry=None):
    if not isinstance(entry, dict):
        return None

    project_name = entry.get("name")
    if not project_name:
        error("entry missing name in config")

    print("Processing entry:", project_name)

    data = entry.get("bytes", None) 
    if data is not None:
        if not isinstance(data, list): 
            error(f"'bytes' for '{project_name}' must be a list but is {type(data)}")

        try: 
            data = bytes(int(x) for x in data) 
        except Exception: 
            error(f"invalid byte value in 'bytes' for '{project_name}': {data}") 
    else: 
        bin_path = entry.get("bin_path", None)
        bin_path = autoresolve_path(bin_path, project_name, root_path, _type="bin")
        data = bin_path.read_bytes()

    address_spec = entry.get("address", None)
    if address_spec is not None:
        if address_spec == "auto":
            if not previous_parsed_entry:
                error(f"Could not resolve address for {project_name}, there was no previous entry.")

            address = previous_parsed_entry.get("address", 0) + previous_parsed_entry.get("size", 0)
        else:
            try:
                if isinstance(address_spec, str):
                    address = int(address_spec, 16 if address_spec.lower().startswith(("0x") ) else 10)
                else:
                    address = int(address_spec)
            except Exception:
                error(f"invalid address specified for '{project_name}': {address_spec}")
    else:
        dump_path = entry.get("dump_path", None)
        dump_path = autoresolve_path(dump_path, project_name, root_path, _type="dump")

        address = find_address_in_dump(dump_path)
        if address is None:
            error(f"could not find address in dump: {dump_path}")

    return {
        "name": project_name,
        "data": data,
        "size": len(data),
        "address": address,
        "offset": address - START_ADDRESS,
    }

def verify_address(entry, allocated_entries=[]):
    address = entry["address"]
    size = entry["size"]
    offset = entry["offset"]

    if offset < 0 or offset + size >= UNPACKED_SIZE:
        error(f"address {address:#x} is outside memory range [{START_ADDRESS:#x},{START_ADDRESS+UNPACKED_SIZE:#x}]")

    for _entry in allocated_entries:
        entry_address = _entry["address"]
        entry_size = _entry["size"]

        if not (address + size <= entry_address or address >= entry_address + entry_size):
            error(f"address {address:#x} overlaps with previously allocated entry at {entry_address:#x} (size {entry_size})")

    allocated_entries.append(entry)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--config", "-c", default="packager.yaml")
    ap.add_argument("--input_path", "-i")
    ap.add_argument("--output_path", "-o")
    args = ap.parse_args()

    cfg_path = Path(args.config)
    if not cfg_path.exists():
        error(f"config not found: {cfg_path}")

    cfg = yaml.safe_load(cfg_path.read_text())
    if "entries" not in cfg:
        error("config must contain 'entries' keys")

    output_path = Path(args.output_path)
    if not output_path.exists:
        error(f"root path not found: {output_path}")

    entries = cfg["entries"]

    unpacked = bytearray(UNPACKED_SIZE)
    allocated_entries = []

    previous_parsed_entry = None
    for entry in entries:
        parsed_entry = parse_entry(entry, args.input_path, previous_parsed_entry)
        if parsed_entry is None:
            continue

        verify_address(parsed_entry, allocated_entries)
        
        data = parsed_entry["data"]
        size = parsed_entry["size"]        
        offset = parsed_entry["offset"]

        unpacked[offset:offset + size] = data

        previous_parsed_entry = parsed_entry

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

    output_path.mkdir(exist_ok=True)

    unpacked_path = output_path / "unpacked.bin"
    unpacked_path.write_bytes(bytes(unpacked))
    print(f"Wrote unpacked memory: {unpacked_path} ({UNPACKED_SIZE} bytes)")

    packed_path = output_path / "packed.bin"
    packed_path.write_bytes(bytes(packed))
    print(f"Wrote packed memory with box padding: {packed_path} ({PACKED_SIZE} bytes)")

    return 0

if __name__ == "__main__":
    sys.exit(main())
