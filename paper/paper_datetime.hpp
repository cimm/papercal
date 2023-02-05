#pragma once

#include <WiFi.h>
#include <time.h>

class PaperDatetime {
public:
  struct tm time_info;

  void fetch(const char* time_zone, const char* ntp_pool = "pool.ntp.org") {
    configTzTime(time_zone, ntp_pool);
    while (!getLocalTime(&time_info)) {
      delay(500);
    }
  }

  void format(const char* fmt, char* out_formatted, size_t out_length) {
    strftime(out_formatted, 100, fmt, &time_info);
  }
};
