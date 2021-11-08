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

#include "SuplaWebPageOther.h"

void createWebPageOther() {
#ifdef GUI_OTHER
  WebServer->httpServer->on(getURL(PATH_OTHER), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleOther();
    else
      handleOtherSave();
  });

#if defined(SUPLA_IMPULSE_COUNTER)
  WebServer->httpServer->on(getURL(PATH_IMPULSE_COUNTER_SET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleImpulseCounterSet();
    else
      handleImpulseCounterSaveSet();
  });
#endif

#if defined(SUPLA_HLW8012) || defined(SUPLA_CSE7766)
  if ((ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) ||
      ConfigESP->getGpio(FUNCTION_CSE7766_RX) != OFF_GPIO) {
    WebServer->httpServer->on(getURL(PATH_CALIBRATE), [&]() {
      if (!WebServer->isLoggedIn()) {
        return;
      }

      if (WebServer->httpServer->method() == HTTP_GET)
        handleCounterCalibrate();
      else
        handleCounterCalibrateSave();
    });
  }
#endif
#ifdef SUPLA_RF_BRIDGE
  // if (ConfigESP->getGpio(FUNCTION_RF_BRIDGE_RECEIVE) != OFF_GPIO) {
  WebServer->httpServer->on(getURL(PATH_BRIDGE), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      receiveCodeRFBridge();
  });
  //}
#endif

#endif
}

#ifdef GUI_OTHER
void handleOther(int save) {
  uint8_t nr, selected;

  WebServer->sendHeaderStart();
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);

  addForm(webContentBuffer, F("post"), PATH_OTHER);
