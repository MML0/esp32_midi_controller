from PIL import Image
import struct

# ===== SETTINGS =====
input_folder = "tft_screen/"                 # folder with PNGs
output_file = "tft_screen/128x160_TFT1.8_esp32/photo.h"
output_file = "tft_screen/photo.h"
num_frames = 20                             # total number of frames
array_prefix = "photo"                        # array names: photo1, photo2, ...
width, height = 128, 160                      # TFT size

# ===== OPEN HEADER FILE =====
with open(output_file, "w") as f:
    f.write("#pragma once\n")
    f.write("#include <Arduino.h>\n\n")

    # ===== PROCESS EACH IMAGE =====
    for i in range(1, num_frames + 1):
        # filename = f"{input_folder}photo{i}.png"
        filename = f"{input_folder}BG-001-LiL LCD-F001_{i-1:05d}.png"
        img = Image.open(filename).convert("RGB")
        img = img.rotate(90, expand=True)
        img = img.resize((width, height))
        width, height = img.size
        array_name = f"{array_prefix}{i}"
        f.write(f"const uint16_t {array_name}[{width*height}] PROGMEM = {{\n")

        for y in range(height):
            for x in range(width):
                r, g, b = img.getpixel((x, y))
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                f.write(f"0x{rgb565:04X}, ")
            f.write("\n")
        f.write("};\n\n")

    # ===== GENERATE FRAMES POINTER ARRAY =====
    f.write("// Array of pointers to frames\n")
    f.write(f"const uint16_t* frames[] = {{\n")
    for i in range(1, num_frames + 1):
        f.write(f"  {array_prefix}{i}")
        if i != num_frames:
            f.write(",")
        f.write("\n")
    f.write("};\n")
    f.write(f"const int numFrames = sizeof(frames) / sizeof(frames[0]);\n")

print(f"{output_file} generated successfully with {num_frames} frames!")
