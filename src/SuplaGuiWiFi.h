#ifndef SuplaGuiWiFi_h
#define SuplaGuiWiFi_h

#include <supla/network/esp_wifi.h>

#define MAX_HOSTNAME 32

namespace Supla {
class GUIESPWifi : public Supla::ESPWifi {
 public:
  GUIESPWifi(const char *wifiSsid = nullptr, const char *wifiPassword = nullptr) : ESPWifi(wifiSsid, wifiPassword) {
  }

  int connect(const char *server, int port = -1) {
    String message;
    if (client == NULL) {
      if (isSecured) {
        message = "Secured connection";
        client = new WiFiClientSecure();
        if (fingerprint.length() > 0) {
          message += " with certificate matching";
#ifdef ARDUINO_ARCH_ESP8266
          ((WiFiClientSecure *)client)->setFingerprint(fingerprint.c_str());
#else
          message += " - NOT SUPPORTED ON ESP32 implmentation";
#endif
        }
        else {
          message += " without certificate matching";
#ifdef ARDUINO_ARCH_ESP8266
          ((WiFiClientSecure *)client)->setInsecure();
#else
          message += " - NOT SUPPORTED ON ESP32 implmentation";
#endif
        }
      }
      else {
        message = "unsecured connection";
        client = new WiFiClient();
      }

      if (isBuffer) {
        ((WiFiClientSecure *)client)->setBufferSizes(256, 256);  // EXPERIMENTAL
      }
    }

    int connectionPort = (isSecured ? 2016 : 2015);
    if (port != -1) {
      connectionPort = port;
    }

    supla_log(LOG_DEBUG, "Establishing %s with: %s (port: %d)", message.c_str(), server, connectionPort);

    bool result = client->connect(server, connectionPort);

    if (result && isSecured) {
      if (!((WiFiClientSecure *)client)->verify(fingerprint.c_str(), server)) {
        supla_log(LOG_DEBUG, "Provided certificates doesn't match!");
        client->stop();
        return false;
      }
    };

    return result;
  }

  void setup() {
    if (!wifiConfigured) {
      wifiConfigured = true;
#ifdef ARDUINO_ARCH_ESP8266
      gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
        (void)(event);
        Serial.print(F("local IP: "));
        Serial.println(WiFi.localIP());
        Serial.print(F("subnetMask: "));
        Serial.println(WiFi.subnetMask());
        Serial.print(F("gatewayIP: "));
        Serial.println(WiFi.gatewayIP());
        long rssi = WiFi.RSSI();
        Serial.print(F("Signal strength (RSSI): "));
        Serial.print(rssi);
        Serial.println(F(" dBm"));
      });
      disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
        (void)(event);
        Serial.println(F("WiFi station disconnected"));
      });
#else
      WiFiEventId_t event_gotIP = WiFi.onEvent(
          [](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.print(F("local IP: "));
            Serial.println(WiFi.localIP());
            Serial.print(F("subnetMask: "));
            Serial.println(WiFi.subnetMask());
            Serial.print(F("gatewayIP: "));
            Serial.println(WiFi.gatewayIP());
            long rssi = WiFi.RSSI();
            Serial.print(F("Signal Strength (RSSI): "));
            Serial.print(rssi);
            Serial.println(F(" dBm"));
          },
          WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

      WiFiEventId_t event_disconnected = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) { Serial.println(F("wifi Station disconnected")); },
                                                      WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
#endif
      Serial.print(F("WiFi: establishing connection with SSID: \""));
      Serial.print(ssid);
      Serial.println(F("\""));
      if (hostname) {
        WiFi.hostname(hostname);
      }
      WiFi.softAPdisconnect(true);
      WiFi.setAutoConnect(false);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
    }
    else if (ConfigESP->configModeESP == NORMAL_MODE) {
      Serial.println(F("WiFi: resetting WiFi connection"));
      if (client) {
        delete client;
        client = nullptr;
      }
      WiFi.reconnect();
    }

    yield();
  }

  void enableBuffer(bool value) {
    isBuffer = value;
  }

  void setHostName(const char *wifiHostname) {
    if (wifiHostname) {
      strncpy(hostname, wifiHostname, MAX_HOSTNAME);
    }
  }

 protected:
  bool isBuffer;
  char hostname[MAX_HOSTNAME];
};
};      // namespace Supla
#endif  // SuplaGuiWiFi_h
