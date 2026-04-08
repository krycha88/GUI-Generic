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

#ifndef _direct_links_multi_h
#define _direct_links_multi_h

#include "DirectLinksConnect.h"

#include <ArduinoJson.h>
#include <supla/sensor/thermometer.h>
#include <supla/sensor/therm_hygro_meter.h>
#include "pressure.h"
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/sensor/electricity_meter.h>
#include <supla/sensor/distance.h>

namespace Supla {
namespace Sensor {

class DirectLinksThermometer : public DirectLinksConnect, public Thermometer {
 public:
  DirectLinksThermometer(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  double getValue();

 private:
  void onInit();

  double temp = TEMPERATURE_NOT_AVAILABLE;
};

class DirectLinksThermHygroMeter : public DirectLinksConnect, public ThermHygroMeter {
 public:
  DirectLinksThermHygroMeter(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  double getTemp();
  double getHumi();

 private:
  void onInit();

  double temp = TEMPERATURE_NOT_AVAILABLE;
  double humi = HUMIDITY_NOT_AVAILABLE;
};

class DirectLinksPressMeter : public DirectLinksConnect, public Supla::Sensor::Pressure {
 public:
  DirectLinksPressMeter(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  double getValue();

 private:
  void onInit();
  double press = PRESSURE_NOT_AVAILABLE;
};

class DirectLinksOnePhaseElectricityMeter : public DirectLinksConnect, public OnePhaseElectricityMeter {
 public:
  DirectLinksOnePhaseElectricityMeter(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
};

class DirectLinksElectricityMeter : public DirectLinksConnect, public ElectricityMeter {
 public:
  DirectLinksElectricityMeter(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  void setPhaseData(int phaseIndex, JsonObject &phase);
};

class DirectLinksDistance : public DirectLinksConnect, public Supla::Sensor::Distance {
 public:
  DirectLinksDistance(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  double getValue();

 private:
  void onInit();
  double distance = DISTANCE_NOT_AVAILABLE;
};

class DirectLinksDepth : public DirectLinksConnect, public Supla::Sensor::Distance {
 public:
  DirectLinksDepth(const char *url, const char *host, bool isSecured = true);
  void sendRequest();
  double getValue();

 private:
  void onInit();
  double depth = DISTANCE_NOT_AVAILABLE;
};

};  // namespace Sensor
};  // namespace Supla

#endif
#endif