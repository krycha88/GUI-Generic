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

#ifndef _direct_link_sensor_thermometer_h
#define _direct_link_sensor_thermometer_h

#define MAX_DIRECT_LINKS_SIZE 32
#define MAX_HOST_SIZE         32

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>
#include <supla/sensor/thermometer.h>
#include <WiFiClient.h>

namespace Supla {
namespace Sensor {

class DirectLinksSensorThermometer : public Supla::Sensor::Thermometer {
 public:
  DirectLinksSensorThermometer(const char *host, bool isSecured = true);
  ~DirectLinksSensorThermometer();

  void onInit();
  void setHost(const char *host);
  void setPort(uint16_t port);
  void setUrl(const char *url);
  void enableSSL(bool isSecured);

  double getValue();

  bool checkConnection();
  void toggleConnection();
  bool openConnection();
  bool closeConnection();
  void sendRequest(const char *url);
  void send(const char *url);
  void iterateAlways();

 protected:
  WiFiClient *client;

  double _temp;
  int8_t retryCount;

  bool _isSecured;
  char _urlSensor[MAX_DIRECT_LINKS_SIZE];
  char _host[MAX_HOST_SIZE];
  uint16_t _port;
};

};  // namespace Sensor
};  // namespace Supla

#endif
#endif