/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifdef SUPLA_DIRECT_LINKS_MULTI_SENSOR

#include <Arduino.h>
#include "DirectLinksConnect.h"
#include "../../GUIGenericCommon.h"

#ifdef ARDUINO_ARCH_ESP32
#include <WiFiClientSecure.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#endif

namespace Supla {
namespace Sensor {

DirectLinksConnect::DirectLinksConnect(const char *url, const char *host, bool isSecured) : client(nullptr), lastReadTime(0) {
  setUrl(url);

  char hostCopy[MAX_HOST_SIZE + 16];
  strncpy(hostCopy, host, sizeof(hostCopy));
  hostCopy[sizeof(hostCopy) - 1] = '\0';

  char *colon = strchr(hostCopy, ':');
  if (colon != nullptr) {
    *colon = '\0';  // oddziel host od portu
    setHost(hostCopy);
    setPort(atoi(colon + 1));
  }
  else {
    setHost(hostCopy);
    setPort(isSecured ? 443 : 80);
  }

  enableSSL(isSecured);
}

DirectLinksConnect::~DirectLinksConnect() {
  if (client) {
    delete client;
    client = nullptr;
  }
}

void DirectLinksConnect::setHost(const char *host) {
  if (host) {
    strncpy(_host, host, MAX_HOST_SIZE);
    _host[MAX_HOST_SIZE - 1] = '\0';
  }
}

void DirectLinksConnect::setPort(uint16_t port) {
  _port = port;
}

void DirectLinksConnect::setUrl(const char *url) {
  if (url) {
    strncpy(_url, url, MAX_DIRECT_LINKS_SIZE);
    _url[MAX_DIRECT_LINKS_SIZE - 1] = '\0';
  }
}

void DirectLinksConnect::enableSSL(bool isSecured) {
  _isSecured = isSecured;
}

bool DirectLinksConnect::openConnection() {
  if (!client)
    return false;
  Serial.printf("Connecting to %s:%d ...\n", _host, _port);
  bool res = client->connect(_host, _port);
  if (!res)
    Serial.println("Connection failed!");
  return res;
}

bool DirectLinksConnect::closeConnection() {
  if (client)
    client->stop();
  return !checkConnection();
}

bool DirectLinksConnect::checkConnection() {
  return client && client->connected();
}

void DirectLinksConnect::toggleConnection() {
  if (!client) {
    if (_isSecured) {
#ifdef ARDUINO_ARCH_ESP32
      WiFiClientSecure *sslClient = new WiFiClientSecure();
      sslClient->setInsecure();
      sslClient->setTimeout(30);
      client = sslClient;
#else
      WiFiClientSecure *sslClient = new WiFiClientSecureBearSSL();
      sslClient->setInsecure();
      sslClient->setBufferSizes(1024, 512);
      sslClient->setTimeout(10);
      client = sslClient;
#endif
    }
    else {
      client = new WiFiClient();
    }
  }

  if (checkConnection()) {
    closeConnection();
  }
  else {
    openConnection();
  }
}

void DirectLinksConnect::send() {
  toggleConnection();
  sendRequest();
  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
  printFreeMemory("DirectLinksConnect");
}

const char *DirectLinksConnect::getRequest() {
  static char result[1088];
  char request[256];

  int n = snprintf(request, sizeof(request),
                   "GET /direct/%s HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "User-Agent: BuildFailureDetectorESP\r\n"
                   "Connection: close\r\n\r\n",
                   _url, _host);
  if (n < 0 || n >= (int)sizeof(request)) {
    Serial.println(F("Error: URL or host too long"));
    return nullptr;
  }

  if (!client || !client->connected()) {
    Serial.println(F("Error: Client not connected"));
    return nullptr;
  }

  client->print(request);

  // Odczyt nagłówków
  char line[128];
  while (client->connected() || client->available()) {
    size_t len = client->readBytesUntil('\n', line, sizeof(line) - 1);
    line[len] = '\0';
    if (strcmp(line, "\r") == 0) {
      Serial.println(F("Direct links - Headers received"));
      break;
    }
    delay(1);
  }

  // Odczyt odpowiedzi
  size_t i = 0;
  while (client->connected() || client->available()) {
    while (client->available()) {
      char c = client->read();
      if (i < sizeof(result) - 1)
        result[i++] = c;
      else
        break;
    }
    delay(1);
  }
  result[i] = '\0';
  Serial.println(result);

  return result;
}

void DirectLinksConnect::sendRequest() {
  getRequest();
}

void DirectLinksConnect::iterateAlways() {
  onInitNetworkConnected();

  if (millis() - lastReadTime > 60000) {
    send();
    lastReadTime = millis();
  }
}

void DirectLinksConnect::onInitNetworkConnected() {
  if (Supla::Network::IsReady() && !initNetworkConnected) {
    send();
    lastReadTime = millis();
    initNetworkConnected = true;
  }
}

};  // namespace Sensor
};  // namespace Supla

#endif