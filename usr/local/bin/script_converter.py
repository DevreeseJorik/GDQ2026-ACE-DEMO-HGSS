import argparse
import struct
from pathlib import Path
from typing import List, Tuple

class ScriptPacker:
    def __init__(self):
        self._header_struct = struct.Struct("<4H")
        pass

    def load_script_binaries(self, directory: Path) -> List[Tuple[int, bytes]]:
        scripts = []

        for subdir in directory.iterdir():
            if not subdir.is_dir():
                continue

            try:
                script_id = int(subdir.name, 10)
            except ValueError:
                continue

            script_file = subdir / f"{subdir.name}.script"
            if not script_file.is_file():
                continue

            data = script_file.read_bytes()
            scripts.append((script_id, data))

        if not scripts:
            raise ValueError(f"No *.script files found in {directory}")

        return sorted(scripts, key=lambda x: x[0])

    def build_output_blob(self, scripts: List[Tuple[int, bytes]]) -> bytes:
        """
        Build the final binary:
            [headers...+1 empty][binaries...]
        """
        header_count = len(scripts) + 1
        header_block_size = header_count * self._header_struct.size

        offset = header_block_size
        headers = []

        for script_id, blob in scripts:
            exists = 1
            size = len(blob)
            headers.append((exists, script_id, size, offset))
            offset += size

        headers.append((0, 0, 0, 0))

        out = bytearray()

        for h in headers:
            out.extend(self._header_struct.pack(*h))

        for _, blob in scripts:
            out.extend(blob)

        return bytes(out)


    def pack_all_scripts(self, base_dir: Path) -> bytes:
        scripts = self.load_script_binaries(base_dir)
        return self.build_output_blob(scripts)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input", help="Directory containing <id>.script files")
    ap.add_argument("-o", "--output", default="scripts.bin", help="Output file")
    args = ap.parse_args()

    directory = Path(args.input)
    if not directory.is_dir():
        raise ValueError(f"{directory} is not a directory")

    packer = ScriptPacker()
    scripts = packer.load_script_binaries(directory)
    blob = packer.build_output_blob(scripts)

    Path(args.output).write_bytes(blob)
    print(f"Wrote {args.output} with {len(scripts)} scripts.")


if __name__ == "__main__":
    main()
