#pragma once

#include <WiFi.h>
#include <time.h>

class PaperDatetime {
  const uint8_t PAUZE = 500;  // miliseconds

public:
  tm time_info;

  void fetch(const char* time_zone, const char* ntp_pool = "pool.ntp.org") {
    configTzTime(time_zone, ntp_pool);
    while (!getLocalTime(&time_info)) {
      delay(PAUZE);
    }
    char formatted_date[64];
    strftime(formatted_date, sizeof(formatted_date), "%d/%m %H:%M", &time_info);
    Serial.printf("It is %s according to %s\n", formatted_date, ntp_pool);
  }
};
