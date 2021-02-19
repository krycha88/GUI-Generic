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

#include "pushover.h"

#include <Arduino.h>

namespace Supla {
namespace Control {

Pushover::Pushover(const char *token, const char *user, bool isSecured)
    : lastStateSend(false) {
  setToken(token);
  setUser(user);
  _isSecured = isSecured;
}

Pushover::~Pushover() {
  delete[] client;
}

void Pushover::setToken(const char *token) {
  if (token) {
    strncpy(_token, token, MAX_TOKEN_SIZE);
  }
}

void Pushover::setUser(const char *user) {
  if (user) {
    strncpy(_user, user, MAX_TOKEN_SIZE);
  }
}

void Pushover::setTitle(const char *title) {
  if (title) {
    strncpy(_title, title, MAX_TOKEN_SIZE);
  }
}

void Pushover::setMessage(const char *message) {
  if (message) {
    strncpy(_message, message, MAX_TOKEN_SIZE);
  }
}

bool Pushover::openConnection() {
  if (!client->connect(host, _isSecured ? 443 : 80)) {
    return false;
  }
  return true;
}

bool Pushover::closeConnection() {
  client->stop();
  return checkConnection();
}

bool Pushover::checkConnection() {
  if (client->connected() == 1) {
    return true;
  } else {
    return false;
  }
}

void Pushover::toggleConnection() {
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

void Pushover::sendRequest() {
  if (client) {
    String post = String("token=") + _token + "&user=" + _user +
                  "&title=" + _title + "&message=" + _message;

    /* String("token=") + _token + "&user=" + _user + "&title=" + _title +
     "&message=" + _message + "&device=" + _device + "&url=" + _url +
     "&url_title=" + _url_title + "&priority=" + _priority +
     "&retry=" + _retry + "&expire=" + _expire + "&sound=" + _sound;*/

    (WiFiClientSecure *)client->print(
        String("POST ") + path + " HTTP/1.1\r\n" + "host: " + host + "\r\n" +
        "Content-length: " + String(post.length(), DEC) +
        "\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n" +
        "Connection: close\r\n\r\n" + post);

    while (client->connected()) {
      String line = client->readStringUntil('\n');
      if (line == "\r") {
        Serial.println(F("Headers received"));
        break;
      }
    }

    /* String line = client->readString();
     if (line.indexOf("\"status\":1") != -1 || line.indexOf("200 OK") != -1) {
       Serial.println(F("Alert sent successfully!"));
     } else {
       Serial.print(F("Alert failure"));
       Serial.println(line);
     }*/
  }
}

void Pushover::send() {
  toggleConnection();
  sendRequest();
  toggleConnection();

  if (client) {
    delete client;
    client = nullptr;
  }
}

void Pushover::iterateAlways() {
  if (lastStateSend) {
    lastStateSend = false;
    send();
  }
}

void Pushover::handleAction(int event, int action) {
  (void)(event);
  if (action == SEND_NOTIF_1) {
    lastStateSend = true;
  }
}

};  // namespace Control
};  // namespace Supla
