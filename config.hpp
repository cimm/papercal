#pragma once

// WiFi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

// CalDAV Calendar
#define CALENDAR_USER "user"
#define CALENDAR_PASSWORD "password"
#define CALENDAR_URL "https://synology:5001/caldav/user/calid/" // Click chevron next to calendar name in webview → CALDav Account → Thunderbird

// Date & time
#define TIME_ZONE "CET-1CEST,M3.5.0,M10.5.0/3"  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define NTP_POOL "pool.ntp.org"

#define REFRESH_ON_ERROR true  // Keep old events on screen if there is an error? No error messages will be shown on the display.

constexpr bool _strings_equal(const char* a, const char* b) {
  return *a == *b && (*a == '\0' || _strings_equal(a + 1, b + 1));
}

static_assert(!_strings_equal(WIFI_SSID, "ssid"), "WIFI_SSID is still set to the default value!");
static_assert(!_strings_equal(WIFI_PASSWORD, "password"), "WIFI_PASSWORD is still set to the default value!");
static_assert(!_strings_equal(CALENDAR_USER, "user"), "CALENDAR_USER is still set to the default value!");
static_assert(!_strings_equal(CALENDAR_PASSWORD, "password"), "CALENDAR_PASSWORD is still set to the default value!");
