/*
  Gets the current date and time from an NTP server and can format the result.
  Requires an active WiFi network connection.
*/

#include <WiFi.h>
#include <time.h>

class PaperDatetime {
  const char* ntp_server = "pool.ntp.org";

public:
  struct tm time_info;

  void fetch(const char* time_zone) {
    configTzTime(time_zone, ntp_server);
    while (!getLocalTime(&time_info)) {
      delay(500);
    }
  }

  void format(const char* fmt, char* out_formatted, size_t out_length) {
    strftime(out_formatted, 100, fmt, &time_info);
  }
};
