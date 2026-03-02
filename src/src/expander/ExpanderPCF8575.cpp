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
#include "ExpanderPCF8575.h"

#ifdef SUPLA_PCF8575

namespace Supla {
namespace Control {

ExpanderPCF8575::ExpanderPCF8575(TwoWire *wire, uint8_t address) : Supla::Io::Base(false) {
  if (_control.begin(address, wire)) {
    Serial.print("PCF8575 is connected address: ");
    Serial.println(address, HEX);
    isConnected = true;
  }
  else {
    Serial.println("Couldn't find PCF8575");
  }
}

void ExpanderPCF8575::onInit() {
}

void ExpanderPCF8575::customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
  if (isConnected)
    _control.pinMode(pin, mode);
}

int ExpanderPCF8575::customDigitalRead(int channelNumber, uint8_t pin) {
  return isConnected ? _control.digitalRead(pin) : 0;
}

void ExpanderPCF8575::customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if (isConnected)
    _control.digitalWrite(pin, val);
}

}  // namespace Control
}  // namespace Supla
#endif