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
#ifdef SUPLA_WAKE_ON_LAN
#include "WakeOnLanRelay.h"
#include <supla/storage/storage.h>

namespace Supla {
namespace Control {
WakeOnLanRelay::WakeOnLanRelay(const char *MACAddress) : VirtualRelay(), WOL(UDP) {
  channel.setDefault(SUPLA_CHANNELFNC_CONTROLLINGTHEGATEWAYLOCK);

  if (MACAddress) {
    strncpy(this->MACAddress, MACAddress, MAC_ADDRESS_SIZE);
    this->MACAddress[MAC_ADDRESS_SIZE - 1] = '\0';
  }
}

void WakeOnLanRelay::onInit() {
  if (stateOnInit == STATE_ON_INIT_ON || stateOnInit == STATE_ON_INIT_RESTORED_ON) {
    turnOn();
  }
  else {
    turnOff();
  }

  WOL.setRepeat(3, 100);  // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
}

void WakeOnLanRelay::turnOn(_supla_int_t duration) {
  durationMs = duration;
  durationTimestamp = millis();
  if (keepTurnOnDurationMs) {
    durationMs = storedTurnOnDurationMs;
  }
  state = true;

  channel.setNewValue(state);
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);

  wakePC();
}

void WakeOnLanRelay::wakePC() {
  WOL.sendMagicPacket(this->MACAddress);  // Send Wake On Lan packet with the above MAC address. Default to port 9.
                                          // WOL.sendMagicPacket(MACAddress, 7); // Change the port number
}

}  // namespace Control
}  // namespace Supla
#endif