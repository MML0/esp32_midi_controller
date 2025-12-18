#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "photo.h"

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

#define TFT_W 128
#define TFT_H 160

// Wi-Fi AP settings
const char* ssid = "ESP32_Frame_AP";
const char* password = "12345678";

WiFiUDP udp;
const unsigned int udpPort = 12345;  // port to listen for frames
unsigned long lastFrameTime = 0;

// Buffer for receiving a frame (128x160 RGB565 = 32768 bytes)
uint16_t udpFrame[TFT_W * TFT_H];

void setup() {
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  // Start Wi-Fi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  udp.begin(udpPort);
  Serial.printf("Listening on UDP port %d\n", udpPort);
}

void loop() {
  // Check for UDP packet
  int packetSize = udp.parsePacket();
  bool gotFrame = false;

  if (packetSize >= TFT_W * TFT_H * 2) { // full RGB565 frame
    udp.read((uint8_t*)udpFrame, TFT_W * TFT_H * 2);
    tft.drawRGBBitmap(0, 0, udpFrame, TFT_W, TFT_H);
    lastFrameTime = millis();
    gotFrame = true;
  }

  // If no frame received for 1 second, continue normal animation
  if (millis() - lastFrameTime > 1000) {
    for (int i = 0; i < numFrames; i++) {
      tft.drawRGBBitmap(0, 0, frames[i], TFT_W, TFT_H);
      delay(100); // adjust frame speed
      // Check again if a UDP frame arrived mid-animation
      packetSize = udp.parsePacket();
      if (packetSize >= TFT_W * TFT_H * 2) {
        udp.read((uint8_t*)udpFrame, TFT_W * TFT_H * 2);
        tft.drawRGBBitmap(0, 0, udpFrame, TFT_W, TFT_H);
        lastFrameTime = millis();
        break; // exit animation loop to show UDP frame
      }
    }
  }

  delay(10); // small delay to avoid tight loop
}
