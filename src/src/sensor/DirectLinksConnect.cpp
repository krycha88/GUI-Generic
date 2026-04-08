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
#include <supla/network/network.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFiClientSecure.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#endif

namespace Supla {
namespace Sensor {

DirectLinksConnect::DirectLinksConnect(
    const char *url,
    const char *host,
    bool isSecured)
    : _isSecured(isSecured),
      lastReadTime(0) {

  setUrl(url);

  char hostCopy[MAX_HOST_SIZE + 8];

  strncpy(hostCopy, host, sizeof(hostCopy));
  hostCopy[sizeof(hostCopy) - 1] = '\0';

  char *colon = strchr(hostCopy, ':');

  if (colon) {
    *colon = '\0';
    setHost(hostCopy);
    setPort(atoi(colon + 1));
  } else {
    setHost(hostCopy);
    setPort(isSecured ? 443 : 80);
  }
}

DirectLinksConnect::~DirectLinksConnect() {}

void DirectLinksConnect::setHost(const char *host) {

  if (!host)
    return;

  strncpy(_host, host, MAX_HOST_SIZE);
  _host[MAX_HOST_SIZE - 1] = '\0';
}

void DirectLinksConnect::setPort(uint16_t port) {
  _port = port;
}

void DirectLinksConnect::setUrl(const char *url) {

  if (!url)
    return;

  strncpy(_url, url, MAX_DIRECT_LINKS_SIZE);
  _url[MAX_DIRECT_LINKS_SIZE - 1] = '\0';
}

bool DirectLinksConnect::openConnection() {

#ifdef ARDUINO_ARCH_ESP8266

  client.setInsecure();
  client.setBufferSizes(1024, 512);
  client.setTimeout(10);

#else

  client.setInsecure();
  client.setHandshakeTimeout(20);
  client.setTimeout(20);

#endif

  Serial.printf(
      "Connecting %s:%d heap:%u\n",
      _host,
      _port,
      ESP.getFreeHeap());

  bool ok = client.connect(_host, _port);

  if (!ok)
    Serial.println("Connection failed");

  return ok;
}

bool DirectLinksConnect::closeConnection() {

  client.stop();
  return !client.connected();
}

bool DirectLinksConnect::checkConnection() {

  return client.connected();
}

void DirectLinksConnect::toggleConnection() {

  if (client.connected()) {
    client.stop();
    return;
  }

  openConnection();
}

void DirectLinksConnect::send() {

  toggleConnection();
  sendRequest();
  toggleConnection();

  printFreeMemory("DirectLinks");
}

const char *DirectLinksConnect::getRequest() {

  int n = snprintf(
      request,
      sizeof(request),
      "GET /direct/%s HTTP/1.1\r\n"
      "Host: %s\r\n"
      "Connection: close\r\n\r\n",
      _url,
      _host);

  if (n <= 0 || n >= sizeof(request)) {
    Serial.println("Request too long");
    return nullptr;
  }

  if (!client.connected()) {
    Serial.println("Client not connected");
    return nullptr;
  }

  client.print(request);

  // headers

  while (client.connected()) {

    size_t len =
        client.readBytesUntil(
            '\n',
            line,
            sizeof(line) - 1);

    line[len] = 0;

    if (strcmp(line, "\r") == 0)
      break;

    delay(0);
  }

  // body

  size_t i = 0;

  while (client.connected() || client.available()) {

    while (client.available()) {

      char c = client.read();

      if (i < sizeof(result) - 1)
        result[i++] = c;
    }

    delay(0);
  }

  result[i] = 0;

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

  if (Supla::Network::IsReady() &&
      !initNetworkConnected) {

    send();

    lastReadTime = millis();
    initNetworkConnected = true;
  }
}

};  
};  

#endif