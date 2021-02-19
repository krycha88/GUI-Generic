/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include "direct_links.h"

#include <Arduino.h>

namespace Supla {
namespace Control {

DirectLinks::DirectLinks(const char *host, bool isSecured)
    : _lastStateON(false), _lastStateOFF(false) {
  setHost(host);
  enableSSL(isSecured);
}

DirectLinks::~DirectLinks() {
  delete[] client;
}

void DirectLinks::setHost(const char *host) {
  if (host) {
    strncpy(_host, host, MAX_HOST_SIZE);
  }
}

void DirectLinks::setUrlON(const char *url) {
  if (url) {
    strncpy(_urlON, url, MAX_DIRECT_LINKS_SIZE);
  }
}

void DirectLinks::setUrlOFF(const char *url) {
  if (url) {
    strncpy(_urlOFF, url, MAX_DIRECT_LINKS_SIZE);
  }
}

void DirectLinks::enableSSL(bool isSecured) {
  _isSecured = isSecured;
}

bool DirectLinks::openConnection() {
  if (!client->connect(_host, _isSecured ? 443 : 80)) {
    return false;
  }
  return true;
}

bool DirectLinks::closeConnection() {
  client->stop();
  return checkConnection();
}

bool DirectLinks::checkConnection() {
  if (client->connected() == 1) {
    return true;
  } else {
    return false;
  }
}

void DirectLinks::toggleConnection() {
  if (client == NULL) {
    if (_isSecured) {
      client = new WiFiClientSecure();
      ((WiFiClientSecure *)client)->setInsecure();
      ((WiFiClientSecure *)client)->setBufferSizes(256, 256);
      ((WiFiClientSecure *)client)->setTimeout(200);
    } else {
      client = new WiFiClient();
    }
  }

  if (checkConnection()) {
    closeConnection();
  } else {
    openConnection();
  }
}

void DirectLinks::sendRequest(const char *url) {
  if (client) {
    (WiFiClientSecure *)client->print(
        String("GET /direct/") + url + " HTTP/1.1\r\n" + "Host: " + _host +
        "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" +
        "Connection: close\r\n\r\n");

    while (client->connected() || client->available()) {
      String line = client->readStringUntil('\n');
      if (line == "\r") {
        Serial.println(F("Direct links - Headers received"));
        break;
      }
    }

    String line = client->readString();
    if (line.indexOf("true") != -1) {
      Serial.println(F("Alert sent successfully!"));
    } else {
      Serial.print(F("Alert failure"));
      Serial.println(line);
    }
  }
}

void DirectLinks::send(const char *url) {
  toggleConnection();
  sendRequest(url);
  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
}

void DirectLinks::iterateAlways() {
  if (_lastStateON) {
    _lastStateON = false;
    send(_urlON);
  }
  if (_lastStateOFF) {
    _lastStateOFF = false;
    send(_urlOFF);
  }
}

void DirectLinks::handleAction(int event, int action) {
  (void)(event);

  switch (action) {
    case SEND_DIRECT_LINKS_ON:
      _lastStateON = true;
      break;
    case SEND_DIRECT_LINKS_OFF:
      _lastStateOFF = true;
      break;
  }
}

};  // namespace Control
};  // namespace Supla
