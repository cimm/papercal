/*
  Gets events from a Synology Calendar via the Synology API.

  - https://global.download.synology.com/download/Document/Software/DeveloperGuide/Package/Calendar/2.4/enu/Synology_Calendar_API_Guide_enu.pdf
  - https://github.com/ProtoThis/python-synology/blob/master/src/synology_dsm/const.py
*/

#include <HTTPClient.h>
#include <ArduinoJson.h>

class SynoCalendar {
  String _webapi_url;
  String _sid;
  String _synotoken;
  int _auth_max_api_version;
  int _event_max_api_version;

  time_t utc_timestamp(tm time_info) {  // tm is not time zone aware
    char* tz = getenv("TZ");
    char* prev_time_zone = new char[strlen(tz) + 1];
    strcpy(prev_time_zone, tz);
    setenv("TZ", "UTC0", 1);
    time_t utc_timestamp = mktime(&time_info);
    setenv("TZ", prev_time_zone, 1);
    return utc_timestamp;
  }

public:
  String last_error_message = "";

  SynoCalendar(const char* webapi_url) {
    _webapi_url = webapi_url;
    fetch_api_versions();
  }

  bool error() {
    return last_error_message != "";
  }

  void authenticate(const char* account, const char* passwd) {
    last_error_message = "";
    String url_buf = "{api}/auth.cgi?api=SYNO.API.Auth&method=login&version={ver}&account={acc}&passwd={pwd}&format=sid&enable_syno_token=yes";
    url_buf.replace("{api}", _webapi_url);
    url_buf.replace("{ver}", String(_auth_max_api_version));
    url_buf.replace("{acc}", account);
    url_buf.replace("{pwd}", passwd);

    char* url = new char[url_buf.length() + 1];
    strcpy(url, url_buf.c_str());
    DynamicJsonDocument auth_doc = fetch_json(url);

    bool success = auth_doc["success"].as<bool>();
    if (!success) {
      last_error_message = "Synology API could not authenticate";
      return;
    }

    _sid = auth_doc["data"]["sid"].as<const char*>();
    _synotoken = auth_doc["data"]["synotoken"].as<const char*>();
  }

  JsonArrayConst events(char* cal_id, tm start, tm end, size_t capacity, bool repeat = true) {  // capacity is the JSON doc memory to reserve
    last_error_message = "";
    String url_buf = "{api}/entry.cgi?version={ver}&cal_id_list=['{cal_id}']&start={start}&end={end}&list_repeat={repeat}&api=SYNO.Cal.Event&method=list&_sid={sid}";  // start & and are unix timestamps in UTC
    url_buf.replace("{api}", _webapi_url);
    url_buf.replace("{ver}", String(_event_max_api_version));
    url_buf.replace("{cal_id}", cal_id);
    time_t start_utc_timestamp = utc_timestamp(start);
    url_buf.replace("{start}", String(start_utc_timestamp));
    time_t end_utc_timestamp = utc_timestamp(end);
    url_buf.replace("{end}", String(end_utc_timestamp));
    if (repeat) {
      url_buf.replace("{repeat}", "true");  // FIXME Returns weird results that fall outside of the date range
    } else {
      url_buf.replace("{repeat}", "false");
    }
    url_buf.replace("{sid}", _sid);
    char* url = new char[url_buf.length() + 1];
    strcpy(url, url_buf.c_str());
    DynamicJsonDocument events_doc = fetch_json(url, capacity);

    JsonArrayConst events = events_doc["data"][cal_id];
    if (events.size() == 0) {
      last_error_message = "Synology API found no events";
    }
    return events;
  }

  void logout() {
    last_error_message = "";
    String url_buf = "{api}/auth.cgi?api=SYNO.API.Auth&version={ver}&method=logout";
    url_buf.replace("{api}", _webapi_url);
    url_buf.replace("{ver}", String(_auth_max_api_version));
    char* url = new char[url_buf.length() + 1];
    strcpy(url, url_buf.c_str());
    DynamicJsonDocument logout_doc = fetch_json(url);

    bool success = logout_doc["success"].as<bool>();
    if (!success) {
      last_error_message = "Synology API could not logout";
    }
  }

private:
  void fetch_api_versions() {
    last_error_message = "";
    String url_buf = "{api}/query.cgi?api=SYNO.API.Info&version=1&method=query&query=SYNO.API.Auth,SYNO.Cal.Event";
    url_buf.replace("{api}", _webapi_url);
    char* url = new char[url_buf.length() + 1];
    strcpy(url, url_buf.c_str());
    DynamicJsonDocument version_doc = fetch_json(url);

    _auth_max_api_version = version_doc["data"]["SYNO.API.Auth"]["maxVersion"].as<int>();
    _event_max_api_version = 3;  // Synology API returns 4 but that does not seem to work, API bug?
    if (_auth_max_api_version == 0 || _event_max_api_version == 0) {
      last_error_message = "Synology API not found";
    }
  }

  DynamicJsonDocument fetch_json(char* url, size_t capacity = 512) {
    last_error_message = "";
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.begin(client, url);
    if (_synotoken != "") { http.addHeader("X-SYNO-TOKEN", _synotoken); }  // authenticated
    DynamicJsonDocument doc(capacity);                                     // TODO ArduinoJSON can filter fields to limit memory, usefull for the events JSON
    int response_code = http.GET();
    if (response_code == HTTP_CODE_OK) {
      DeserializationError json_error = deserializeJson(doc, http.getStream());
      if (json_error) {
        last_error_message = "Deserialization error: ";
        last_error_message += json_error.c_str();
      }
    } else {
      last_error_message = "HTTP error";
    }
    http.end();
    return doc;
  }
};
