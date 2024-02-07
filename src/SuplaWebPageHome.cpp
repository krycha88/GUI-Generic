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

#include "SuplaWebPageHome.h"

void createWebPageHome() {
  WebServer->httpServer->on(PATH_START, [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    const char* rebotArg = WebServer->httpServer->arg(F(PATH_REBOT)).c_str();
    if (strcmp(rebotArg, "") != 0) {
      if (strcmp(rebotArg, "3") == 0) {
        handlePageHome(SaveResult::RESTART_MODULE);
      }

      if (strcmp(rebotArg, "1") == 0 || strcmp(rebotArg, "2") == 0 || strcmp(rebotArg, "3") == 0) {
        ConfigESP->rebootESP();
      }
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handlePageHome();
    else
      handlePageHomeSave();
  });
}

void handlePageHome(int save) {
  WebServer->sendHeaderStart();

  SuplaSaveResult(save);
  SuplaJavaScript();

  if (getCountSensorChannels() > 0) {
    addFormHeader();
#ifdef SUPLA_MODBUS_SDM
    if (Supla::GUI::smd) {
      addLabel("SuccCount:" + String(Supla::GUI::smd->getSuccCount()) + " ErrCount:" + String(Supla::GUI::smd->getErrCount()) +
               " ErrCode:" + String(Supla::GUI::smd->getErrCode()));
    }
#endif
#ifdef SUPLA_MODBUS_SDM_ONE_PHASE
    if (Supla::GUI::smd120) {
      addLabel("SuccCount:" + String(Supla::GUI::smd120->getSuccCount()) + " ErrCount:" + String(Supla::GUI::smd120->getErrCount()) +
               " ErrCode:" + String(Supla::GUI::smd120->getErrCode()));
    }
#endif
    for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          addLabel(String(element->getChannelNumber()) + S_SPACE + "-" + S_SPACE + String(channel->getValueDouble(), 2) + S_CELSIUS);
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          addLabel(String(element->getChannelNumber()) + S_SPACE + "-" + S_SPACE + String(channel->getValueDoubleFirst(), 2) + S_CELSIUS + S_SPACE +
                   String(channel->getValueDoubleSecond(), 2) + "%");
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
          addLabel(String(element->getChannelNumber()) + S_SPACE + "-" + S_SPACE + String(channel->getValueDoubleSecond(), 2) + "%");
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
          addLabel(String(element->getChannelNumber()) + S_SPACE + "-" + S_SPACE + String(channel->getValueDouble(), 2) + "m");
        }

#ifdef GUI_ALL_ENERGY
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
          TSuplaChannelExtendedValue* extValue = channel->getExtValue();
          if (extValue == nullptr)
            continue;

          TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
          if (emValue->m_count < 1 || emValue == nullptr)
            continue;

          String voltage = "";
          String power_active = "";
          String current = "";

          _supla_int_t flags = channel->getFlags();

          if (flags & (SUPLA_CHANNEL_FLAG_PHASE2_UNSUPPORTED | SUPLA_CHANNEL_FLAG_PHASE3_UNSUPPORTED)) {
            voltage += String(emValue->m[0].voltage[0] / 100.0) + " | ";
            power_active += String(emValue->m[0].power_active[0] / 100000.0) + " | ";
            current += String(emValue->m[0].current[0] / 1000.0) + " | ";
          }
          else {
            for (size_t i = 0; i < MAX_PHASES; i++) {
              voltage += String(emValue->m[0].voltage[i] / 100.0) + " | ";
              power_active += String(emValue->m[0].power_active[i] / 100000.0) + " | ";
              current += String(emValue->m[0].current[i] / 1000.0) + " | ";
            }
          }

          voltage.setCharAt(voltage.length() - 2, 'V');
          power_active.setCharAt(power_active.length() - 2, 'W');
          current.setCharAt(current.length() - 2, 'A');

          addLabel(voltage);
          addLabel(power_active);
          addLabel(current);
        }
#endif

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          addLabel(String(channel->getValueDouble()) + "hPa");
        }
      }

      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          addLabel(String(channel->getValueDouble()) + "hPa");
        }
      }
    }
    addFormHeaderEnd();
  }

  addForm(F("post"));

#ifdef SUPLA_WT32_ETH01_LAN8720

