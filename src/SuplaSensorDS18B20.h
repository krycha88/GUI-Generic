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

#ifndef SuplaSensorDS18B20_h
#define SuplaSensorDS18B20_h

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <supla-common/log.h>
#include <supla/sensor/thermometer.h>


class OneWireBus {
 public:
  OneWireBus(uint8_t pinNumberConfig);
  int8_t getIndex(uint8_t *deviceAddress);

  uint8_t pin;
  OneWireBus *nextBus;
  unsigned long lastReadTime;
  DallasTemperature sensors;

 protected:
  OneWire oneWire;
};

class DS18B20 : public Supla::Sensor::Thermometer {
 public:
  DS18B20(uint8_t pin, uint8_t *deviceAddress = nullptr);
  void iterateAlways();
  double getValue();
  void onInit();
  uint8_t getPin();
  void setDeviceAddress(uint8_t *deviceAddress = nullptr);

 protected:
  static OneWireBus *oneWireBus;
  OneWireBus *myBus;
  DeviceAddress address;
  int8_t retryCounter;
  double lastValidValue;
};

#endif  // SuplaSensorDS18B20_h
