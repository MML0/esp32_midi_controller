#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "photo.h"

#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

#define TFT_W 128
#define TFT_H 160

void setup() {
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // Draw the image from photo.h
  tft.drawRGBBitmap(0, 0, photo, TFT_W, TFT_H);
}

void loop() {
  // nothing
}
