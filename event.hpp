#pragma once

#include <sstream>
#include <time.h>

class Event {
  std::string _raw;

  std::string property(std::string component, std::string property) {
    std::stringstream raw_stream(_raw);
    std::string line;
    bool in_component = false;
    int property_value_index = property.length() + 1;  // include colon
    while (getline(raw_stream, line, '\n')) {
      if (in_component && line.find(property) != std::string::npos) {
        return line.substr(property_value_index, -1);
      }
      if (line.find("BEGIN:" + component) != std::string::npos) { in_component = true; }
      if (line.find("END:" + component) != std::string::npos) { break; }
    }
    return "";
  }

  tm to_time_info(std::string datetime) {  // rfc5545 datetime in UTC
    tm utc_time_info = { 0 };
    utc_time_info.tm_year = std::atoi(datetime.substr(0, 4).c_str()) - 1900;
    utc_time_info.tm_mon = std::atoi(datetime.substr(4, 2).c_str()) - 1;
    utc_time_info.tm_mday = std::atoi(datetime.substr(6, 2).c_str());
    if (datetime.size() > 8) {
      utc_time_info.tm_hour = std::atoi(datetime.substr(9, 2).c_str());
      utc_time_info.tm_min = std::atoi(datetime.substr(11, 2).c_str());
      utc_time_info.tm_sec = std::atoi(datetime.substr(13, 2).c_str());
    }
    time_t utc_timestamp = mktime(&utc_time_info) - _timezone;  // timegm not implemented on esp32
    return *localtime(&utc_timestamp);
  }

public:
  Event(std::string raw) {
    _raw = raw;
  }

  std::string summary() {
    return property("VEVENT", "SUMMARY");
  }

  tm start() {
    std::string attr = (is_all_day()) ? property("VEVENT", "DTSTART;VALUE=DATE") : property("VEVENT", "DTSTART");
    return to_time_info(attr);
  }

  std::string formatted_start(std::string fmt) {
    tm time_info = start();
    char formatted[100];
    strftime(formatted, sizeof(formatted), fmt.c_str(), &time_info);
    std::string str = formatted;
    return str;
  }

  std::string duration() {
    return property("VEVENT", "DURATION");
  }

  bool valid_on(tm day) {  // Synology also returns single all days events the day after, see https://stackoverflow.com/q/75379228
    return (duration() == "P1D" && start().tm_mday != day.tm_mday) ? false : true;
  }

  bool is_all_day() {
    return duration() == "P1D";  // TODO all day events an be longer than 1 day
  }
};
