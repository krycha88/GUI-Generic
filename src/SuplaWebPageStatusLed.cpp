#include "SuplaWebPageStatusLed.h"
#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

void createWebStatusLed() {
  WebServer->httpServer->on(getURL(PATH_LED), handleStatusLed);
  WebServer->httpServer->on(getURL(PATH_SAVE_LED), handleStatusLedSave);
}

void handleStatusLed() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  webStatusLed(0);
}

void handleStatusLedSave() {
  uint8_t nr;

  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_LEVEL_LED).c_str(), "") != 0) {
    ConfigManager->set(KEY_LEVEL_LED, WebServer->httpServer->arg(INPUT_LEVEL_LED).c_str());
  }

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (!WebServer->saveGPIO(INPUT_LED, FUNCTION_LED, nr)) {
      webStatusLed(6);
      return;
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      webStatusLed(1);
      break;
    case E_CONFIG_FILE_OPEN:
      webStatusLed(2);
      break;
  }
}

void webStatusLed(int save) {
  uint8_t nr, selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_LED);
  addForm(webContentBuffer, F("post"), PATH_SAVE_LED);

  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" LED"));
  selected = ConfigManager->get(KEY_LEVEL_LED)->getValueInt();
  addListBox(webContentBuffer, INPUT_LEVEL_LED, S_STATE_CONTROL, LEVEL_P, 2, selected);

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_LED, F("LED"), FUNCTION_LED, nr);
  }

  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}