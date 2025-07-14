#pragma once

// Sound pins
#define PIEZO 26

class PaperSound {
  const uint16_t PAUZE = 300;  // miliseconds

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
      delay(PAUZE);
    }
    Serial.println("Played some sound");
  }
};
