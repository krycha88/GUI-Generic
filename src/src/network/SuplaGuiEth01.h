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

/*  - for ETH01 - */

#ifndef SRC_SUPLA_GUI_NETWORK_ETH01_H_
#define SRC_SUPLA_GUI_NETWORK_ETH01_H_

#ifdef SUPLA_ETH01_LAN8720
#include <Arduino.h>
#include <ETH.h>
#include <supla/network/network.h>
#include <supla/supla_lib_config.h>
#include <supla/log_wrapper.h>
#include <supla/network/esp32eth.h>
#include <supla/tools.h>

namespace Supla {
class GUI_ETH01 : public Supla::ESPETH {
 public:
  explicit GUI_ETH01(uint8_t ethmode, unsigned char *ip = nullptr);

  void setup();

 private:
  bool ethConfigured = false;
};
};  // namespace Supla

#endif
#endif  // SRC_GUI_SUPLA_NETWORK_WT32_ETH01_H_
