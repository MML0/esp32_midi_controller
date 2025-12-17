from PIL import Image

# ===== SETTINGS =====
input_image = "tft_screen/photo.bmp"      # input image path
output_file = "tft_screen/128x160_TFT1.8_esp32/photo.h"        # output C header file
array_name = "photo"          # name of C array
width, height = 128, 160       # TFT size


# ===== OPEN & RESIZE IMAGE =====
img = Image.open(input_image).convert("RGB")
img = img.resize((width, height))

# ===== RGB888 → RGB565 =====
def rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

pixels = []

# IMPORTANT: y goes TOP → BOTTOM (0 → height-1)
for y in range(height):
    for x in range(width):
        r, g, b = img.getpixel((x, y))
        color = rgb565(r, g, b)

        # 🔁 BYTE SWAP (UNCOMMENT IF COLORS ARE WRONG)
        # color = ((color << 8) & 0xFF00) | (color >> 8)

        pixels.append(color)

# ===== WRITE HEADER =====
with open(output_file, "w") as f:
    f.write("#pragma once\n")
    f.write("#include <Arduino.h>\n\n")
    f.write(f"const uint16_t {array_name}[{width*height}] PROGMEM = {{\n")

    for i, p in enumerate(pixels):
        f.write(f"0x{p:04X}, ")
        if (i + 1) % 16 == 0:
            f.write("\n")

    f.write("\n};\n")

print("photo.h generated successfully")
