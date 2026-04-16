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
#ifdef SUPLA_WT32_ETH01_LAN8720
#include "SuplaGuiWt32_eth01.h"
#include "../../SuplaDeviceGUI.h"

namespace Supla {
GUI_WT32_ETH01::GUI_WT32_ETH01(uint8_t ethmode) : Supla::ESPETH(ethmode) {
}

void GUI_WT32_ETH01::setup() {
  ESPETH::setup();

  if (mode == Supla::DEVICE_MODE_CONFIG) {
    uint8_t mac[6] = {};
    WiFi.macAddress(mac);
    char macStr[12 + 6] = {};
    generateHexString(mac, macStr, 6);

    String cstr = "SUPLA-GUI-Generic-";
    cstr.reserve(32);
    cstr += macStr;

    SUPLA_LOG_INFO("WiFi: enter config mode with SSID: \"%s\"", cstr.c_str());
    WiFi.mode(WIFI_MODE_AP);

    WiFi.softAP(cstr.c_str(), "", 6);

    Supla::GUI::crateWebServer();
  }
  else if (ConfigManager->get(KEY_ENABLE_GUI)->getValueInt()) {
    Supla::GUI::crateWebServer();
  }

  delay(0);
}

};  // namespace Supla
#endif
