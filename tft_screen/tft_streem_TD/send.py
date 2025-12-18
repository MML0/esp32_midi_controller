import socket
import cv2
import numpy as np

# ===== UDP socket =====
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ===== List of ESP devices =====
esp_ip = [
    {"ip": "192.168.4.1"},  # ESP32 AP default IP
]

# ===== Screen size =====
WIDTH  = 128
HEIGHT = 160

def send_frame_to_esps():
    image_path = "jpg_out/snapshot.png"
    frame = cv2.imread(image_path)

    if frame is None:
        print("❌ Failed to read image.")
        return

    # Resize to 128x160
    frame = cv2.resize(frame, (WIDTH, HEIGHT), interpolation=cv2.INTER_AREA)

    # Convert to RGB565
    rgb565_frame = []
    for y in range(HEIGHT):
        for x in range(WIDTH):
            b, g, r = frame[y, x]  # OpenCV uses BGR
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            rgb565_frame.append(rgb565)

    # Convert to bytes
    data = bytearray()
    for val in rgb565_frame:
        data.append((val >> 8) & 0xFF)
        data.append(val & 0xFF)

    # Send to each ESP
    for esp in esp_ip:
        try:
            sock.sendto(data, (esp['ip'], 12345))  # match UDP port in ESP32
            print(f"✅ Sent {len(data)} bytes to {esp['ip']}")
        except Exception as er:
            print(f"❌ Failed to send to {esp['ip']}: {er}")

# ===== TouchDesigner Callbacks =====
def onOnToOff(channel, sampleIndex, val, prev):
    top = op('out1')  # TouchDesigner TOP
    top.save("jpg_out/snapshot.png")
    send_frame_to_esps()

def whileOn(channel, sampleIndex, val, prev):
    return

def onOffToOn(channel, sampleIndex, val, prev):
    return

def whileOff(channel, sampleIndex, val, prev):
    return

def onValueChange(channel, sampleIndex, val, prev):
    return
