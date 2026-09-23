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

/*  - for WT32-ETH01 - */

#ifndef SRC_SUPLA_GUI_NETWORK_WT32_ETH01_H_
#define SRC_SUPLA_GUI_NETWORK_WT32_ETH01_H_

#ifdef SUPLA_WT32_ETH01_LAN8720
#include <Arduino.h>

// LAN8720 podłącza się do wbudowanego kontrolera EMAC, który mają tylko
// ESP32 (i ESP32-P4). C3, C5, C6, S2, S3 go nie mają — bez tego czytelnego
// komunikatu build kończy się błędami w esp32eth.h.
#if !defined(SOC_EMAC_SUPPORTED) || !SOC_EMAC_SUPPORTED
#error "SUPLA_WT32_ETH01_LAN8720: ten uklad nie ma wbudowanego kontrolera Ethernet (EMAC). Flaga dziala tylko na ESP32."
#endif
#include <ETH.h>
#include <supla/network/network.h>
#include <supla/supla_lib_config.h>
#include <supla/log_wrapper.h>
#include <supla/network/esp32eth.h>
#include <supla/tools.h>

namespace Supla {
class GUI_WT32_ETH01 : public Supla::ESPETH {
 public:
  explicit GUI_WT32_ETH01(uint8_t ethmode);
  void setup();
};
};  // namespace Supla

#endif
#endif  // SRC_GUI_SUPLA_NETWORK_WT32_ETH01_H_
