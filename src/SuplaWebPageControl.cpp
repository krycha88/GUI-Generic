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

#include "SuplaWebPageControl.h"

#ifdef GUI_CONTROL
void createWebPageControl() {
  WebServer->httpServer->on(getURL(PATH_CONTROL), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleControl();
    else
      handleControlSave();
  });

  WebServer->httpServer->on(getURL(PATH_BUTTON_SET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSetMCP23017();
      else
        handleButtonSaveSetMCP23017();
    }
    else {
#ifdef SUPLA_BUTTON
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSet();
      else
        handleButtonSaveSet();
#endif
    }
#else
#ifdef SUPLA_BUTTON
      if (WebServer->httpServer->method() == HTTP_GET)
        handleButtonSet();
      else
        handleButtonSaveSet();
#endif
#endif
  });
}
#endif

#ifdef GUI_CONTROL
void handleControlSave() {
  uint8_t nr;

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      if (!WebServer->saveGpioMCP23017(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        handleControl(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
        handleControl(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_BUTTON, nr, INPUT_MAX_BUTTON)) {
      handleControl(6);
      return;
    }
#endif

#ifdef SUPLA_ROLLERSHUTTER
    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr) {
      if (nr % 2 == 0) {
        if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO_STOP, FUNCTION_BUTTON_STOP, nr)) {
          handleControl(6);
          return;
        }
      }
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_BUTTON, WebServer->httpServer->arg(INPUT_MAX_BUTTON).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleControl(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleControl(2);
      break;
  }
}

void handleControl(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_CONTROL);

  addForm(webContentBuffer, F("post"), PATH_CONTROL);
  addFormHeader(webContentBuffer, S_GPIO_SETTINGS_FOR_BUTTONS);
  addNumberBox(webContentBuffer, INPUT_MAX_BUTTON, S_QUANTITY, KEY_MAX_BUTTON, ConfigESP->countFreeGpio(FUNCTION_BUTTON));

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
#ifdef SUPLA_MCP23017
    if (ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)) {
      addListMCP23017GPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, FUNCTION_BUTTON, nr, PATH_BUTTON_SET);
    }
    else {
      addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER), FUNCTION_BUTTON, nr);
    }
#else
    addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO, S_BUTTON, getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER), FUNCTION_BUTTON, nr);
#endif

#ifdef SUPLA_ROLLERSHUTTER
    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr
#ifdef SUPLA_MCP23017
        && !ConfigESP->checkActiveMCP23017(FUNCTION_BUTTON)
#endif
    ) {
      if (nr % 2 == 0) {
        addListGPIOLinkBox(webContentBuffer, INPUT_BUTTON_GPIO_STOP, String(S_BUTTON) + S_SPACE + " STOP",
                           getParameterRequest(PATH_BUTTON_SET, "stop"), FUNCTION_BUTTON_STOP, nr);
      }
    }
#endif
  }

  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_BUTTON)
void handleButtonSaveSet() {
  String input, button, buttonStop;
  uint8_t key, gpio;

  button = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  buttonStop = WebServer->httpServer->arg("stop");

  if (!buttonStop.isEmpty()) {
    gpio = ConfigESP->getGpio(buttonStop.toInt(), FUNCTION_BUTTON_STOP);
  }
  else {
    gpio = ConfigESP->getGpio(button.toInt(), FUNCTION_BUTTON);
  }
  key = KEY_GPIO + gpio;

  input = INPUT_BUTTON_INVERSED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, INVERSED_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, INVERSED_BUTTON, 0);
  }

  input = INPUT_BUTTON_LEVEL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 1);
  }
  else {
    ConfigManager->setElement(key, PULL_UP_BUTTON, 0);
  }

  input = INPUT_BUTTON_EVENT;
  ConfigManager->setElement(key, EVENT_BUTTON, WebServer->httpServer->arg(input).toInt());

  input = INPUT_BUTTON_ACTION;
  ConfigManager->setElement(key, ACTION_BUTTON, WebServer->httpServer->arg(input).toInt());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleButtonSet(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleButtonSet(2);
      break;
  }
}