#ifdef SUPLA_IMPULSE_COUNTER
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_IMPULSE_COUNTER);
  addNumberBox(webContentBuffer, INPUT_MAX_IMPULSE_COUNTER, S_QUANTITY, KEY_MAX_IMPULSE_COUNTER, ConfigESP->countFreeGpio(FUNCTION_IMPULSE_COUNTER));
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
    addListGPIOLinkBox(webContentBuffer, INPUT_IMPULSE_COUNTER_GPIO, F("IC GPIO"), getParameterRequest(PATH_IMPULSE_COUNTER_SET, ARG_PARM_NUMBER),
                       FUNCTION_IMPULSE_COUNTER, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_HLW8012
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_HLW8012);
  addListGPIOBox(webContentBuffer, INPUT_CF, F("CF"), FUNCTION_CF);
  addListGPIOBox(webContentBuffer, INPUT_CF1, F("CF1"), FUNCTION_CF1);
  addListGPIOBox(webContentBuffer, INPUT_SEL, F("SELi"), FUNCTION_SEL);
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    selected = Supla::GUI::counterHLW8012->getMode();
    addListBox(webContentBuffer, INPUT_MODE_HLW8012, "Mode", LEVEL_P, 2, selected);
    float count = Supla::GUI::counterHLW8012->getCounter();
    addNumberBox(webContentBuffer, INPUT_COUNTER_CHANGE_VALUE_HLW8012, String(S_IMPULSE_COUNTER_CHANGE_VALUE) + S_SPACE + F("[kWh]"), F("kWh"), false,
                 String(count / 100 / 1000));
    addLinkBox(webContentBuffer, S_CALIBRATION, getParameterRequest(PATH_CALIBRATE, ARG_PARM_URL) + PATH_HLW8012);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_CSE7766
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + F("CSE7766"));
  addListGPIOBox(webContentBuffer, INPUT_CSE7766_RX, F("RX"), FUNCTION_CSE7766_RX);
  if (ConfigESP->getGpio(FUNCTION_CSE7766_RX) != OFF_GPIO) {
    float count = Supla::GUI::counterCSE7766->getCounter();
    addNumberBox(webContentBuffer, INPUT_COUNTER_CHANGE_VALUE_CSE7766, String(S_IMPULSE_COUNTER_CHANGE_VALUE) + S_SPACE + F("[kWh]"), F("kWh"), false,
                 String(count / 100 / 1000));
    addLinkBox(webContentBuffer, S_CALIBRATION, getParameterRequest(PATH_CALIBRATE, ARG_PARM_URL) + PATH_CSE7766);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_PZEM_V_3
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + F("PZEM-004T V3") + S_SPACE + S_ELECTRIC_PHASE);
  for (nr = 1; nr <= 3; nr++) {
    if (nr >= 2)
      addListGPIOBox(webContentBuffer, INPUT_PZEM_RX, String(F("L")) + nr + F(" - RX") + S_OPTIONAL, FUNCTION_PZEM_RX, nr, true, "", true);
    else
      addListGPIOBox(webContentBuffer, INPUT_PZEM_RX, String(F("L")) + nr + F(" - RX"), FUNCTION_PZEM_RX, nr, true, "", true);

    addListGPIOBox(webContentBuffer, INPUT_PZEM_TX, String(F("L")) + nr + F(" - TX"), FUNCTION_PZEM_TX, nr, true, "", true);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_HC_SR04
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_HC_SR04);
  addListGPIOBox(webContentBuffer, INPUT_TRIG_GPIO, F("TRIG"), FUNCTION_TRIG);
  addListGPIOBox(webContentBuffer, INPUT_ECHO_GPIO, F("ECHO"), FUNCTION_ECHO);
  addNumberBox(webContentBuffer, INPUT_HC_SR04_MAX_SENSOR_READ, S_DEPTH_CM, S_SENSOR_READING_DISTANCE, false,
               ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValue());
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_RGBW
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_RGBW_RGB_DIMMER);
  addNumberBox(webContentBuffer, INPUT_RGBW_MAX, S_QUANTITY, KEY_MAX_RGBW, ConfigESP->countFreeGpio());
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_RGBW_RED, F("RED"), FUNCTION_RGBW_RED, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_GREEN, F("GREEN"), FUNCTION_RGBW_GREEN, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_BLUE, F("BLUE"), FUNCTION_RGBW_BLUE, nr, false);
    addListGPIOBox(webContentBuffer, INPUT_RGBW_BRIGHTNESS, F("WHITE / DIMMER"), FUNCTION_RGBW_BRIGHTNESS, nr, false);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#if defined(SUPLA_PUSHOVER)
  addFormHeader(webContentBuffer, String(S_SETTING_FOR) + S_SPACE + S_PUSHOVER);
  addTextBox(webContentBuffer, INPUT_PUSHOVER_TOKEN, F("Token"), KEY_PUSHOVER_TOKEN, 0, MAX_TOKEN_SIZE, false);
  addTextBox(webContentBuffer, INPUT_PUSHOVER_USER, F("Users"), KEY_PUSHOVER_USER, 0, MAX_USER_SIZE, false);
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
  String input;

  addFormHeader(webContentBuffer, String(S_DIRECT_LINKS) + S_SPACE + "-" + S_SPACE + S_TEMPERATURE);
  addNumberBox(webContentBuffer, INPUT_MAX_DIRECT_LINKS_SENSOR_THERMOMETR, S_QUANTITY, KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR, MAX_DIRECT_LINK);

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR)->getValueInt(); nr++) {
    input = INPUT_DIRECT_LINKS_SENSOR_THERMOMETR;
    input = input + nr;
    String massage = ConfigManager->get(KEY_DIRECT_LINKS_SENSOR_THERMOMETR)->getElement(nr).c_str();
    addTextBox(webContentBuffer, input, String(S_SENSOR) + S_SPACE + (nr + 1), massage, F("xx/xxxxxxxxx/read"), 0, MAX_DIRECT_LINKS_SIZE, false);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_RF_BRIDGE
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + F("RF BRIDGE"));
  addListGPIOBox(webContentBuffer, INPUT_RF_BRIDGE_TX, F("TX"), FUNCTION_RF_BRIDGE_TRANSMITTER);
  addListGPIOBox(webContentBuffer, INPUT_RF_BRIDGE_RX, F("RX"), FUNCTION_RF_BRIDGE_RECEIVE);
  if (ConfigESP->getGpio(FUNCTION_RF_BRIDGE_RECEIVE) != OFF_GPIO) {
    addLinkBox(webContentBuffer, String(S_CALIBRATION) + S_SPACE + S_CODES, PATH_BRIDGE);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleOtherSave() {
  uint8_t nr, last_value;

#ifdef SUPLA_IMPULSE_COUNTER
  // Supla::GUI::impulseCounter[0]->setCounter((unsigned long long)WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());

  last_value = ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt();
  for (nr = 0; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_IMPULSE_COUNTER_GPIO, FUNCTION_IMPULSE_COUNTER, nr, INPUT_MAX_IMPULSE_COUNTER)) {
      handleOther(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_IMPULSE_COUNTER).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_IMPULSE_COUNTER, WebServer->httpServer->arg(INPUT_MAX_IMPULSE_COUNTER).c_str());
  }
