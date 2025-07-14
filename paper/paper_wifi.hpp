#pragma once

#include <WiFi.h>

class PaperWifi {
  const uint16_t PAUZE = 500;  // miliseconds

public:
  void connect(char* ssid, char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(PAUZE);
    }
    Serial.printf("WiFi connected to %s as %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  }

  void disconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi disconnected");
  }
};
