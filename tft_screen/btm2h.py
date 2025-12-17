from PIL import Image
import struct

input_image = "tft_screen/photo.png"
output_file = "tft_screen/128x160_TFT1.8_esp32/photo.h"
array_name = "photo"
width, height = 128, 160

# open and resize
img = Image.open(input_image).convert("RGB")
img = img.resize((width, height))

with open(output_file, "w") as f:
    f.write("#pragma once\n")
    f.write("#include <Arduino.h>\n\n")
    f.write(f"const uint16_t {array_name}[{width*height}] PROGMEM = {{\n")

    for y in range(height):
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            f.write(f"0x{rgb565:04X}, ")
        f.write("\n")
    f.write("};\n")

print("photo.h (RGB565) generated successfully")
