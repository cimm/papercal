#pragma once

// Sound pins
#define PIEZO 26

class PaperSound {
  long pauze = 300000;  // microseconds

public:
  PaperSound() {
    pinMode(PIEZO, OUTPUT);
  }

  void clicks(uint16_t times) {
    for (uint16_t i = 1; i <= times; i++) {
      if (i % 2) {
        digitalWrite(PIEZO, HIGH);
      } else {
        digitalWrite(PIEZO, LOW);
      }
      delayMicroseconds(pauze);
    }
  }
};
