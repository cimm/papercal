#include "config.hpp"
#include "paper_datetime.hpp"
#include "paper_wifi.hpp"
#include "calendar.hpp"
#include "Paperdink.h"

#include <time.h>
#include <ArduinoJson.h>

PAPERDINK_DEVICE Paperdink;
const int padding = 5;
PaperWifi device_wifi;
PaperDatetime device_datetime;

void setup() {
  Serial.begin(115200);
  init_display();
  connect_wifi();
  fetch_datetime();
  current_date_to_display();
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
  device_datetime.fetch();
}

void init_display() {
  Paperdink.begin();
  Paperdink.enable_display();
  Paperdink.epd.fillScreen(GxEPD_WHITE);
  Paperdink.epd.setTextColor(GxEPD_BLACK);
}

void refresh_display() {
  Paperdink.epd.display();
}

void current_date_to_display() {
  char formatted_current_date[30];
  device_datetime.format("%a %d %b", formatted_current_date, sizeof(formatted_current_date));
  Paperdink.epd.fillRect(0, 0, Paperdink.epd.width(), 8 * padding, GxEPD_BLACK);
  Paperdink.epd.setTextColor(GxEPD_WHITE);
  Paperdink.epd.setTextSize(4);
  Paperdink.epd.setCursor(padding, padding);
  Paperdink.epd.println(formatted_current_date);
}

void events_to_display() {
  Calendar cal(SYNOLOGY_WEBAPI);
  if (cal.error()) {
    Serial.println(cal.last_error_message);
    return;
  }

  cal.authenticate(SYNOLOGY_ACCOUNT, SYNOLOGY_PASSWORD);
  if (cal.error()) {
    Serial.println(cal.last_error_message);
  }

  struct tm start = device_datetime.time_info;  // now
  struct tm end = { 0 };                        // end of today
  end.tm_year = start.tm_year;
  end.tm_mon = start.tm_mon;
  end.tm_mday = start.tm_mday;
  end.tm_hour = 23;
  end.tm_min = 59;

  JsonArrayConst events = cal.events(SYNOLOGY_CALENDAR_ID, start, end, 1024 * 30, false);  // TODO How do we know how much memory is needed?
  if (cal.error()) {
    Serial.println(cal.last_error_message);
  } else {
    Paperdink.epd.setTextColor(GxEPD_BLACK);
    Paperdink.epd.setTextSize(2);
    Paperdink.epd.setCursor(0, 10 * padding);
    for (JsonVariantConst event : events) {
      event_to_display(event);
    }
  }

  cal.logout();
  if (cal.error()) {
    Serial.println(cal.last_error_message);
  }
}

void event_to_display(Event event) {
  Paperdink.epd.print(" ");
  char formatted_start[8];
  event.formatted_start("%H:%M", formatted_start, sizeof(formatted_start));
  Paperdink.epd.print(formatted_start);
  Paperdink.epd.print(" ");
  String summary = event.summary();
  if (summary.length() > 24) { summary = summary.substring(0, 24) + "-"; }
  Paperdink.epd.println(summary);
}

void refresh_datetime_to_display() {
  char formatted_refresh_date[30];
  device_datetime.format("Refresh: %d/%m %H:%M", formatted_refresh_date, sizeof(formatted_refresh_date));
  Paperdink.epd.setTextSize(1);
  Paperdink.epd.setCursor(Paperdink.epd.width() - 120 - padding, Paperdink.epd.height() - 2 * padding);
  Paperdink.epd.print(formatted_refresh_date);
}

void deep_sleep() {
  uint64_t sleep_in_microseconds = 3600000000 * 6;  // = 6 hours
  Paperdink.deep_sleep_timer_button_wakeup(sleep_in_microseconds, BUTTON_1_PIN);
}