#pragma once

#define BUTTON_1_PIN 14

class PaperDevice {
public:
  void sleep_timer_and_button_wakeup(uint64_t sleep_seconds) {
    esp_sleep_enable_timer_wakeup(sleep_seconds * 1000000);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1_PIN, 0);
    Serial.printf("Wakeup after %lld sec or button on pin %d", sleep_seconds, BUTTON_1_PIN);
    esp_deep_sleep_start();
  }
};
