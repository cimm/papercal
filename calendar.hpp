#pragma once

#include <HTTPClient.h>
#include <time.h>
#include <vector>
#include <sstream>
#include "tinyxml2/tinyxml2.cpp"
#include "event.hpp"

class Calendar {
  std::string _user;
  std::string _password;
  std::string _calendar_url;

  std::string utc_xml_formatted(tm local_time_info) {
    time_t utc_timestamp = mktime(&local_time_info);
    tm utc_time_info = *gmtime(&utc_timestamp);
    char formatted[17];
    strftime(formatted, 17, "%Y%m%dT%H%M%SZ", &utc_time_info);
    return std::string(formatted);
  }

  std::vector<Event> response_payload_to_events(std::string payload, tm start) {
    tinyxml2::XMLDocument doc;
    doc.Parse(payload.c_str());
    std::vector<Event> events;
    for (tinyxml2::XMLElement* child = doc.RootElement()->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
      Event event(child->FirstChildElement("propstat")->FirstChildElement("prop")->FirstChildElement("C:calendar-data")->GetText());
      if (event.valid_on(start)) { events.push_back(event); }
    }
    return events;
  }

  static bool by_start(Event event1, Event event2) {
    tm start1 = event1.start();
    tm start2 = event2.start();
    return (mktime(&start1) < mktime(&start2));
  }

public:
  std::string last_error_message;

  Calendar(std::string user, std::string password, std::string calendar_url) {
    _user = user;
    _password = password;
    _calendar_url = calendar_url;
  }

  bool error() {
    return !last_error_message.empty();
  }

  bool is_valid() {
    last_error_message.clear();
    HTTPClient http;
    http.begin(_calendar_url.c_str());
    http.setAuthorization(_user.c_str(), _password.c_str());
    int response_code = http.sendRequest("PROPFIND", "");
    http.end();
    if (response_code == HTTP_CODE_MULTI_STATUS) { return true; }
    last_error_message = "Calendar not found";
    return false;
  }

  std::vector<Event> events(tm start) {
    last_error_message.clear();
    tm end_of_day = { 0 };
    end_of_day.tm_year = start.tm_year;
    end_of_day.tm_mon = start.tm_mon;
    end_of_day.tm_mday = start.tm_mday;
    end_of_day.tm_hour = 23;
    end_of_day.tm_min = 59;
    end_of_day.tm_sec = 59;
    std::stringstream query_stream;
    query_stream
      << "<C:calendar-query xmlns:D='DAV:' xmlns:C='urn:ietf:params:xml:ns:caldav'>"
      << "  <D:prop>"
      << "    <C:calendar-data>"
      << "      <C:expand start='" << utc_xml_formatted(start) << "' end='" << utc_xml_formatted(end_of_day) << "'/>"
      << "    </C:calendar-data>"
      << "  </D:prop>"
      << "  <C:filter>"
      << "    <C:comp-filter name='VCALENDAR'>"
      << "      <C:comp-filter name='VEVENT'>"
      << "        <C:time-range start='" << utc_xml_formatted(start) << "' end='" << utc_xml_formatted(end_of_day) << "'/>"
      << "      </C:comp-filter>"
      << "    </C:comp-filter>"
      << "  </C:filter>"
      << "</C:calendar-query>"
      << std::endl;
    auto query = query_stream.str();
    HTTPClient http;
    http.begin(_calendar_url.c_str());
    http.setAuthorization(_user.c_str(), _password.c_str());
    int response_code = http.sendRequest("REPORT", query.c_str());
    std::vector<Event> events;
    if (response_code == HTTP_CODE_MULTI_STATUS) {
      std::string payload = http.getString().c_str();
      events = response_payload_to_events(payload, start);
    }
    http.end();
    if (events.empty()) { last_error_message = "Calendar found no events"; }
    sort(events.begin(), events.end(), by_start);
    return events;
  }
};
