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
    configTime(0, 0, ntp_server);
    setenv("TZ", time_zone, 1);
    tzset();
    while (!getLocalTime(&time_info)) {
      delay(500);
    }
  }

  // formatted_out is an "out string" as values of pointers created in this
  // scope are emptied at the end of the function, so the pointer would
  // point to nothing
  void format(const char* fmt, char* out_formatted, size_t out_length) {
    strftime(out_formatted, 100, fmt, &time_info);
  }
};