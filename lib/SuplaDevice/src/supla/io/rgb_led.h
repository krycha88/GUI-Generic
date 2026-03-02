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

#pragma once

#include <supla/io.h>

namespace Supla {
namespace Io {
class RgbLed : public Supla::Io::Base {
 public:
  RgbLed(
         uint8_t red = 255,
         uint8_t green = 255,
         uint8_t blue = 255)
      : Supla::Io::Base(false),
        red_(red),
        green_(green),
        blue_(blue) {
  }

  void customPinMode(int channelNumber, uint8_t pin, uint8_t mode) override {
  }

  void customDigitalWrite(int channelNumber,
                          uint8_t pin,
                          uint8_t val) override {
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || \
    CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C3
    if (val == HIGH) {
      rgbLedWrite(pin, red_, green_, blue_);
    } else {
      rgbLedWrite(pin, 0, 0, 0);
    }
#endif
  }

  int customDigitalRead(int channelNumber, uint8_t pin) override {
    return 0;
  }

  unsigned int customPulseIn(int channelNumber,
                             uint8_t pin,
                             uint8_t value,
                             uint64_t timeoutMicro) override {
    return 0;
  }

  void customAnalogWrite(int channelNumber, uint8_t pin, int val) override {
    // Not used for RGB LED
  }

  int customAnalogRead(int channelNumber, uint8_t pin) override {
    return 0;
  }

  void setColor(uint8_t red, uint8_t green, uint8_t blue) {
    red_ = red;
    green_ = green;
    blue_ = blue;
  }

 protected:
  uint8_t red_ = 255;
  uint8_t green_ = 255;
  uint8_t blue_ = 255;
};
};  // namespace Io
};  // namespace Supla