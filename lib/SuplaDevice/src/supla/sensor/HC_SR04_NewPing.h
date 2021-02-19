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

#ifndef _hc_sr04_newping_h
#define _hc_sr04_newping_h

#include <NewPing.h>

#include "supla/channel.h"
#include "supla/sensor/distance.h"

namespace Supla {
namespace Sensor {
class HC_SR04_NewPing : public Distance {
 public:
  HC_SR04_NewPing(int8_t trigPin, int8_t echoPin, int16_t depth = 0) : retryCount(0) {
    _depth = depth;
    sonar = new NewPing(trigPin, echoPin);
  }
  void onInit() {
    channel.setNewValue(getValue());
  }

  virtual double getValue() {
    unsigned long uS = sonar->ping_median();

    if (uS < 50) {
      retryCount++;
      if (retryCount > 3) {
        retryCount = 0;
        value = DISTANCE_NOT_AVAILABLE;
      }
    } else {
      retryCount = 0;
      unsigned long distance = sonar->convert_cm(uS);

      if (_depth > 0) {
        value = map(distance, 0, _depth, _depth, 0);
        if (value > _depth) value = 0;
      } else {
        value = distance;
      }
      value = value / 100.0;
    }
    return value;
  }

 protected:
  NewPing *sonar = nullptr;
  double value;
  int16_t _depth;
  int8_t retryCount;
};

};  // namespace Sensor
};  // namespace Supla

#endif
