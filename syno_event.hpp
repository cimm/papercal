/*
  Formats an event returned from a SynoCalendar.
*/

#include <ArduinoJson.h>

class SynoEvent {
  JsonVariantConst _json_event;

public:
  SynoEvent(JsonVariantConst json_event) {
    _json_event = json_event;
  }

  tm start() {
    struct tm time_info = { 0 };
    String start = _json_event["dtstart"];
    if (is_all_day() || start == "") { return time_info; }
    String start_without_tz = start;
    int index = start.indexOf(':');
    if (index >= 0) { start_without_tz = start.substring(index + 1, -1); };
    time_info.tm_year = start_without_tz.substring(0, 4).toInt() - 1900;
    time_info.tm_mon = start_without_tz.substring(4, 6).toInt() - 1;
    time_info.tm_mday = start_without_tz.substring(6, 8).toInt();
    time_info.tm_hour = start_without_tz.substring(9, 11).toInt();
    time_info.tm_min = start_without_tz.substring(11, 13).toInt();
    time_info.tm_sec = start_without_tz.substring(13, 15).toInt();
    return time_info;
  }

  void formatted_start(const char* fmt, char* out_formatted, size_t out_length) {
    struct tm time_info;
    time_info = start();
    strftime(out_formatted, 100, fmt, &time_info);
  }

  bool is_all_day() {
    return _json_event["is_all_day"];
  }

  String summary() {
    String summary = _json_event["summary"].as<const char*>();
    return summary;
  }
};