#!/usr/bin/env python3

import os
import struct
import argparse

from PIL import Image
from collections import Counter


class ImageConverter:
    def __init__(self, image_path, background_color=None):
        self.image_path = image_path
        self.background_color = background_color
        self.pixels = []
        self.color_counts = Counter()
        self.color_map = {}

    def parse_image(self):
        """Reads the image, stores pixel values, color frequencies, and creates a color map."""
        with Image.open(self.image_path) as img:
            img = img.convert("RGBA")

            self.pixels = list(img.getdata())
            self.color_counts = Counter(self.pixels)
            assert len(self.color_counts) <= 16, "Image has more than 16 unique colors."

            sorted_colors = [color for color, _ in self.color_counts.most_common()]

            if self.background_color is not None:
                bg = self.background_color
                if bg not in self.color_counts:
                    available = "\n".join(str(c) for c in sorted_colors)
                    raise ValueError(
                        f"Background color {bg} not found in image.\n"
                        f"Available colors:\n{available}"
                    )

                sorted_colors.remove(bg)
                sorted_colors.insert(0, bg)

            self.color_map = {color: i for i, color in enumerate(sorted_colors)}


    def convert_image(self):
        """Convert the pixel data into a binary format using 4-bit IDs."""
        byte_data = []
        for i in range(0, len(self.pixels), 2):
            color_id1 = self.color_map[self.pixels[i]]
            color_id2 = self.color_map[self.pixels[i + 1]] if i + 1 < len(self.pixels) else 0
            byte_data.append((color_id2 << 4) | color_id1)

        return bytes(byte_data)

    def rgba_to_rgba5(self, rgba):
        """Convert an RGBA color to a 16-bit color (5 bits per channel)."""
        r, g, b, a = rgba
        r = round((r * 31) / 255)
        g = round((g * 31) / 255)
        b = round((b * 31) / 255)
        a = 1 if a != 0 else 0
        return  (a << 15) | (b << 10) | (g << 5) | r

    def get_palette_data(self): 
        """Return the 16-color RGBA5 palette as binary data.""" 
        out = bytearray()
        for color in self.color_map: 
            color_16bit = self.rgba_to_rgba5(color) 
            out += struct.pack('H', color_16bit) 
        for _ in range(len(self.color_map), 16): 
            out += struct.pack('H', 0)
        return bytes(out)

    def save_converted_image(self, output_path):
        """Saves the converted pixel data to a binary file, including the actual 16-bit colors."""
        dirpath = os.path.dirname(output_path)
        if dirpath:
            os.makedirs(dirpath, exist_ok=True)

        with open(output_path, 'wb') as f:
            f.write(self.process_image())

    def print_palette(self):
        """Prints out all the unique colors sorted by frequency."""
        print("Colors in the Image (sorted by frequency):")
        sorted_colors = self.color_counts.most_common()
        for color, count in sorted_colors:
            print(f"Color {color}: {count} times")

    def process_image(self):
        self.parse_image()
        image = self.convert_image()
        palette = self.get_palette_data()
        return image + palette

def parse_arguments():
    parser = argparse.ArgumentParser(description="Convert a given image to the ingame binary format used to send to VRAM.")
    parser.add_argument("--input_path", "-i", type=str, help="Path to the directory containing images to convert.")
    parser.add_argument("--output_path", "-o", type=str, help="Path to the directory the converted image(s) should be saved to.")
    parser.add_argument("--background_color", "-b", type=str, help="Optional RGBA background color to force as palette index 0. Format: R,G,B,A")
    return parser.parse_args()


def process_image(input_path, output_path, background_color):
    converter = ImageConverter(input_path, background_color)
    converter.process_image()
    
    filename = os.path.splitext(os.path.basename(input_path))[0]
    output_file = os.path.join(output_path, f"{filename}.bin")

    converter.save_converted_image(output_file)
    print(f"Converted image saved to {output_file}")


def process_directory(input_directory, output_directory, background_color):
    for filename in os.listdir(input_directory):
        file_path = os.path.join(input_directory, filename)
        if os.path.isfile(file_path) and filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
            process_image(file_path, output_directory, background_color)


if __name__ == "__main__":
    args = parse_arguments()

    bg_color = None
    if args.background_color:
        try:
            parts = tuple(int(x) for x in args.background_color.split(","))
            if len(parts) != 4:
                raise ValueError
            bg_color = parts
        except ValueError:
            print("Invalid background color format. Use: R,G,B,A (e.g., 255,0,255,255)")
            exit(1)

    if args.input_path is None or args.output_path is None:
        print(f"No input path provided. Exiting")
    elif os.path.isdir(args.input_path):
        print(f"Processing all images in directory: {args.input_path}")
        process_directory(args.input_path, args.output_path, bg_color)
    elif os.path.isfile(args.input_path):
        print(f"Processing single image: {args.input_path}")
        process_image(args.input_path, args.output_path, bg_color)
    else:
        print(f"Invalid input path: {args.input_path}")

