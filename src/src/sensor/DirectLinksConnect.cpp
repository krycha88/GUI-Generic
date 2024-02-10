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

#include <Arduino.h>
#include "DirectLinksConnect.h"
#include "../../GUIGenericCommon.h"

namespace Supla {
namespace Sensor {

DirectLinksConnect::DirectLinksConnect(const char *url, const char *host, bool isSecured) : client(nullptr), lastReadTime(0) {
  this->setUrl(url);

  String server(host);
  auto npos = server.indexOf(":");

  String suplaServer = server.substring(0, npos);
  setHost(suplaServer.c_str());

  if (npos > 0) {
    String portServer = server.substring(npos + 1);
    setPort(portServer.toInt());
  }
  else {
    setPort(isSecured ? 443 : 80);
  }

  enableSSL(isSecured);
}

DirectLinksConnect::~DirectLinksConnect() {
  delete[] client;
}

void DirectLinksConnect::setHost(const char *host) {
  if (host) {
    strncpy(_host, host, MAX_HOST_SIZE);
  }
}

void DirectLinksConnect::setPort(uint16_t port) {
  _port = port;
}

void DirectLinksConnect::setUrl(const char *url) {
  if (url) {
    strncpy(_url, url, MAX_DIRECT_LINKS_SIZE);
  }
}

void DirectLinksConnect::enableSSL(bool isSecured) {
  _isSecured = isSecured;
}

bool DirectLinksConnect::openConnection() {
  return client->connect(_host, _port) ? true : false;
}

bool DirectLinksConnect::closeConnection() {
  client->stop();
  return checkConnection();
}

bool DirectLinksConnect::checkConnection() {
  return client->connected() == 1 ? true : false;
}

void DirectLinksConnect::toggleConnection() {
  if (client == NULL) {
    if (_isSecured) {
      client = new WiFiClientSecure();
      ((WiFiClientSecure *)client)->setInsecure();
#ifdef ARDUINO_ARCH_ESP8266
      ((WiFiClientSecure *)client)->setBufferSizes(1024, 512);
#endif
      ((WiFiClientSecure *)client)->setTimeout(10);
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

  if (static_cast<unsigned int>(snprintf(request, sizeof(request),
                                         "GET /direct/%s HTTP/1.1\r\n"
                                         "Host: %s\r\n"
                                         "User-Agent: BuildFailureDetectorESP8266\r\n"
                                         "Connection: close\r\n\r\n",
                                         _url, _host)) >= sizeof(request)) {
    Serial.println(F("Error: URL or host too long"));
    return nullptr;
  }

  if (!client || !client->connected()) {
    Serial.println(F("Error: Client not connected"));
    return nullptr;
  }

  client->print(request);

  while (client->connected() || client->available()) {
    if (client->readStringUntil('\n') == "\r") {
      Serial.println(F("Direct links - Headers received"));
      break;
    }
    delay(0);
  }

  size_t i = 0;

  while (client->connected() || client->available()) {
    char c = client->read();
    if (i < sizeof(result) - 1) {  // Avoid buffer overflow
      result[i++] = c;
      // Check for the end of response
      if (c == '}' && client->peek() == -1) {
        break;
      }
    }
    else {
      break;  // Stop reading to prevent buffer overflow
    }
    delay(0);
  }

  result[i] = '\0';  // Null-terminate the result string
  Serial.println(result);

  return result;
}

void DirectLinksConnect::sendRequest() {
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
