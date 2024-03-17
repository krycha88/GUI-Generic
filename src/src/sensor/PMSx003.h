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
#ifndef _pmsx003_h
#define _pmsx003_h

#include <Arduino.h>
#include <supla/sensor/general_purpose_measurement.h>
#include <supla/element.h>

#include <PMserial.h>

namespace Supla {
namespace Sensor {

class PMSx003 : public Element {
 public:
  explicit PMSx003(int8_t pin_rx, int8_t pin_tx = -1) {
    sensor = new SerialPM(PLANTOWER_AUTO, pin_rx, pin_tx);
    sensor->init();
  }

  void iterateAlways() {
    if (sleepSensor && (millis() - lastSleepTime > 900000)) {  // 15min
      lastReadTime = millis();
      Serial.println(F("Turning ON PMS sensor"));
      sensor->wake();
      sleepSensor = false;
    }

    if (!sleepSensor && (millis() - lastReadTime > 30000)) {  // 30s
      sensor->read();

      if (sensor->has_particulate_matter()) {
        Serial.println(F("Turning OFF PMS sensor"));
        sensor->sleep();
        lastSleepTime = millis();
        sleepSensor = true;
      }
      else {
        lastReadTime = millis();
      }

      switch (sensor->status) {
        case this->sensor->OK:  // should never come here
          break;                // included to compile without warnings
        case this->sensor->ERROR_TIMEOUT:
          Serial.println(F(PMS_ERROR_TIMEOUT));
          break;
        case this->sensor->ERROR_MSG_UNKNOWN:
          Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
          break;
        case this->sensor->ERROR_MSG_HEADER:
          Serial.println(F(PMS_ERROR_MSG_HEADER));
          break;
        case this->sensor->ERROR_MSG_BODY:
          Serial.println(F(PMS_ERROR_MSG_BODY));
          break;
        case this->sensor->ERROR_MSG_START:
          Serial.println(F(PMS_ERROR_MSG_START));
          break;
        case this->sensor->ERROR_MSG_LENGTH:
          Serial.println(F(PMS_ERROR_MSG_LENGTH));
          break;
        case this->sensor->ERROR_MSG_CKSUM:
          Serial.println(F(PMS_ERROR_MSG_CKSUM));
          break;
        case this->sensor->ERROR_PMS_TYPE:
          Serial.println(F(PMS_ERROR_PMS_TYPE));
          break;
      }
    }
  }

  void onInit() {
    lastReadTime = millis();
    Serial.println(F("Turning ON PMS sensor"));
    sensor->wake();
    sensor->read();
    sleepSensor = false;
  }

  SerialPM *sensor;

 protected:
  unsigned long lastSleepTime = 0;
  unsigned long lastReadTime = 0;
  bool sleepSensor = false;
};

class PMS_PM01 : public GeneralPurposeMeasurement {
 public:
  PMS_PM01(PMSx003 *sensor) {
    pmsx003 = sensor;
    this->setKeepHistory(SUPLA_GENERAL_PURPOSE_MEASUREMENT_CHART_TYPE_LINEAR);
    this->setDefaultUnitAfterValue("μg/m³");
  }

  double getValue() {
    double value = NAN;
    if (pmsx003->sensor->has_particulate_matter()) {
      Serial.print(F("PM1.0 : "));
      Serial.println(pmsx003->sensor->pm01);
      value = pmsx003->sensor->pm01;
    }
    return value;
  }

 protected:
  PMSx003 *pmsx003;
};

class PMS_PM25 : public GeneralPurposeMeasurement {
 public:
  PMS_PM25(PMSx003 *sensor) {
    pmsx003 = sensor;
    this->setKeepHistory(SUPLA_GENERAL_PURPOSE_MEASUREMENT_CHART_TYPE_LINEAR);
    this->setDefaultUnitAfterValue("μg/m³");
  }

  double getValue() {
    double value = NAN;
    if (pmsx003->sensor->has_particulate_matter()) {
      Serial.print(F("PM2.5 : "));
      Serial.println(pmsx003->sensor->pm25);
      value = pmsx003->sensor->pm25;
    }
    return value;
  }

 protected:
  PMSx003 *pmsx003;
};

class PMS_PM10 : public GeneralPurposeMeasurement {
 public:
  PMS_PM10(PMSx003 *sensor) {
    pmsx003 = sensor;
    this->setKeepHistory(SUPLA_GENERAL_PURPOSE_MEASUREMENT_CHART_TYPE_LINEAR);
    this->setDefaultUnitAfterValue("μg/m³");
  }

  double getValue() {
    double value = NAN;
    if (pmsx003->sensor->has_particulate_matter()) {
      Serial.print(F("PM10 : "));
      Serial.println(pmsx003->sensor->pm10);
      value = pmsx003->sensor->pm10;
    }
    return value;
  }

 protected:
  PMSx003 *pmsx003;
};

}  // namespace Sensor
}  // namespace Supla

#endif