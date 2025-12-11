import serial
import serial.tools.list_ports
import time

data_list = []  # store parsed values
PREFERRED_COM = "COM4"  # change this to your preferred port

def handle_data(line):
    """Parse line from ESP32 and store in data_list"""
    try:
        values = list(map(int, line.strip('[]').split(',')))
        push   = values[0:4]
        toggle = values[4:8]
        analog = values[8:14]
        global data_list
        data_list = [push, toggle, analog]
        print(analog[-2:])
    except:
        pass

# ======= Auto-find ESP32 serial port =======
def find_esp32_port():
    ports = list(serial.tools.list_ports.comports())

    # 1️⃣ Try preferred COM first
    if PREFERRED_COM:
        for p in ports:
            if p.device == PREFERRED_COM:
                try:
                    ser = serial.Serial(p.device, 115200, timeout=1)
                    ser.flushInput()
                    time.sleep(1)
                    if ser.in_waiting:
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        if line.startswith('[') and line.endswith(']'):
                            ser.close()
                            return p.device
                    ser.close()
                except:
                    pass

    # 2️⃣ Try name-based scan (e.g., CP210x / CH340)
    for p in ports:
        desc = (p.description + str(p.manufacturer)).lower()
        if any(k in desc for k in ["cp210", "ch340", "usb serial"]):
            try:
                print('found by name')
                ser = serial.Serial(p.device, 115200, timeout=1)
                ser.flushInput()
                time.sleep(1)
                if ser.in_waiting:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line.startswith('[') and line.endswith(']'):
                        print('connected')
                        ser.close()
                        return p.device
                ser.close()
            except:
                continue

    # 3️⃣ Fallback: try all remaining ports
    for p in ports:
        try:
            ser = serial.Serial(p.device, 115200, timeout=0.1)
            ser.flushInput()
            time.sleep(0.1)
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line.startswith('[') and line.endswith(']'):
                    ser.close()
                    return p.device
            ser.close()
        except:
            continue

    return None

# ======= Read data continuously with reconnect =======
def read_loop():
    ser = None
    esp_port = None

    while True:
        try:
            if ser is None or not ser.is_open:
                esp_port = find_esp32_port()
                if esp_port:
                    print(f"Connecting to ESP32 on {esp_port}...")
                    ser = serial.Serial(esp_port, 115200, timeout=1)
                    time.sleep(1)  # wait for ESP32 reset
                else:
                    print("ESP32 not found. Retrying...")
                    # time.sleep(1)
                    continue

            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    handle_data(line)
            time.sleep(0.03)

        except (serial.SerialException, OSError) as e:
            print(f"Connection lost: {e}. Re-scanning ports...")
            try:
                if ser:
                    ser.close()
            except:
                pass
            ser = None
            time.sleep(0.1)

        except KeyboardInterrupt:
            print("Stopped by user")
            if ser:
                ser.close()
            break

# ======= Main =======
if __name__ == "__main__":
    read_loop()
