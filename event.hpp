#pragma once

#include <sstream>

class Event {
  std::string _raw;

  std::string attribute(std::string attribute) {
    std::stringstream raw_stream(_raw);
    std::string line;
    while (getline(raw_stream, line, '\n')) {
      if (starts_with(line, attribute)) {
        int index = attribute.length() + 1;  // include colon
        return line.substr(index, -1);
      }
    }
    return "";
  }

  bool starts_with(std::string source, std::string prefix) {
    return (source.rfind(prefix, 0) == 0) ? true : false;
  }

  tm to_time_info(std::string vcard_datetime) {
    struct tm time_info = { 0 };
    time_info.tm_year = std::atoi(vcard_datetime.substr(0, 4).c_str()) - 1900;
    time_info.tm_mon = std::atoi(vcard_datetime.substr(4, 2).c_str()) - 1;
    time_info.tm_mday = std::atoi(vcard_datetime.substr(6, 2).c_str());
    time_info.tm_hour = std::atoi(vcard_datetime.substr(9, 2).c_str());
    time_info.tm_min = std::atoi(vcard_datetime.substr(11, 2).c_str());
    time_info.tm_sec = std::atoi(vcard_datetime.substr(13, 2).c_str());
    return time_info;
  }

public:
  Event(std::string raw) {
    _raw = raw;
  }

  std::string summary() {
    return attribute("SUMMARY");
  }

  std::string formatted_start(std::string fmt) {
    std::string attr = attribute("DTSTART");
    struct tm time_info = to_time_info(attr);
    char formatted[100];
    strftime(formatted, sizeof(formatted), fmt.c_str(), &time_info);
    std::string str = formatted;
    return str;
  }

  std::string duration() {
    return attribute("DURATION");
  }

  bool is_all_day() {
    return duration() == "P1D";
  }
};
