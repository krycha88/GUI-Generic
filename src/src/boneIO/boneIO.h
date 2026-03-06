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
#ifdef SUPLA_BONEIO
#ifndef _bone_IO_h
#define _bone_IO_h

#if defined(ARDUINO_ARCH_ESP32)
#if defined(SUPLA_BONEIO_32x10A) || defined(SUPLA_BONEIO_24x16A)
#define SUPLA_BONEIO
#endif
#endif

#if defined(SUPLA_BONEIO)

#if defined(SUPLA_MCP23017)
#define USE_MCP_OUTPUT
#endif

#if defined(SUPLA_BONEIO_24x16A)
#define BONEIO_BOARD_OUTPUT_24
#endif

#if defined(SUPLA_ROLLERSHUTTER)
#define BONEIO_ROLLER_SHUTTER_CHANNEL
#else
#define BONEIO_RELAY_CHANNEL
#endif

#include <Arduino.h>
#include "../sensor/LM75.h"
#include "../../SuplaDeviceGUI.h"
#include "../expander/ExpanderMCP23017.h"
#include "../expander/ExpanderPCF8574.h"
#include "../expander/ExpanderPCF8575.h"
#include "display/SSD1306.h"
#ifdef SUPLA_INA219
#include "../sensor/INA_219.h"
#endif

#define P00 0
#define P01 1
#define P02 2
#define P03 3
#define P04 4
#define P05 5
#define P06 6
#define P07 7

#define P10 8
#define P11 9
#define P12 10
#define P13 11
#define P14 12
#define P15 13
#define P16 14
#define P17 15

#define BONEIO_RELAY_CONFIG 30

namespace Supla {

struct DevicePin {
  Supla::Io::Base *io;
  int pin;
};

struct RelayButtonPair {
  Supla::Control::Relay *relay;
  Supla::Control::Button *button;
};

struct RollerShutterButtonPair {
  Supla::Control::RollerShutter *shutter;
  Supla::Control::Button *openButton;
  Supla::Control::Button *closeButton;
};

class boneIO {
 public:
  boneIO();

 private:
  RollerShutterButtonPair
  createRollerShutterChannel(DevicePin shutterRelayUpPin, DevicePin shutterRelayDownPin, DevicePin openButtonPin, DevicePin closeButtonPin);
  RelayButtonPair createRelayChannel(DevicePin relayPin, DevicePin buttonPin);
};

};  // namespace Supla
#endif
#endif
#endif