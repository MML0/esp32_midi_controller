#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "photo.h"

// ===== TFT PINS =====
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  SPI.begin(18, -1, 23);   // SCK, MISO(not used), MOSI

  tft.initR(INITR_BLACKTAB);   // change if needed
  tft.setRotation(1);

  drawPhoto();
}

void loop() {
}

// ===== DRAW IMAGE FROM FLASH (PROGMEM) =====
void drawPhoto() {
  tft.startWrite();
  tft.setAddrWindow(0, 0, 128, 160);

  for (int i = 0; i < 20480; i++) {
    uint16_t color = pgm_read_word(&photo[i]);
    tft.pushColor(color);
  }

  tft.endWrite();
}
