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
#include "../../GUIGenericCommonDefined.h"

#ifdef SUPLA_PCF8574
#ifndef EXPANDER_PCF8574_H_
#define EXPANDER_PCF8574_H_

#include <supla/io.h>
#include <supla/element.h>
#include <Adafruit_PCF8574.h>

namespace Supla {
namespace Control {
class ExpanderPCF8574 : public Supla::Io::Base, public Supla::Element {
 public:
  ExpanderPCF8574(TwoWire *wire, uint8_t address);

  virtual void onInit();

  virtual void customPinMode(int channelNumber, uint8_t pin, uint8_t mode);
  virtual int customDigitalRead(int channelNumber, uint8_t pin);
  virtual void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val);

  virtual void customAnalogWrite(int channelNumber, uint8_t pin, int val){};
  virtual unsigned int customPulseIn(int channelNumber, uint8_t pin, uint8_t value, uint64_t timeoutMicro) {
    return 0;
  }
  virtual int customAnalogRead(int channelNumber, uint8_t pin) {
    return 0;
  }

 protected:
  Adafruit_PCF8574 _control;
  bool isConnected = false;
};
};  // namespace Control
};  // namespace Supla

#endif
#endif