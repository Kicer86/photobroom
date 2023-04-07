import argparse
from io import BytesIO
from PIL import Image

import cairosvg


def svg_to_any(input_svg, output_any, desired_size):
    buffer = BytesIO()
    cairosvg.svg2png(url=input_svg, write_to=buffer, output_width=desired_size[0], output_height=desired_size[1])
    buffer.seek(0)
    img = Image.open(buffer)
    img = img.convert('RGB')

    with open(output_any, 'wb') as output_file:
        img.save(output_file, sizes=[desired_size])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="SVG to any converter.")
    parser.add_argument("input_svg", help="SVG file path")
    parser.add_argument("output_file", help="Output image path")
    parser.add_argument("--width", type=int, default=64, help="Output file width")
    parser.add_argument("--height", type=int, default=64, help="Output file height")

    args = parser.parse_args()

    svg_to_any(args.input_svg, args.output_file, (args.width, args.height))
