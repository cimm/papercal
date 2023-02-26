#pragma once

#include "paper/paper_display.hpp"

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <GxEPD2_BW.h>

class UI {
  PaperDisplay* _display;
  uint16_t _height;
  uint16_t _width;
  uint8_t _current_row = 1;
  static const uint8_t SIDEBAR_WIDTH = 90;
  static const uint8_t PADDING = 5;
  static const uint8_t LINE_HEIGHT = 40;

  uint16_t text_width(std::string text) {
    int16_t x, y;
    uint16_t width, height;
    _display->panel.getTextBounds(text.c_str(), 0, 0, &x, &y, &width, &height);
    return width;
  }

public:
  UI(PaperDisplay* display) {
    _display = display;
    _height = display->panel.height();
    _width = display->panel.width();
  }

  void reset() {
    _display->panel.fillScreen(GxEPD_WHITE);
    _display->panel.fillRect(0, 0, SIDEBAR_WIDTH, _height, GxEPD_BLACK);  // left sidebar
    _display->panel.setCursor(0, 3 * PADDING);
  }

  void row(std::string left, std::string right, bool bold = false) {
    if (bold) {
      _display->panel.setFont(&FreeSansBold12pt7b);
    } else {
      _display->panel.setFont(&FreeSans12pt7b);
    }
    uint16_t y = _current_row * LINE_HEIGHT - (2 * PADDING);
    _display->panel.setCursor(SIDEBAR_WIDTH - text_width(left) - 3 * PADDING, y);
    _display->panel.setTextColor(GxEPD_WHITE);
    _display->panel.print(left.c_str());
    _display->panel.setCursor(SIDEBAR_WIDTH + PADDING, y);
    _display->panel.setTextColor(GxEPD_BLACK);
    _display->panel.print(right.c_str());
    _current_row++;
  }

  void footer_left(std::string text) {
    _display->panel.setFont();
    _display->panel.setTextSize(1);
    _display->panel.setCursor(SIDEBAR_WIDTH + PADDING, _height - 2 * PADDING);
    _display->panel.print(text.c_str());
  }

  void footer_right(std::string text) {
    _display->panel.setFont();
    _display->panel.setTextSize(1);
    _display->panel.setCursor(_width - text_width(text) - PADDING, _height - 2 * PADDING);
    _display->panel.print(text.c_str());
  }
};
