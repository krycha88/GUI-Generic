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
#pragma once

#ifdef SUPLA_DIRECT_LINKS_MULTI_SENSOR

#include <Arduino.h>
#include <supla/element.h>

#define DIRECT_LINKS_RESULT_SIZE  512
#define DIRECT_LINKS_REQUEST_SIZE 192
#define DIRECT_LINKS_LINE_SIZE    96

#define MAX_HOST_SIZE         64
#define MAX_DIRECT_LINKS_SIZE 128

#ifdef ARDUINO_ARCH_ESP32
#include <WiFiClientSecure.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#endif

namespace Supla {
namespace Sensor {

class DirectLinksConnect : public Element {
 public:
  DirectLinksConnect(const char *url, const char *host, bool isSecured);

  ~DirectLinksConnect();

  void iterateAlways();
  void send();
  const char *getRequest();
  
 private:
  void setHost(const char *host);
  void setPort(uint16_t port);
  void setUrl(const char *url);

  bool openConnection();
  bool closeConnection();
  bool checkConnection();
  void toggleConnection();

  void sendRequest();

  void onInitNetworkConnected();

 protected:
#ifdef ARDUINO_ARCH_ESP32
  WiFiClientSecure client;
#else
  WiFiClientSecureBearSSL client;
#endif

  int8_t retryCount = 0;

 private:
  char result[DIRECT_LINKS_RESULT_SIZE];
  char request[DIRECT_LINKS_REQUEST_SIZE];
  char line[DIRECT_LINKS_LINE_SIZE];

  char _host[MAX_HOST_SIZE];
  char _url[MAX_DIRECT_LINKS_SIZE];

  uint16_t _port = 80;
  bool _isSecured = false;

  bool initNetworkConnected = false;
  unsigned long lastReadTime = 0;
};

};  // namespace Sensor
};  // namespace Supla

#endif