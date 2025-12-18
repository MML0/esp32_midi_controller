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

void setup() {
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  // tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  // for (int i = 0; i < numFrames; i++) {
  //   tft.drawRGBBitmap(0, 0, frames[i], TFT_W, TFT_H);
  //   delay(100); // ~30 FPS
  // }
  // delay(2000); // ~30 FPS
    for (int i = 0; i < 16; i++) {
    tft.drawRGBBitmap(0, 0, frames[i], TFT_W, TFT_H);
    delay(100); // ~30 FPS
  }
  delay(500); // ~30 FPS

  tft.drawRGBBitmap(0, 0, frames[16], TFT_W, TFT_H);
  delay(2000); // ~30 FPS

  tft.drawRGBBitmap(0, 0, frames[17], TFT_W, TFT_H);
  delay(700); // ~30 FPS
  tft.drawRGBBitmap(0, 0, frames[18], TFT_W, TFT_H);
  delay(700); // ~30 FPS
  tft.drawRGBBitmap(0, 0, frames[19], TFT_W, TFT_H);
  delay(700); // ~30 FPS

  tft.drawRGBBitmap(0, 0, frames[16], TFT_W, TFT_H);
  delay(1000); // ~30 FPS
 
  tft.drawRGBBitmap(0, 0, frames[17], TFT_W, TFT_H);
  delay(700); // ~30 FPS
  tft.drawRGBBitmap(0, 0, frames[18], TFT_W, TFT_H);
  delay(700); // ~30 FPS
  tft.drawRGBBitmap(0, 0, frames[19], TFT_W, TFT_H);
  delay(700); // ~30 FPS

  // tft.drawRGBBitmap(0, 0, frames[17], TFT_W, TFT_H);
  // delay(500); // ~30 FPS
  // tft.drawRGBBitmap(0, 0, frames[18], TFT_W, TFT_H);
  // delay(500); // ~30 FPS
  // tft.drawRGBBitmap(0, 0, frames[19], TFT_W, TFT_H);
  // delay(700); // ~30 FPS
}
