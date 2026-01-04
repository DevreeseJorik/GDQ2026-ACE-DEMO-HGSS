#!/usr/bin/env python3

import re
import argparse
import textwrap
from pathlib import Path

def parse_address_from_dump(dump_path: Path) -> int:
    """
    Extract the first hex address from the dump.
    """
    text = dump_path.read_text(errors="ignore")
    match = re.search(r"\b0x?[0-9A-Fa-f]{6,8}\b", text)
    if not match:
        raise ValueError(f"No address found in dump: {dump_path}")
    return int(match.group(0), 16)


def format_u8_array(data: bytes) -> str:
    items = ", ".join(f"0x{b:02X}" for b in data)
    return f"{{ {items} }}"

def strip_padding(data: bytes) -> bytes:
    """
    If the binary ends with two zero bytes AND its length is not 4-byte aligned,
    remove the last two bytes (padding).
    """
    if len(data) % 4 != 0 and len(data) >= 2 and data[-2:] == b"\x00\x00":
        return data[:-2]
    return data


def get_autohook_preprocessor():
    return textwrap.dedent(r"""
        #define SET_HOOK(NAME)                                                    \
        do {                                                                      \
            if ((NAME##_TARGET % 4) == 0) {                                       \
                write_u32((u32 *)NAME##_TARGET, NAME##_DATA);                 \
            }                                                                     \
            else if ((NAME##_TARGET % 2) == 0) {                                  \
                write_u32_16bit_alligned((u16 *)NAME##_TARGET, NAME##_DATA);  \
            }                                                                     \
        } while (0)
    """)


def generate_entry(name: str, target_addr: int, data: bytes) -> str:
    macro = name.upper()

    out = []
    out.append(f"// ===== {name} =====")
    out.append(f"#define {macro}_TARGET 0x{target_addr:08X}")

    if len(data) <= 4:
        value = int.from_bytes(data.ljust(4, b'\x00'), "little")
        out.append(f"#define {macro}_DATA 0x{value:08X}")
    else:
        out.append(f"static const unsigned char {macro}_DATA[] = {format_u8_array(data)};")
        out.append(f"#define {macro}_DATA {len(data)}")

    out.append("")  # blank line
    return "\n".join(out)


def main():
    parser = argparse.ArgumentParser(
        description="Generate a single preprocessor header from hook binaries and dumps."
    )

    parser.add_argument("--bins", required=True, type=Path,
                        help="Directory containing hook .bin files")
    parser.add_argument("--dumps", required=True, type=Path,
                        help="Directory containing hook .dump files")
    parser.add_argument("--out", required=True, type=Path,
                        help="Output directory for generated header")

    args = parser.parse_args()

    bin_dir: Path = args.bins
    dump_dir: Path = args.dumps
    out_dir: Path = args.out

    out_dir.mkdir(parents=True, exist_ok=True)

    output_path = out_dir / "preprocessors.hpp"

    entries = []

    for bin_file in sorted(bin_dir.glob("*.bin")):
        name = bin_file.stem
        dump_file = dump_dir / f"{name}.dump"

        if not dump_file.exists():
            print(f"Warning: missing dump for {name}, skipping")
            continue

        target_addr = parse_address_from_dump(dump_file)
        data = strip_padding(bin_file.read_bytes())

        entries.append(generate_entry(name, target_addr, data))

    header = "#pragma once\n\n"
    output_path.write_text(header + get_autohook_preprocessor() + "\n\n" + "\n".join(entries))

    print(f"\nGenerated combined preprocessor header: {output_path}")


if __name__ == "__main__":
    main()
