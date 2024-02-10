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

#ifndef _direct_links_connect_h
#define _direct_links_connect_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <supla/element.h>
#include <supla/network/network.h>

#define MAX_DIRECT_LINKS_SIZE 32
#define MAX_HOST_SIZE         32

namespace Supla {
namespace Sensor {

class DirectLinksConnect : public Element {
 public:
  DirectLinksConnect(const char *url, const char *host, bool isSecured = true);
  ~DirectLinksConnect();

  void setHost(const char *host);
  void setPort(uint16_t port);
  void setUrl(const char *url);
  void enableSSL(bool isSecured);

  bool checkConnection();
  void toggleConnection();
  bool openConnection();
  bool closeConnection();
  void send();
  const char *getRequest();
  virtual void sendRequest();
  void iterateAlways();
  void onInitNetworkConnected();

 protected:
  WiFiClient *client;
  unsigned long lastReadTime;

  bool _isSecured;
  char _url[MAX_DIRECT_LINKS_SIZE];
  char _host[MAX_HOST_SIZE];
  uint16_t _port;

  int8_t retryCount = 0;
  bool initNetworkConnected = false;
};

};  // namespace Sensor
};  // namespace Supla

#endif