#else
  addFormHeader(S_SETTING_WIFI_SSID);
  addTextBox(INPUT_WIFI_SSID, S_WIFI_SSID, KEY_WIFI_SSID, 0, MAX_SSID, true);
  addTextBoxPassword(INPUT_WIFI_PASS, S_WIFI_PASS, KEY_WIFI_PASS, 0, MAX_PASSWORD, false);
  addTextBox(INPUT_HOSTNAME, S_HOST_NAME, KEY_HOST_NAME, 0, MAX_HOSTNAME, true);
  addFormHeaderEnd();
#endif

  addFormHeader(S_SETTING_SUPLA);
  addTextBox(INPUT_SERVER, S_SUPLA_SERVER, KEY_SUPLA_SERVER, DEFAULT_SERVER, 0, MAX_SUPLA_SERVER, true);
  addTextBox(INPUT_EMAIL, S_SUPLA_EMAIL, KEY_SUPLA_EMAIL, DEFAULT_EMAIL, 0, MAX_EMAIL, true);
  addFormHeaderEnd();

  addFormHeader(S_SETTING_ADMIN);
  addTextBox(INPUT_MODUL_LOGIN, S_LOGIN, KEY_LOGIN, 0, MAX_MLOGIN, true);
  addTextBoxPassword(INPUT_MODUL_PASS, S_LOGIN_PASS, KEY_LOGIN_PASS, MIN_PASSWORD, MAX_MPASSWORD, true);
  addFormHeaderEnd();

#ifdef SUPLA_BONEIO
  addFormHeader(String(S_SETTINGS_FOR) + S_SPACE + S_BONEIO);
  uint8_t selected = ConfigESP->getLevel(BONEIO_RELAY_CONFIG);
  addListBox(INPUT_RELAY_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);
  selected = ConfigESP->getMemory(BONEIO_RELAY_CONFIG);
  addListBox(INPUT_RELAY_MEMORY, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);
  addFormHeaderEnd();
#endif

#ifdef SUPLA_ROLLERSHUTTER
  uint8_t maxrollershutter = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  if (maxrollershutter >= 2) {
    addFormHeader(S_ROLLERSHUTTERS);
    addNumberBox(INPUT_ROLLERSHUTTER, S_QUANTITY, KEY_MAX_ROLLERSHUTTER, (maxrollershutter / 2));
    addFormHeaderEnd();
  }
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();

  addButton(S_DEVICE_SETTINGS, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handlePageHomeSave() {
  if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str(), "") != 0)
    ConfigManager->set(KEY_WIFI_SSID, WebServer->httpServer->arg(INPUT_WIFI_SSID).c_str());
  // if (strcmp(WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str(), "") != 0)
  ConfigManager->set(KEY_WIFI_PASS, WebServer->httpServer->arg(INPUT_WIFI_PASS).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_SERVER).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_SERVER, WebServer->httpServer->arg(INPUT_SERVER).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_EMAIL).c_str(), "") != 0)
    ConfigManager->set(KEY_SUPLA_EMAIL, WebServer->httpServer->arg(INPUT_EMAIL).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_HOSTNAME).c_str(), "") != 0)
    ConfigManager->set(KEY_HOST_NAME, WebServer->httpServer->arg(INPUT_HOSTNAME).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN, WebServer->httpServer->arg(INPUT_MODUL_LOGIN).c_str());
  if (strcmp(WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str(), "") != 0)
    ConfigManager->set(KEY_LOGIN_PASS, WebServer->httpServer->arg(INPUT_MODUL_PASS).c_str());

#ifdef SUPLA_BONEIO
  ConfigESP->setMemory(BONEIO_RELAY_CONFIG, WebServer->httpServer->arg(INPUT_RELAY_MEMORY).toInt());
  ConfigESP->setLevel(BONEIO_RELAY_CONFIG, WebServer->httpServer->arg(INPUT_RELAY_LEVEL).toInt());
#endif

#ifdef SUPLA_ROLLERSHUTTER
  if (strcmp(WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_ROLLERSHUTTER, WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).toInt());

    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() > 0) {
      ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer->arg(INPUT_ROLLERSHUTTER).toInt() * 2);
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      if (ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL) {
        handlePageHome(SaveResult::DATA_SAVE);
      }
      else {
        handlePageHome(SaveResult::DATA_SAVE_MODE_CONFIG);
      }
      break;

    case E_CONFIG_FILE_OPEN:
      handlePageHome(2);
      break;
  }
}