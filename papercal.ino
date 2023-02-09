#include "config.hpp"
#include "paper/paper_datetime.hpp"
#include "paper/paper_device.hpp"
#include "paper/paper_display.hpp"
#include "paper/paper_wifi.hpp"
#include "calendar.hpp"
#include "event.hpp"

#include <GxEPD2_BW.h>
#include <time.h>
#include <vector>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

const int padding = 5;
PaperDevice device;
PaperDisplay device_display;
PaperWifi device_wifi;
PaperDatetime device_datetime;
tm _today = { 0 };
tm _tomorrow = { 0 };

void setup() {
  Serial.begin(9600);
  enable_display();
  connect_wifi();
  set_datetimes();
  events_to_display(_today);
  events_to_display(_tomorrow);
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

void set_datetimes() {
  device_datetime.fetch(TIME_ZONE, NTP_POOL);
  _today = device_datetime.time_info;
  time_t next_day_timestamp = mktime(&_today) + (24 * 3600);  // add 1 day in seconds
  _tomorrow = *localtime(&next_day_timestamp);
  _tomorrow.tm_hour = 0;  // beginning of day
  _tomorrow.tm_min = 0;
  _tomorrow.tm_sec = 1;
}

void enable_display() {
  device_display.enable_display();
  device_display.panel.fillScreen(GxEPD_WHITE);
  device_display.panel.setTextColor(GxEPD_BLACK);
  device_display.panel.setCursor(0, 15);  // top margin
}

void refresh_display() {
  device_display.panel.display();
}

void events_to_display(tm start) {
  Calendar cal(CALENDAR_USER, CALENDAR_PASSWORD, CALENDAR_URL);
  if (!cal.is_valid()) {
    error_to_display(cal.last_error_message.c_str());
    return;
  }
  std::vector<Event> events = cal.events(start);
  if (cal.error()) {
    error_to_display(cal.last_error_message.c_str());
    return;
  }
  if (cal.error()) {
    error_to_display(cal.last_error_message.c_str());
  } else {
    day_to_display(start);
    for (Event event : events) {
      event_to_display(event);
    }
  }
  device_display.panel.println("");
}

void event_to_display(Event event) {
  device_display.panel.setFont(&FreeSans12pt7b);
  char formatted_start[8];
  if (!event.is_all_day()) {
    device_display.panel.print(event.formatted_start("%H:%M").c_str());
    device_display.panel.print(" ");
  }
  std::string summary = event.summary();
  if (summary.length() > 24) { summary = summary.substr(0, 26) + "-"; }
  device_display.panel.println(summary.c_str());
}

void day_to_display(tm time_info) {
  char formatted_date[11];
  strftime(formatted_date, sizeof(formatted_date), "%a %e %b", &time_info);
  device_display.panel.setFont(&FreeSansBold12pt7b);
  device_display.panel.println(formatted_date);
}

void error_to_display(const char* error_message) {
  device_display.panel.setFont();
  device_display.panel.setTextSize(1);
  device_display.panel.setCursor(padding, device_display.panel.height() - 2 * padding);
  device_display.panel.print(error_message);
}

void refresh_datetime_to_display() {
  char formatted_date[12];
  strftime(formatted_date, sizeof(formatted_date), "%d/%m %H:%M", &_today);
  device_display.panel.setFont();
  device_display.panel.setTextSize(1);
  device_display.panel.setCursor(device_display.panel.width() - 65 - padding, device_display.panel.height() - 2 * padding);
  device_display.panel.print(formatted_date);
}

void deep_sleep() {
  device.sleep_timer_and_button_wakeup(3600 * 3);
}
