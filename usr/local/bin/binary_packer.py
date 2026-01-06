import argparse
import struct
from pathlib import Path
from typing import List, Tuple


class BinaryPacker:
    """
    Generic binary packer that scans subdirectories named with integer IDs
    and packs files with a given extension into a single binary blob.
    """

    def __init__(self, extension: str):
        """
        :param extension: File extension to look for, e.g. '.script' or '.bin'
        """
        if not extension.startswith("."):
            extension = "." + extension
        self.extension = extension
        self._header_struct = struct.Struct("<4H")

    def load_binaries(self, directory: Path) -> List[Tuple[int, bytes]]:
        """
        Loads all <id>/<id><extension> files.
        Returns a list of (id, data) tuples sorted by id.
        """
        binaries = []

        for subdir in directory.iterdir():
            if not subdir.is_dir():
                continue

            try:
                entry_id = int(subdir.name, 10)
            except ValueError:
                continue

            file_path = subdir / f"{subdir.name}{self.extension}"
            if not file_path.is_file():
                continue

            data = file_path.read_bytes()
            binaries.append((entry_id, data))

        if not binaries:
            raise ValueError(f"No *{self.extension} files found in {directory}")

        return sorted(binaries, key=lambda x: x[0])

    def build_output_blob(self, binaries: List[Tuple[int, bytes]]) -> bytes:
        """
        Build the final binary:
            [headers...+1 empty][binaries...]
        """
        header_count = len(binaries) + 1
        header_block_size = header_count * self._header_struct.size

        offset = header_block_size
        headers = []

        for entry_id, blob in binaries:
            exists = 1
            size = len(blob)
            headers.append((exists, entry_id, size, offset))
            offset += size

        # Add terminating empty header
        headers.append((0, 0, 0, 0))

        out = bytearray()

        for h in headers:
            out.extend(self._header_struct.pack(*h))

        for _, blob in binaries:
            out.extend(blob)

        return bytes(out)

    def pack(self, base_dir: Path) -> bytes:
        binaries = self.load_binaries(base_dir)
        return self.build_output_blob(binaries)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input", required=True,
                    help="Directory containing <id>/<id><ext> files")
    ap.add_argument("-e", "--extension", required=True,
                    help="File extension to pack (e.g. .script, .bin)")
    ap.add_argument("-o", "--output", default="output.bin",
                    help="Output file")
    args = ap.parse_args()

    directory = Path(args.input)
    if not directory.is_dir():
        raise ValueError(f"{directory} is not a directory")

    packer = BinaryPacker(args.extension)
    binaries = packer.load_binaries(directory)
    blob = packer.build_output_blob(binaries)

    Path(args.output).write_bytes(blob)
    print(f"Wrote {args.output} with {len(binaries)} entries.")


if __name__ == "__main__":
    main()
