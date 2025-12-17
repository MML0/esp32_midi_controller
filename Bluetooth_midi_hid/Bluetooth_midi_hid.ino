#include <Arduino.h>
#include <BLEMidi.h>

#define POT_PIN 34     // ADC pin connected to potentiometer
#define CC_NUM  7      // MIDI CC number (volume)
#define CHANNEL 1      // MIDI channel 1 (0 in library is channel 1 internally)

int lastVal = -1;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // 0–4095
  Serial.println("Initializing BLE MIDI...");
  BLEMidiServer.begin("Basic MIDI device");
  Serial.println("Waiting for connections...");
  // BLEMidiServer.enableDebugging();  // Uncomment to see debug messages
}

void loop() {
  if (BLEMidiServer.isConnected()) {
    int raw = analogRead(POT_PIN);
    int val = raw >> 5; // scale 0–4095 to 0–127

    if (val != lastVal) {         // only send if changed
      lastVal = val;
      BLEMidiServer.controlChange(CHANNEL - 1, CC_NUM, val);
      Serial.printf("Sent CC %d = %d on CH %d\n", CC_NUM, val, CHANNEL);
    }
  }

  delay(5); // small delay to avoid flooding BLE
}
