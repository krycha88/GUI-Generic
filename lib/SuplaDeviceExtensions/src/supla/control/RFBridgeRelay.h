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
#ifdef SUPLA_RF_BRIDGE

#ifndef _RF_Bridge_Relay_h
#define _RF_Bridge_Relay_h

#include <Arduino.h>
#include <RCSwitch.h>
#include <supla/control/RFBridge.h>
#include <supla/control/relay.h>
#include <supla/storage/storage.h>
#include <supla/io.h>

namespace Supla {
namespace Control {

class RFBridgeRelay : public Relay, public RFBridge{
 public:
  RFBridgeRelay(int transmitterPin,
                int pin,
                bool highIsOn,
                _supla_int_t functions =
                    (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

  void turnOn(_supla_int_t duration = 0);
  void turnOff(_supla_int_t duration = 0);

};

};  // namespace Control
};  // namespace Supla
#endif
#endif