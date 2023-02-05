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

  std::string utc_xml_formatted(tm time_info) {
    time_t utc_timestamp = mktime(&time_info);
    tm utc_time_info = *gmtime(&utc_timestamp);
    char formatted[17];
    strftime(formatted, 17, "%Y%m%dT%H%M%SZ", &utc_time_info);
    return std::string(formatted);
  }

  std::vector<Event> response_payload_to_events(std::string payload) {
    tinyxml2::XMLDocument doc;
    doc.Parse(payload.c_str());
    std::vector<Event> events;
    for (tinyxml2::XMLElement* child = doc.RootElement()->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
      Event event(child->FirstChildElement("propstat")->FirstChildElement("prop")->FirstChildElement("C:calendar-data")->GetText());
      events.push_back(event);
    }
    return events;
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
    http.addHeader("Content-Type", "application/xml");
    int response_code = http.sendRequest("PROPFIND", "");
    http.end();
    if (response_code == HTTP_CODE_MULTI_STATUS) { return true; }
    last_error_message = "Invalid calendar URL";
    return false;
  }

  std::vector<Event> events(tm start, tm end) {
    last_error_message.clear();
    std::stringstream query_stream;
    query_stream
      << "<?xml version='1.0' encoding='utf-8' ?>"
      << "<C:calendar-query xmlns:D='DAV:' xmlns:C='urn:ietf:params:xml:ns:caldav'>"
      << "  <D:prop>"
      << "    <C:calendar-data>"
      << "      <C:expand start='" << utc_xml_formatted(start) << "' end='" << utc_xml_formatted(end) << "'/>"
      << "    </C:calendar-data>"
      << "  </D:prop>"
      << "  <C:filter>"
      << "    <C:comp-filter name='VCALENDAR'>"
      << "      <C:comp-filter name='VEVENT'>"
      << "        <C:time-range start='" << utc_xml_formatted(start) << "' end='" << utc_xml_formatted(end) << "'/>"
      << "      </C:comp-filter>"
      << "    </C:comp-filter>"
      << "  </C:filter>"
      << "</C:calendar-query>"
      << std::endl;
    auto query = query_stream.str();
    HTTPClient http;
    http.begin(_calendar_url.c_str());
    http.setAuthorization(_user.c_str(), _password.c_str());
    http.addHeader("Content-Type", "application/xml");
    int response_code = http.sendRequest("REPORT", query.c_str());
    std::vector<Event> events;
    if (response_code == HTTP_CODE_MULTI_STATUS) {
      std::string payload = http.getString().c_str();
      events = response_payload_to_events(payload);
    }
    http.end();
    if (events.empty()) { last_error_message = "Calendar found no events"; }
    return events;
  }
};
