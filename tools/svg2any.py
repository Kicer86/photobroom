
import argparse
from io import BytesIO
from PIL import Image
from defusedxml import ElementTree as ET

import cairosvg


def get_svg_size(input_svg):
    tree = ET.parse(input_svg)
    root = tree.getroot()

    width_str = root.get('width')
    height_str = root.get('height')

    width = int(float(width_str.rstrip('pt')))
    height = int(float(height_str.rstrip('pt')))

    return width, height


def svg_to_any(input_svg, output_any, desired_size):
    default_width, default_height = get_svg_size(input_svg)

    width = default_width if desired_size[0] == -1 else desired_size[0]
    height = default_height if desired_size[1] == -1 else desired_size[1]

    buffer = BytesIO()
    cairosvg.svg2png(url=input_svg, write_to=buffer, output_width=width, output_height=height)
    buffer.seek(0)
    img = Image.open(buffer).convert("RGBA")

    # replace alpha with white color
    white_background = Image.new("RGB", img.size, color=(255, 255, 255))
    white_background.paste(img, mask=img)
    img = white_background

    with open(output_any, 'wb') as output_file:
        img.save(output_file, sizes=[(width, height)])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="SVG to any converter.")
    parser.add_argument("input_svg", help="SVG file path")
    parser.add_argument("output_file", help="Output image path")
    parser.add_argument("--width", type=int, default=-1, help="Output file width")
    parser.add_argument("--height", type=int, default=-1, help="Output file height")
    parser.add_argument("--check-requirements", action='store_true', default=False, help="Do nothing, just exit. Used for dependencies check.")

    args = parser.parse_args()
    if not args.check_requirements:
        svg_to_any(args.input_svg, args.output_file, (args.width, args.height))