#endif

#ifdef SUPLA_HLW8012
  if (!WebServer->saveGPIO(INPUT_CF, FUNCTION_CF) || !WebServer->saveGPIO(INPUT_CF1, FUNCTION_CF1) || !WebServer->saveGPIO(INPUT_SEL, FUNCTION_SEL)) {
    handleOther(6);
    return;
  }
  else {
    // Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
    if (strcmp(WebServer->httpServer->arg(INPUT_MODE_HLW8012).c_str(), "") != 0) {
      Supla::GUI::counterHLW8012->setMode(WebServer->httpServer->arg(INPUT_MODE_HLW8012).toInt());
    }
    if (strcmp(WebServer->httpServer->arg(INPUT_COUNTER_CHANGE_VALUE_HLW8012).c_str(), "") != 0) {
      Supla::GUI::counterHLW8012->setCounter(WebServer->httpServer->arg(INPUT_COUNTER_CHANGE_VALUE_HLW8012).toFloat() * 100 * 1000);
    }
    Supla::Storage::ScheduleSave(1000);
  }
#endif

#ifdef SUPLA_CSE7766
  if (!WebServer->saveGPIO(INPUT_CSE7766_RX, FUNCTION_CSE7766_RX)) {
    handleOther(6);
    return;
  }
  else {
    Supla::GUI::addCSE7766(ConfigESP->getGpio(FUNCTION_CSE7766_RX));
    if (strcmp(WebServer->httpServer->arg(INPUT_COUNTER_CHANGE_VALUE_CSE7766).c_str(), "") != 0) {
      Supla::GUI::counterCSE7766->setCounter(WebServer->httpServer->arg(INPUT_COUNTER_CHANGE_VALUE_CSE7766).toFloat() * 100 * 1000);
      Supla::Storage::ScheduleSave(1000);
    }
  }
#endif

#ifdef SUPLA_PZEM_V_3
  for (nr = 1; nr <= 3; nr++) {
    if (!WebServer->saveGPIO(INPUT_PZEM_RX, FUNCTION_PZEM_RX, nr) || !WebServer->saveGPIO(INPUT_PZEM_TX, FUNCTION_PZEM_TX, nr)) {
      handleOther(6);
      return;
    }
  }
#endif

#ifdef SUPLA_HC_SR04
  if (!WebServer->saveGPIO(INPUT_TRIG_GPIO, FUNCTION_TRIG) || !WebServer->saveGPIO(INPUT_ECHO_GPIO, FUNCTION_ECHO)) {
    handleOther(6);
    return;
  }
  ConfigManager->set(KEY_HC_SR04_MAX_SENSOR_READ, WebServer->httpServer->arg(INPUT_HC_SR04_MAX_SENSOR_READ).c_str());
#endif

#ifdef SUPLA_RGBW
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    if (!WebServer->saveGPIO(INPUT_RGBW_RED, FUNCTION_RGBW_RED, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_GREEN, FUNCTION_RGBW_GREEN, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_BLUE, FUNCTION_RGBW_BLUE, nr, INPUT_RGBW_MAX) ||
        !WebServer->saveGPIO(INPUT_RGBW_BRIGHTNESS, FUNCTION_RGBW_BRIGHTNESS, nr, INPUT_RGBW_MAX)) {
      handleOther(6);
      return;
    }
  }
  ConfigManager->set(KEY_MAX_RGBW, WebServer->httpServer->arg(INPUT_RGBW_MAX).c_str());
#endif

