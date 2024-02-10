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

#include "SuplaWebPageConfig.h"

#ifdef SUPLA_CONFIG
void createWebPageConfig() {
  WebServer->httpServer->on(getURL(PATH_CONFIG), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleConfig();
    else
      handleConfigSave();
  });
}

void handleConfigSave() {
  if (!WebServer->saveGPIO(INPUT_CFG_LED_GPIO, FUNCTION_CFG_LED)) {
    handleConfig(6);
    return;
  }
  else {
    String input = INPUT_CFG_LED_LEVEL;
    ConfigESP->setLevel(ConfigESP->getGpio(FUNCTION_CFG_LED),  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  if (!WebServer->saveGPIO(INPUT_CFG_BTN_GPIO, FUNCTION_CFG_BUTTON)) {
    handleConfig(6);
    return;
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_MODE).c_str(), "") != 0)
    ConfigManager->set(KEY_CFG_MODE, WebServer->httpServer->arg(INPUT_CFG_MODE).c_str());

  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_SSL).c_str(), "") != 0)
    ConfigManager->set(KEY_ENABLE_SSL, 1);
  else
    ConfigManager->set(KEY_ENABLE_SSL, 0);

  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_AVAILABLE_GUI).c_str(), "") != 0)
    ConfigManager->set(KEY_ENABLE_GUI, 1);
  else
    ConfigManager->set(KEY_ENABLE_GUI, 0);

  if (strcmp(WebServer->httpServer->arg(INPUT_FORCE_RESTART_ESP).c_str(), "") != 0)
    ConfigManager->set(KEY_FORCE_RESTART_ESP, 1);
  else
    ConfigManager->set(KEY_FORCE_RESTART_ESP, 0);

#ifdef SUPLA_DEEP_SLEEP
  if (strcmp(WebServer->httpServer->arg(INPUT_DEEP_SLEEP_TIME).c_str(), "") != 0)
    ConfigManager->set(KEY_DEEP_SLEEP_TIME, WebServer->httpServer->arg(INPUT_DEEP_SLEEP_TIME).c_str());

#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleConfig(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleConfig(2);
      break;
  }
}

void handleConfig(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_CONFIG);

  addForm(F("post"), PATH_CONFIG);
  addFormHeader(S_GPIO_SETTINGS_FOR_CONFIG);
  addListGPIOBox(INPUT_CFG_LED_GPIO, F("LED"), FUNCTION_CFG_LED);

  selected = ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED));
  addListBox(INPUT_CFG_LED_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);

  addListGPIOBox(INPUT_CFG_BTN_GPIO, S_BUTTON, FUNCTION_CFG_BUTTON);

  selected = ConfigManager->get(KEY_CFG_MODE)->getValueInt();
  addListBox(INPUT_CFG_MODE, S_CFG_MODE, CFG_MODE_P, 2, selected);

  addFormHeaderEnd();

  addFormHeader(F("Połączenie"));
  selected = ConfigManager->get(KEY_ENABLE_SSL)->getValueBool();
  addCheckBox(INPUT_CFG_SSL, F("SSL"), selected);
  addFormHeaderEnd();

  addFormHeader(F("GUI podczas normalnej pracy"));
  selected = ConfigManager->get(KEY_ENABLE_GUI)->getValueBool();
  addCheckBox(INPUT_CFG_AVAILABLE_GUI, F("GUI"), selected);
  addFormHeaderEnd();

  addFormHeader(F("Gdy brak połączenia"));
  selected = ConfigManager->get(KEY_FORCE_RESTART_ESP)->getValueBool();
  addCheckBox(INPUT_FORCE_RESTART_ESP, F("Wymuś reset ESP"), selected);
  addFormHeaderEnd();

#ifdef SUPLA_DEEP_SLEEP
  addFormHeader(String(S_SETTING_FOR) + S_SPACE + "uśpienia");
  addNumberBox(INPUT_DEEP_SLEEP_TIME, F("Czas uśpienia[min]"), KEY_DEEP_SLEEP_TIME, 999);
  addFormHeaderEnd();
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();

  addButton(S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}
#endif
