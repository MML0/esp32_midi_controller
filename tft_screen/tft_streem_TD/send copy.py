import socket
import cv2
import numpy as np

# ===== UDP socket =====
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ===== List of ESP devices =====
esp_ip = [
    {"ip": "192.168.93.250"},  # <-- put your ESP IP(s) here
]

# ===== Screen size =====
WIDTH  = 64
HEIGHT = 48

def send_frame_to_esps():
    image_path = "jpg_out/snapshot.png"
    frame = cv2.imread(image_path)

    if frame is None:
        print("❌ Failed to read image.")
        return

    # Resize to 64x48
    frame = cv2.resize(frame, (WIDTH, HEIGHT), interpolation=cv2.INTER_AREA)

    # Convert to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Threshold to 1-bit (black/white)
    _, bw = cv2.threshold(gray, 127, 1, cv2.THRESH_BINARY)

    # Pack bits → 384 bytes
    packed = np.packbits(bw.astype(np.uint8), axis=None)

    # Add header byte (0xAA)
    data = b'\xAA' + packed.tobytes()

    # Send to each ESP
    for esp in esp_ip:
        try:
            sock.sendto(data, (esp['ip'], 8266))
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
