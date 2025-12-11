## ESP32 MIDI Controller – Input Test Code

This code is for an ESP32 MIDI controller.  
It sends analog, toggle switch, and digital button data as MIDI messages through USB or WiFi.

### Pin Assignments
Push buttons: 2, 4, 5, 12
Toggle switches: 13, 14, 15, 16
Analog pins: 32, 33, 34, 35, 36, 39

### Features
- Converts analog inputs to MIDI values
- Converts toggle switches to MIDI on/off
- Converts push buttons to MIDI on/off
- Can send MIDI over USB Serial or WiFi (depending on your setup)
- Prints debug data like:
  Push:0,1,1,0 | Toggle:1,1,0,0 | Analog:1234,2000,3000,4095,1000,800

### How to Use
1. Upload code to ESP32
2. Open Serial Monitor at 115200
3. Connect to your MIDI software or WiFi MIDI receiver
4. Move pots or flip switches to send MIDI data
