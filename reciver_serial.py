import serial
import serial.tools.list_ports
import time

# ======= Auto-find ESP32 serial port =======
def find_esp32_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        try:
            ser = serial.Serial(p.device, 115200, timeout=0.5)
            # try reading a line to check if it's ESP32
            ser.flushInput()
            time.sleep(0.1)
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line.startswith('[') and line.endswith(']'):
                    print(f"ESP32 found on {p.device}")
                    return p.device
            ser.close()
        except:
            continue
    return None

# ======= Read data =======
def read_esp32(port):
    try:
        with serial.Serial(port, 115200, timeout=1) as ser:
            while True:
                if ser.in_waiting:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        print("Received:", line)
    except KeyboardInterrupt:
        print("Stopped.")
    except Exception as e:
        print("Error:", e)

# ======= Main =======
esp_port = find_esp32_port()
esp_port = find_esp32_port()
if esp_port:
    read_esp32(esp_port)
else:
    print("ESP32 not found on any COM port.")
