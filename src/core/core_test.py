
import sys
from PIL import Image
import os

def crop_image(input_path, output_path, left=0, top=0, right=0, bottom=0):
    original_image = Image.open(input_path)
    cropped_image = original_image.crop((left, top, original_image.width - right, original_image.height - bottom))
    cropped_image = cropped_image.resize(original_image.size);
    cropped_image.save(output_path)

def move_image(input_path, output_path, x_offset=0, y_offset=0):
    original_image = Image.open(input_path)
    moved_image = Image.new("RGB", original_image.size)
    moved_image.paste(original_image, (x_offset, y_offset))
    moved_image.save(output_path)

def rotate_image(input_path, output_path, angle=0):
    original_image = Image.open(input_path)
    rotated_image = original_image.rotate(angle)
    rotated_image.save(output_path)

def process_image(input_path, output_dir):
    filename, ext = os.path.splitext(os.path.basename(input_path))
    os.makedirs(output_dir, exist_ok=True)

    crop_image(input_path, os.path.join(output_dir, f"{filename}_cropped{ext}"), left=40, top=40, right=40, bottom=40)
    move_image(input_path, os.path.join(output_dir, f"{filename}_moved_left{ext}"), x_offset=-20)
    move_image(input_path, os.path.join(output_dir, f"{filename}_moved_right{ext}"), x_offset=20)
    rotate_image(input_path, os.path.join(output_dir, f"{filename}_rotated{ext}"), angle=10)
    rotate_image(input_path, os.path.join(output_dir, f"{filename}_rotated_negative{ext}"), angle=-10)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input_image_path output_directory_path")
        sys.exit(1)

    input_path = sys.argv[1]
    output_dir = sys.argv[2]
    process_image(input_path, output_dir)