#if defined(SUPLA_PUSHOVER)
  if (strcmp(WebServer->httpServer->arg(INPUT_PUSHOVER_TOKEN).c_str(), "") != 0) {
    ConfigManager->set(KEY_PUSHOVER_TOKEN, WebServer->httpServer->arg(INPUT_PUSHOVER_TOKEN).c_str());
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_PUSHOVER_USER).c_str(), "") != 0) {
    ConfigManager->set(KEY_PUSHOVER_USER, WebServer->httpServer->arg(INPUT_PUSHOVER_USER).c_str());
  }
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
  String input;

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR)->getValueInt(); nr++) {
    input = INPUT_DIRECT_LINKS_SENSOR_THERMOMETR;
    input = input + nr;
    ConfigManager->setElement(KEY_DIRECT_LINKS_SENSOR_THERMOMETR, nr, WebServer->httpServer->arg(input).c_str());
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DIRECT_LINKS_SENSOR_THERMOMETR).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DIRECT_LINKS_SENSOR_THERMOMETR, WebServer->httpServer->arg(INPUT_MAX_DIRECT_LINKS_SENSOR_THERMOMETR).c_str());
  }
#endif

#ifdef SUPLA_RF_BRIDGE
  if (!WebServer->saveGPIO(INPUT_RF_BRIDGE_TX, FUNCTION_RF_BRIDGE_TRANSMITTER)) {
    handleOther(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_RF_BRIDGE_RX, FUNCTION_RF_BRIDGE_RECEIVE)) {
    handleOther(6);
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleOther(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleOther(2);
      break;
  }
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
void handleImpulseCounterSet(int save) {
  String nr;
  uint8_t gpio, selected;

  nr = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  gpio = ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_OTHER);

  if (nr.toInt() <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt() && gpio != OFF_GPIO) {
    addForm(webContentBuffer, F("post"), getParameterRequest(PATH_IMPULSE_COUNTER_SET, ARG_PARM_NUMBER, nr));
    addFormHeader(webContentBuffer, S_IMPULSE_COUNTER_SETTINGS_NR + nr);

    selected = ConfigESP->getMemory(gpio);
    addCheckBox(webContentBuffer, INPUT_IMPULSE_COUNTER_PULL_UP + nr, S_IMPULSE_COUNTER_PULL_UP, selected);

    selected = ConfigESP->getLevel(gpio);
    addCheckBox(webContentBuffer, INPUT_IMPULSE_COUNTER_RAISING_EDGE + nr, S_IMPULSE_COUNTER_RAISING_EDGE, selected);

    addNumberBox(webContentBuffer, INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, S_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT);

    uint32_t count = Supla::GUI::impulseCounter[nr.toInt()]->getCounter();
    addNumberBox(webContentBuffer, INPUT_IMPULSE_COUNTER_CHANGE_VALUE, S_IMPULSE_COUNTER_CHANGE_VALUE, F(""), false, String(count));

    addFormHeaderEnd(webContentBuffer);
    addButtonSubmit(webContentBuffer, S_SAVE);
    addFormEnd(webContentBuffer);
  }

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}

void handleImpulseCounterSaveSet() {
  String nr, input;

  nr = WebServer->httpServer->arg(ARG_PARM_NUMBER);
  uint8_t key = KEY_GPIO + ConfigESP->getGpio(nr.toInt(), FUNCTION_IMPULSE_COUNTER);

  input = INPUT_IMPULSE_COUNTER_PULL_UP;
  input += nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, MEMORY, 1);
  }
  else {
    ConfigManager->setElement(key, MEMORY, 0);
  }

  input = INPUT_IMPULSE_COUNTER_RAISING_EDGE;
  input += nr;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(key, LEVEL_RELAY, 1);
  }
  else {
    ConfigManager->setElement(key, LEVEL_RELAY, 0);
  }

  ConfigManager->set(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT).c_str());
  Supla::GUI::impulseCounter[nr.toInt()]->setCounter((unsigned long long)WebServer->httpServer->arg(INPUT_IMPULSE_COUNTER_CHANGE_VALUE).toInt());
  Supla::Storage::ScheduleSave(1000);

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      //      Serial.println(F("E_CONFIG_OK: Dane zapisane"));
      handleOther(1);
      break;

    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      handleOther(2);
      break;
  }
}
#endif