void handleButtonSet(int save) {
  uint8_t gpio, selected;
  String button, buttonStop, url;

  button = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  buttonStop = WebServer->httpServer->arg("stop");

  WebServer->sendHeaderStart();

  if (!button.isEmpty() || !buttonStop.isEmpty()) {
    if (!buttonStop.isEmpty()) {
      gpio = ConfigESP->getGpio(buttonStop.toInt(), FUNCTION_BUTTON_STOP);
      url = getParameterRequest(PATH_BUTTON_SET, "stop", buttonStop);
    }
    else {
      gpio = ConfigESP->getGpio(button.toInt(), FUNCTION_BUTTON);
      url = getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, button);
    }

    webContentBuffer += SuplaSaveResult(save);
    webContentBuffer += SuplaJavaScript(url);

    addForm(webContentBuffer, F("post"), url);
    if (!buttonStop.isEmpty()) {
      addFormHeader(webContentBuffer, String(S_SETTING_FOR) + S_SPACE + S_BUTTON + S_SPACE + "STOP");
    }
    else {
      addFormHeader(webContentBuffer, String(S_BUTTON_NR_SETTINGS) + (button.toInt() + 1));
    }

    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > button.toInt() && buttonStop.isEmpty()) {
#ifdef SUPLA_ROLLERSHUTTER
      selected = ConfigESP->getPullUp(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
      selected = ConfigESP->getInversed(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);

      if (!buttonStop.isEmpty()) {
        selected = ConfigESP->getEvent(gpio);
        addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 2, selected);
      }

      if (button.toInt() % 2 == 0) {
        selected = ConfigESP->getEvent(gpio);
        addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
        selected = ConfigESP->getAction(gpio);
        addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_ROLLER_SHUTTER_P, 3, selected);
      }
#endif
    }
    else {
      selected = ConfigESP->getPullUp(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
      selected = ConfigESP->getInversed(gpio);
      addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);
      selected = ConfigESP->getEvent(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
      selected = ConfigESP->getAction(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);
    }

    addFormHeaderEnd(webContentBuffer);
    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}
#endif

#ifdef SUPLA_MCP23017
void handleButtonSetMCP23017(int save) {
  uint8_t gpio, selected;
  String button;

  button.reserve(2);

  WebServer->sendHeaderStart();
  button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!button.isEmpty())
    gpio = ConfigESP->getGpioMCP23017(button.toInt(), FUNCTION_BUTTON);
  else
    gpio = ConfigESP->getGpioMCP23017(1, FUNCTION_BUTTON);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, button));

  addForm(webContentBuffer, F("post"), getParameterRequest(PATH_BUTTON_SET, ARG_PARM_NUMBER, button));

  if (!button.isEmpty()) {
    addFormHeader(webContentBuffer, String(S_BUTTON_NR_SETTINGS) + button.toInt());
  }
  else {
    addFormHeader(webContentBuffer, S_SETTINGS_FOR_BUTTONS);
  }

  if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > button.toInt() && !button.isEmpty()) {
#ifdef SUPLA_ROLLERSHUTTER
    selected = ConfigESP->getPullUp(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
    selected = ConfigESP->getInversed(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);

    if (button.toInt() % 2 == 0) {
      selected = ConfigESP->getEvent(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
      selected = ConfigESP->getAction(gpio);
      addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_ROLLER_SHUTTER_P, 3, selected);
    }
#endif
  }
  else {
    selected = ConfigESP->getPullUp(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_LEVEL, S_INTERNAL_PULL_UP, selected);
    selected = ConfigESP->getInversed(gpio);
    addCheckBox(webContentBuffer, INPUT_BUTTON_INVERSED, S_REVERSE_LOGIC, selected);
    selected = ConfigESP->getEvent(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_EVENT, S_REACTION_TO, TRIGGER_P, 3, selected);
    selected = ConfigESP->getAction(gpio);
    addListBox(webContentBuffer, INPUT_BUTTON_ACTION, S_ACTION, ACTION_P, 3, selected);
  }

  addFormHeaderEnd(webContentBuffer);
  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_CONTROL);

  WebServer->sendHeaderEnd();
}

void handleButtonSaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input, button;
  uint8_t key, gpio, pullup, inversed, event, action;

  input.reserve(10);
  button.reserve(2);

  input = INPUT_BUTTON_EVENT;
  event = WebServer->httpServer->arg(input).toInt();

  input = INPUT_BUTTON_LEVEL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    pullup = 1;
  }
  else {
    pullup = 0;
  }

  input = INPUT_BUTTON_INVERSED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    inversed = 1;
  }
  else {
    inversed = 0;
  }

  input = INPUT_BUTTON_ACTION;
  action = WebServer->httpServer->arg(input).toInt();

  button = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!button.isEmpty()) {
    gpio = ConfigESP->getGpioMCP23017(button.toInt(), FUNCTION_BUTTON);
    key = KEY_GPIO + gpio;

    ConfigManager->setElement(key, PULL_UP_BUTTON, pullup);
    ConfigManager->setElement(key, INVERSED_BUTTON, inversed);
    ConfigManager->setElement(key, EVENT_BUTTON, event);
    ConfigManager->setElement(key, ACTION_BUTTON, action);
  }
  else {
    for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
      key = KEY_GPIO + gpio;
      ConfigManager->setElement(key, PULL_UP_BUTTON, pullup);
      ConfigManager->setElement(key, INVERSED_BUTTON, inversed);
      ConfigManager->setElement(key, EVENT_BUTTON, event);
      ConfigManager->setElement(key, ACTION_BUTTON, action);
    }
  }
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleButtonSetMCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleButtonSetMCP23017(2);
      break;
  }
}
#endif
