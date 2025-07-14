#pragma once

// Display pins
#define EPD_BUSY 34
#define EPD_CS   22
#define EPD_DC   15
#define EPD_EN   12
#define EPD_RST  13

#include <GxEPD2_BW.h>

class PaperDisplay {
  const uint8_t PAUZE = 50;  // miliseconds

public:
  GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> panel = GxEPD2_420(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);

  PaperDisplay() {
    pinMode(EPD_EN, OUTPUT);
    pinMode(EPD_RST, OUTPUT);
  }

  void enable_display() {
    digitalWrite(EPD_EN, LOW);
    digitalWrite(EPD_RST, LOW);
    delay(PAUZE);
    digitalWrite(EPD_RST, HIGH);
    delay(PAUZE);
    panel.init();
    Serial.println("Display enabled");
  }

  void disable_display() {
    digitalWrite(EPD_EN, HIGH);
    digitalWrite(EPD_RST, LOW);
    delay(PAUZE);
    digitalWrite(EPD_RST, HIGH);
    Serial.println("Display disabled");
  }
};