#if defined(SUPLA_HLW8012) || defined(SUPLA_CSE7766)
void handleCounterCalibrate(int save) {
  String couter;
  double curent = 0, voltage = 0, power = 0;

  couter = WebServer->httpServer->arg(ARG_PARM_URL);

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(getParameterRequest(PATH_CALIBRATE, ARG_PARM_URL, couter));

#ifdef SUPLA_HLW8012
  if (couter == PATH_HLW8012) {
    curent = Supla::GUI::counterHLW8012->getCurrentMultiplier();
    voltage = Supla::GUI::counterHLW8012->getVoltageMultiplier();
    power = Supla::GUI::counterHLW8012->getPowerMultiplier();
  }
#endif

#ifdef SUPLA_CSE7766
  if (couter == PATH_CSE7766) {
    curent = Supla::GUI::counterCSE7766->getCurrentMultiplier();
    voltage = Supla::GUI::counterCSE7766->getVoltageMultiplier();
    power = Supla::GUI::counterCSE7766->getPowerMultiplier();
  }
#endif

  addFormHeader(webContentBuffer);
  webContentBuffer += F("<p style='color:#000;'>Current Multi: ");
  webContentBuffer += curent;
  webContentBuffer += F("<br>Voltage Multi: ");
  webContentBuffer += voltage;
  webContentBuffer += F("<br>Power Multi: ");
  webContentBuffer += power;
  webContentBuffer += F("</p>");
  addFormHeaderEnd(webContentBuffer);

  addForm(webContentBuffer, F("post"), getParameterRequest(PATH_CALIBRATE, ARG_PARM_URL, couter));
  addFormHeader(webContentBuffer, S_CALIBRATION_SETTINGS);
  addNumberBox(webContentBuffer, INPUT_CALIB_POWER, S_BULB_POWER_W, F("25"), true);
  addNumberBox(webContentBuffer, INPUT_CALIB_VOLTAGE, S_VOLTAGE_V, F("230"), true);
  addFormHeaderEnd(webContentBuffer);

  addButtonSubmit(webContentBuffer, S_CALIBRATION);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}

void handleCounterCalibrateSave() {
  double calibPower = 0, calibVoltage = 0;
  String couter = WebServer->httpServer->arg(ARG_PARM_URL);

  String input = INPUT_CALIB_POWER;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    calibPower = WebServer->httpServer->arg(input).toDouble();
  }

  input = INPUT_CALIB_VOLTAGE;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    calibVoltage = WebServer->httpServer->arg(input).toDouble();
  }

  if (calibPower != 0 && calibVoltage != 0) {
#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
    for (size_t i = 0; i < Supla::GUI::relay.size(); i++) {
      Supla::GUI::relay[i]->turnOn();
    }
#endif

#ifdef SUPLA_HLW8012
    if (couter == PATH_HLW8012)
      Supla::GUI::counterHLW8012->calibrate(calibPower, calibVoltage);
#endif

#ifdef SUPLA_CSE7766
    if (couter == PATH_CSE7766)
      Supla::GUI::counterCSE7766->calibrate(calibPower, calibVoltage);
#endif
    handleCounterCalibrate(1);
  }
  else {
    handleCounterCalibrate(6);
  }
}
#endif

#ifdef SUPLA_RF_BRIDGE
#include <RCSwitch.h>

void receiveCodeRFBridge() {
  String code;

  if (WebServer->httpServer->arg(ARG_PARM_URL) == "read") {
    RCSwitch* mySwitch = new RCSwitch();
    mySwitch->enableReceive(ConfigESP->getGpio(FUNCTION_RF_BRIDGE_RECEIVE));

    unsigned long timeout = millis();
    while ((millis() - timeout) < 5000) {
      if (mySwitch->available()) {
        code += "Received ";
        code += mySwitch->getReceivedValue();
        code += " Length: ";
        code += mySwitch->getReceivedBitlength();
        code += "bit ";
        code += "Protocol: ";
        code += mySwitch->getReceivedProtocol();
        code += " Pulse Length: ";
        code += mySwitch->getReceivedDelay();
        code += "<br>";

        mySwitch->resetAvailable();
      }
      delay(0);
    }

    delete mySwitch;
  }

  addFormHeader(webContentBuffer, String(S_SETTING_FOR) + S_SPACE + S_CODES);
  webContentBuffer += F("<p style='color:#000;'>");
  if (!code.isEmpty()) {
    webContentBuffer += code;
  }
  else {
    webContentBuffer += "<br>";
    webContentBuffer += String(S_NO) + S_SPACE + S_CODES;
    webContentBuffer += "<br>";
  }
  webContentBuffer += F("</p>");
  addButton(webContentBuffer, S_READ, getParameterRequest(PATH_BRIDGE, ARG_PARM_URL, "read"));
  addFormHeaderEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_OTHER);
  WebServer->sendContent();
}
#endif