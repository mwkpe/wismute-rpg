import argparse
import os
from PIL import Image


def gen_gpl(name, colors, hex_colors, filepath):
  header = f"""GIMP Palette
Name: {name}
Columns: {len(colors)}
"""
  with open(filepath, 'w', newline='\n') as file:
    file.write(header)
    for i in range(len(colors)):
      r, g, b = colors[i]
      hex = hex_colors[i]
      file.write(f"{r}\t{g}\t{b}\t{hex}\n")


def gen_code(hex_colors):
  print("static constexpr std::array<glm::vec4, color_count> colors = {")
  print("    apeiron::engine::as_rgba_norm(\"#ff00ffff\"),  // Invalid")

  for hex in hex_colors[:-1]:
    print(f"    apeiron::engine::as_rgba_norm(\"#{hex}\"),")

  print(f"    apeiron::engine::as_rgba_norm(\"#{hex_colors[-1]}\")")
  print("};\n")
  print("std::map<std::uint32_t, std::uint8_t> color_map = {")
  print("    {0xff00ffff, 0},")

  index = 1

  for hex in hex_colors[:-1]:
    print(f"    {{0x{hex}, {index}}},")
    index += 1

  print(f"    {{0x{hex_colors[-1]}, {index}}}")
  print("};")


def main():
  parser = argparse.ArgumentParser(description="Generate palette code from an image")
  parser.add_argument("image_path", type=str, help="Path to the palette image")
  args = parser.parse_args()

  image = Image.open(args.image_path)

  if image.height != 1 or image.width > 64:
    raise ValueError("Invalid image dimensions")

  pixels = image.load()
  colors = []
  hex_colors = []

  for i in range(image.width):
    pixel = pixels[i, 0]

    if len(pixel) == 4:  # RGBA
      r, g, b, _ = pixel
    else:  # RGB
      r, g, b = pixel

    colors.append([r, g, b])
    hex_colors.append(f"{r:02x}{g:02x}{b:02x}ff")

  root = os.path.splitext(args.image_path)[0]
  name = os.path.basename(root)
  filepath = f"{root}.gpl"

  gen_gpl(name, colors, hex_colors, filepath)
  gen_code(hex_colors)

if __name__ == "__main__":
  try:
    main()
  except Exception as e:
    print(f"Error: {e}")
