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

#ifdef SUPLA_MS5611
#ifndef _ms5611_h
#define _ms5611_h

// Dependency: MS5611 library - use library manager to install it
// https://github.com/RobTillaart/MS5611

#include <MS5611.h>
#include <Wire.h>
#include <supla/sensor/therm_press_meter.h>

namespace Supla {
namespace Sensor {
class MS5611Sensor : public ThermPressMeter {
 public:
  MS5611Sensor(int altitude) {
    this->altitude=altitude;
    Serial.print(F("MS5611 initialized with altitude: "));
    Serial.println(altitude);
  }

  double getValue() {
    Serial.println(F("MS5611 getting temp."));
    float value = TEMPERATURE_NOT_AVAILABLE;
    value = ms5611.getTemperature();

    if (isnan(value)) {
      Serial.println(F("MS5611 temp not available."));
      value = TEMPERATURE_NOT_AVAILABLE;
    }

    return value;
  }

  double getPressure() {
    Serial.println(F("MS5611 getting pres."));
    float value = -123;
    int result=ms5611.read(); 
     if (result != MS5611_READ_OK)
  {
    Serial.print("Error in read: ");
    Serial.println(result);
  }
    else{
    value = ms5611.getPressure();
    float temp=ms5611.getTemperature();
    float multiplier=0.0065;
    value=pow(((multiplier * altitude) / (temp+273.15 ) + 1), 5.255) * value;
    }
    return value;
  }

  void onInit() {
    Serial.println(F("MS5611 init."));
    ms5611= MS5611(0x77);
    if (ms5611.begin() == true)
    {
      Serial.println("MS5611 found.");
    }
    else
    {
      Serial.println("MS5611 not found.");
    }
    pressureChannel.setNewValue(getPressure());
    channel.setNewValue(getValue(), HUMIDITY_NOT_AVAILABLE);
  }

 protected:
  ::MS5611 ms5611;  // I2C
  float altitude=248;
};

};  // namespace Sensor
};  // namespace Supla

#endif
#endif
