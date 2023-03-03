#include "config.hpp"
#include "paper/paper_datetime.hpp"
#include "paper/paper_device.hpp"
#include "paper/paper_display.hpp"
#include "paper/paper_sound.hpp"
#include "paper/paper_wifi.hpp"
#include "calendar.hpp"
#include "event.hpp"
#include "ui.hpp"

#include <time.h>
#include <vector>

PaperDevice device;
PaperDatetime device_datetime;
PaperDisplay device_display;
PaperSound device_sound;
PaperWifi device_wifi;
UI ui(&device_display);
bool error = false;
tm today = { 0 };
tm tomorrow = { 0 };

void setup() {
  Serial.begin(9600);
  enable_display();
  connect_wifi();
  get_datetimes();
  events_to_display(today);
  events_to_display(tomorrow);
  refresh_datetime_to_display();
  disconnect_wifi();
  refresh_display();
  deep_sleep();
}

void loop() {}

void connect_wifi() {
  device_wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

void disconnect_wifi() {
  device_wifi.disconnect();
}

void get_datetimes() {
  device_datetime.fetch(TIME_ZONE, NTP_POOL);
  today = device_datetime.time_info;
  time_t next_day_timestamp = mktime(&today) + (24 * 3600);  // add 1 day in seconds
  tomorrow = *localtime(&next_day_timestamp);
  tomorrow.tm_hour = 0;  // beginning of day
  tomorrow.tm_min = 0;
  tomorrow.tm_sec = 1;
}

void enable_display() {
  device_display.enable_display();
  ui.reset();
}

void refresh_display() {
  if (error && !REFRESH_ON_ERROR) {
    Serial.println("Error and REFRESH_ON_ERROR is false: will not refresh display!");
  } else {
    device_display.panel.display();
  }
}

void events_to_display(tm start) {
  Calendar cal(CALENDAR_USER, CALENDAR_PASSWORD, CALENDAR_URL);
  if (!cal.is_valid()) {
    set_error(cal.last_error_message.c_str());
    return;
  }
  std::vector<Event> events = cal.events(start);
  if (cal.error()) {
    set_error(cal.last_error_message.c_str());
    return;
  }
  if (events.empty()) { return; }
  day_to_display(start);
  for (Event event : events) {
    event_to_display(event);
  }
}

void event_to_display(Event event) {
  std::string start = "";
  if (!event.is_all_day()) {
    char formatted_start[8];
    start = event.formatted_start("%H:%M");
  }
  std::string summary = event.summary();
  if (summary.length() > 24) { summary = summary.substr(0, 26) + "-"; }
  ui.row(start, summary);
}

void day_to_display(tm time_info) {
  char formatted_date[11];
  strftime(formatted_date, sizeof(formatted_date), "%a %e %b", &time_info);
  ui.row("", formatted_date, true);
}

void set_error(const char* error_message) {
  error = true;
  Serial.println(error_message);
  device_sound.clicks(3);
  ui.footer_left(error_message);
}

void refresh_datetime_to_display() {
  char formatted_date[12];
  strftime(formatted_date, sizeof(formatted_date), "%d/%m %H:%M", &today);
  ui.footer_right(formatted_date);
}

void deep_sleep() {
  device.sleep_timer_and_button_wakeup(3600 * 3);
}
