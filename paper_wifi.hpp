/* 
  Connects or disconnects a WiFi network.
*/

#include <WiFi.h>

class PaperWifi {
public:
  void connect(char* ssid, char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  void disconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
};