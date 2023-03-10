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
  }
};
