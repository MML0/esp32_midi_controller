import serial
import serial.tools.list_ports
import time
import threading

data_list = []      # shared data
running = False     # thread running flag
thread = None       # reference to serial thread
PREFERRED_COM = "COM3"

# ======= Find ESP32 port =======
def find_esp32_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if p.device == PREFERRED_COM or any(k in (p.description+str(p.manufacturer)).lower() for k in ["cp210","ch340","usb serial"]):
            try:
                ser = serial.Serial(p.device, 115200, timeout=0.1)
                time.sleep(0.2)
                for _ in range(5):
                    if ser.in_waiting:
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        if line.startswith('[') and line.endswith(']'):
                            ser.close()
                            return p.device
                    time.sleep(0.05)
                ser.close()
            except:
                continue
    return None

# ======= Thread function =======
def read_thread():
    global running
    ser = None
    while running:
        try:
            if ser is None or not ser.is_open:
                esp_port = find_esp32_port()
                if esp_port:
                    ser = serial.Serial(esp_port, 115200, timeout=0.1)
                    time.sleep(0.5)  # ESP32 reset
                else:
                    time.sleep(1)
                    continue

            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    handle_data(line)

            time.sleep(0.001)

        except (serial.SerialException, OSError):
            if ser:
                ser.close()
            ser = None
            time.sleep(0.1)

# ======= Start thread safely =======
def start_serial_thread():
    global running, thread
    if running:       # already running, do nothing
        return
    running = True
    thread = threading.Thread(target=read_thread, daemon=True)
    thread.start()
    print("ESP32 thread started.")

# ======= Stop thread =======
def stop_serial_thread():
    global running, thread
    running = False
    if thread:
        thread.join(timeout=0.5)
        thread = None
    print("ESP32 thread stopped.")

# ======= TouchDesigner callbacks =======
# This is in a Text DAT set to "Run On Start / Play Start"

def onStart():
    start_serial_thread()
    return


def onPlayStateChange(state):
    if state:  # True = timeline is now playing
        start_serial_thread()  # start only once
    else:      # False = paused/stopped
        stop_serial_thread()   # stop thread safely
    return
  
# ======= Frame update for CHOPs =======
def onFrameStart(frame):
    try:
        push, toggle, analog = op('esp32_reader').data_list
        for i, v in enumerate(push):
            op('push_CHOP')['chan'+str(i)] = v
        for i, v in enumerate(toggle):
            op('toggle_CHOP')['chan'+str(i)] = v
        for i, v in enumerate(analog):
            op('analog_CHOP')['chan'+str(i)] = v
    except:
        pass
    return
