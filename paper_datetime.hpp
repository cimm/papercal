/*
  Gets the current date and time from an NTP server and can format the result.
  Requires an active WiFi network connection.
*/

#include <WiFi.h>
#include <time.h>

class PaperDatetime {
  const char* ntp_server = "pool.ntp.org";
  const long gmt_offset_sec = 3600;  // = CET or GMT+1
  const int daylight_offset_sec = 3600;

public:
  struct tm time_info;

  void fetch() {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
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