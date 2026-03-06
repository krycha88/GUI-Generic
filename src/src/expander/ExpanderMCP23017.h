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

#ifdef SUPLA_MCP23017

#ifndef EXPANDER_MCP23017_H_
#define EXPANDER_MCP23017_H_

#include <supla/io.h>
#include <supla/element.h>
#include <Wire.h>

class MCP23017 {
 public:
  MCP23017();

  typedef std::function<void(uint16_t pins, uint16_t values)> callback_t;

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  void init(uint8_t sda, uint8_t scl, bool fast = true);
  void init(bool fast = true) {
    init(SDA, SCL, fast);
  }
#else
  void init(bool fast = true);
#endif

  bool begin(uint8_t address, TwoWire *wire = &Wire);

  void pinMode(uint8_t pin, uint8_t mode);
  void setPullup(uint8_t pin, bool pullup, bool inverse = false);
  bool digitalRead(uint8_t pin);
  void digitalWrite(uint8_t pin, bool value);
  uint16_t digitalReads();
  void digitalWrites(uint16_t values);

  void attachInterrupt(uint8_t pin, callback_t callback);
  void detachInterrupt(uint8_t pin);
  void setupInterrupt(uint8_t pin, bool enable = true);
  void setupInterrupts(uint16_t pins, bool enable = true);

 protected:
  bool writeReg(uint8_t reg, uint8_t value);
  bool writeReg16(uint8_t reg, uint16_t value);
  uint8_t readReg(uint8_t reg);
  uint16_t readReg16(uint8_t reg);
  bool updateReg(uint8_t reg, uint8_t andMask, uint8_t orMask);
  bool updateReg16(uint8_t reg, uint16_t andMask, uint16_t orMask);

  void _interrupt();

  uint8_t _address;
  uint16_t ba = 0;
  unsigned long get_ba = 0;
  callback_t _callback;
  TwoWire *_wire;
};

namespace Supla {
namespace Control {
class ExpanderMCP23017 : public Supla::Io::Base, public Supla::Element {
 public:
  ExpanderMCP23017(TwoWire *wire, uint8_t address);

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
  MCP23017 _control;
  bool isConnected = false;
};
};  // namespace Control
};  // namespace Supla

#endif
#endif
