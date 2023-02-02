// Display pins
#define EPD_BUSY 34
#define EPD_CS   22
#define EPD_DC   15
#define EPD_EN   12
#define EPD_RST  13

#include <GxEPD2_BW.h>

class PaperDisplay {
public:
  GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> panel = GxEPD2_420(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY);

  PaperDisplay() {
    pinMode(EPD_EN, OUTPUT);
    pinMode(EPD_RST, OUTPUT);
  }

  void enable_display() {
    digitalWrite(EPD_EN, LOW);
    digitalWrite(EPD_RST, LOW);
    delay(50);
    digitalWrite(EPD_RST, HIGH);
    delay(50);
    panel.init();
  }

  void disable_display() {
    digitalWrite(EPD_EN, HIGH);
    digitalWrite(EPD_RST, LOW);
    delay(50);
    digitalWrite(EPD_RST, HIGH);
  }
};
