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

const int padding = 5;
PaperDevice device;
PaperDisplay device_display;
PaperWifi device_wifi;
PaperDatetime device_datetime;

void setup() {
  Serial.begin(9600);
  enable_display();
  connect_wifi();
  fetch_datetime();

  events_to_display();
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

void fetch_datetime() {
  device_datetime.fetch(TIME_ZONE, NTP_POOL);
}

void enable_display() {
  device_display.enable_display();
  device_display.panel.fillScreen(GxEPD_WHITE);
  device_display.panel.setTextColor(GxEPD_BLACK);
}

void refresh_display() {
  device_display.panel.display();
}

void events_to_display() {
  tm now = device_datetime.time_info;
  tm start = now;
  if (now.tm_hour >= SHOW_NEXT_DAY_FROM_HOUR) {
    time_t next_day_timestamp = mktime(&now) + (24 * 60 * 60);  // add 1 day in seconds
    start = *localtime(&next_day_timestamp);
    start.tm_hour = 0;  // beginning of day
    start.tm_min = 0;
    start.tm_sec = 1;
  }
  tm end = { 0 };
  end.tm_year = start.tm_year;
  end.tm_mon = start.tm_mon;
  end.tm_mday = start.tm_mday;
  end.tm_hour = 23;
  end.tm_min = 59;
  end.tm_sec = 59;

  title_to_display(start);

  Calendar cal(CALENDAR_USER, CALENDAR_PASSWORD, CALENDAR_URL);
  std::vector<Event> events = cal.events(start, end);
  if (cal.error()) {
    error_to_display(cal.last_error_message.c_str());
    return;
  }

  device_display.panel.setTextColor(GxEPD_BLACK);
  device_display.panel.setTextSize(2);
  device_display.panel.setCursor(0, 10 * padding);
  if (cal.error()) {
    error_to_display(cal.last_error_message.c_str());
  } else {
    for (Event event : events) {
      event_to_display(event);
    }
  }
}

void event_to_display(Event event) {
  char formatted_start[8];
  if (event.is_all_day()) {
    device_display.panel.print("     ");
  } else {
    device_display.panel.print(event.formatted_start("%H:%M").c_str());
  }
  device_display.panel.print(" ");
  std::string summary = event.summary();
  if (summary.length() > 24) { summary = summary.substr(0, 25) + "-"; }
  device_display.panel.println(summary.c_str());
}

void title_to_display(tm time_info) {
  char formatted_date[11];
  strftime(formatted_date, 11, "%a %e %b", &time_info);
  device_display.panel.fillRect(0, 0, device_display.panel.width(), 8 * padding, GxEPD_BLACK);
  device_display.panel.setTextColor(GxEPD_WHITE);
  device_display.panel.setTextSize(4);
  device_display.panel.setCursor(padding, padding);
  device_display.panel.println(formatted_date);
}

void error_to_display(const char* error_message) {
  device_display.panel.setTextSize(1);
  device_display.panel.setCursor(padding, device_display.panel.height() - 2 * padding);
  device_display.panel.print(error_message);
}

void refresh_datetime_to_display() {
  char formatted_refresh_date[30];
  device_datetime.format("%d/%m %H:%M", formatted_refresh_date, sizeof(formatted_refresh_date));
  device_display.panel.setTextSize(1);
  device_display.panel.setCursor(device_display.panel.width() - 65 - padding, device_display.panel.height() - 2 * padding);
  device_display.panel.print(formatted_refresh_date);
}

void deep_sleep() {
  device.sleep_timer_and_button_wakeup(3600 * 3);
}
