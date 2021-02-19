#include "SuplaWebPageConfig.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageConfig *WebPageConfig = new SuplaWebPageConfig();

SuplaWebPageConfig::SuplaWebPageConfig() {
}

void SuplaWebPageConfig::createWebPageConfig() {
  String path;
  path = PATH_START;
  path += PATH_CONFIG;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageConfig::handleConfig, this));
  path = PATH_START;
  path += PATH_SAVE_CONFIG;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageConfig::handleConfigSave, this));
}

void SuplaWebPageConfig::handleConfig() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  supla_webpage_config(0);
}

void SuplaWebPageConfig::handleConfigSave() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (!WebServer->saveGPIO(INPUT_CFG_LED_GPIO, FUNCTION_CFG_LED)) {
    supla_webpage_config(6);
    return;
  }

  uint8_t key = KEY_GPIO + ConfigESP->getGpio(FUNCTION_CFG_LED);
  String input = INPUT_CFG_LED_LEVEL;
  ConfigManager->setElement(key, LEVEL_RELAY, WebServer->httpServer->arg(input).toInt());

  if (!WebServer->saveGPIO(INPUT_CFG_BTN_GPIO, FUNCTION_CFG_BUTTON)) {
    supla_webpage_config(6);
    return;
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_CFG_MODE).c_str(), "") != 0) {
    ConfigManager->set(KEY_CFG_MODE, WebServer->httpServer->arg(INPUT_CFG_MODE).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Config save"));
      supla_webpage_config(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_config(2);
      break;
  }
}

void SuplaWebPageConfig::supla_webpage_config(int save) {
  uint8_t selected, suported;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONFIG);

  addForm(webContentBuffer, F("post"), PATH_SAVE_CONFIG);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_CONFIG);
  addListGPIOBox(webContentBuffer, INPUT_CFG_LED_GPIO, F("LED"), FUNCTION_CFG_LED);
  selected = ConfigESP->getLevel(FUNCTION_CFG_LED);
  addListBox(webContentBuffer, INPUT_CFG_LED_LEVEL, S_STATE_CONTROL, LEVEL_P, 2, selected);

  addListGPIOBox(webContentBuffer, INPUT_CFG_BTN_GPIO, S_BUTTON, FUNCTION_CFG_BUTTON);

  selected = ConfigManager->get(KEY_CFG_MODE)->getValueInt();
  addListBox(webContentBuffer, INPUT_CFG_MODE, S_CFG_MODE, CFG_MODE_P, 2, selected);

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendContent();
}
