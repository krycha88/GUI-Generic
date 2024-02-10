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

#include "SuplaWebPageRelay.h"

#ifdef GUI_RELAY
void createWebPageRelay() {
  WebServer->httpServer->on(getURL(PATH_RELAY), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleRelay();
    else
      handleRelaySave();
  });

  WebServer->httpServer->on(getURL(PATH_RELAY_SET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }
#ifdef GUI_SENSOR_I2C_EXPENDER
    if (Expander->checkActiveExpander(FUNCTION_RELAY)) {
      if (WebServer->httpServer->method() == HTTP_GET)
        handleRelaySetMCP23017();
      else
        handleRelaySaveSetMCP23017();
    }
    else {
#ifdef SUPLA_RELAY
      if (WebServer->httpServer->method() == HTTP_GET)
        handleRelaySet();
      else
        handleRelaySaveSet();
#endif
    }
#else
#ifdef SUPLA_RELAY
    if (WebServer->httpServer->method() == HTTP_GET)
      handleRelaySet();
    else
      handleRelaySaveSet();
#endif
#endif
  });
}

void handleRelaySave() {
  uint8_t nr;

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
#ifdef GUI_SENSOR_I2C_EXPENDER
    if (Expander->checkActiveExpander(FUNCTION_RELAY)) {
      if (!WebServer->saveGpioMCP23017(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        handleRelay(6);
        return;
      }
    }
    else {
      if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
        handleRelay(6);
        return;
      }
    }
#else
    if (!WebServer->saveGPIO(INPUT_RELAY_GPIO, FUNCTION_RELAY, nr, INPUT_MAX_RELAY)) {
      handleRelay(6);
      return;
    }
#endif
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_RELAY, WebServer->httpServer->arg(INPUT_MAX_RELAY).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleRelay(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelay(2);
      break;
  }
}

void handleRelay(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();

  SuplaSaveResult(save);
  SuplaJavaScript(PATH_RELAY);

  addForm(F("post"), PATH_RELAY);
  addFormHeader(S_GPIO_SETTINGS_FOR_RELAYS);
  addNumberBox(INPUT_MAX_RELAY, S_QUANTITY, KEY_MAX_RELAY, ConfigESP->countFreeGpio(FUNCTION_RELAY));

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
#ifdef GUI_SENSOR_I2C_EXPENDER
    addListExpanderBox(INPUT_RELAY_GPIO, S_RELAY, FUNCTION_RELAY, nr, PATH_RELAY_SET);
#else
    addListGPIOLinkBox(INPUT_RELAY_GPIO, S_RELAY, getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER), FUNCTION_RELAY, nr);
#endif
  }
  addFormHeaderEnd();

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}
#endif

#if defined(SUPLA_RELAY)
void handleRelaySaveSet() {
  String input, nr_relay;
  uint8_t gpio;

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);

  input = INPUT_RELAY_MEMORY;
  input += nr_relay;
  ConfigESP->setMemory(gpio, WebServer->httpServer->arg(input).toInt(), nr_relay.toInt());

  if (gpio != GPIO_VIRTUAL_RELAY) {
    input = INPUT_RELAY_LEVEL;
    input += nr_relay;
    ConfigESP->setLevel(gpio, static_cast<int>(WebServer->httpServer->arg(input).toInt()));

    input = INPUT_LIGHT_RELAY;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigESP->setLightRelay(gpio, 1);
    }
    else {
      ConfigESP->setLightRelay(gpio, 0);
    }
  }

#if defined(SUPLA_LED)
  if (gpio != GPIO_VIRTUAL_RELAY) {
    input = INPUT_LED;
    input += nr_relay;
    if (!WebServer->saveGPIO(input, FUNCTION_LED, nr_relay.toInt())) {
      handleRelaySet(6);
      return;
    }
    else {
      input = INPUT_LEVEL_LED;
      input += nr_relay;

      ConfigESP->setLevel(ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_LED), static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    }
  }
#endif

#if defined(SUPLA_DIRECT_LINKS)
  directLinksWebPageSave(nr_relay.toInt());
#endif

#ifdef SUPLA_RF_BRIDGE
  if (nr_relay.toInt() < MAX_BRIDGE_RF) {
    String input;

    input = INPUT_RF_BRIDGE_TYPE;
    ConfigManager->setElement(KEY_RF_BRIDGE_TYPE, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());
    input = INPUT_RF_BRIDGE_PROTOCO;
    ConfigManager->setElement(KEY_RF_BRIDGE_PROTOCOL, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());
    input = INPUT_RF_BRIDGE_PULSE_LENGTHIN;
    ConfigManager->setElement(KEY_RF_BRIDGE_PULSE_LENGTHINT, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());
    input = INPUT_RF_BRIDGE_LENGTH;
    ConfigManager->setElement(KEY_RF_BRIDGE_LENGTH, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());

    input = INPUT_RF_BRIDGE_REPEAT;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_RF_BRIDGE_REPEAT, nr_relay.toInt(), 1);
    }
    else {
      ConfigManager->setElement(KEY_RF_BRIDGE_REPEAT, nr_relay.toInt(), 0);
    }

    input = INPUT_RF_BRIDGE_CODE_ON;
    ConfigManager->setElement(KEY_RF_BRIDGE_CODE_ON, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());
    input = INPUT_RF_BRIDGE_CODE_OFF;
    ConfigManager->setElement(KEY_RF_BRIDGE_CODE_OFF, nr_relay.toInt(), WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_THERMOSTAT
  auto thermostatIndex = nr_relay.toInt();
  uint8_t oldThermostatType = ConfigManager->get(KEY_THERMOSTAT_TYPE)->getElement(thermostatIndex).toInt();
  input = INPUT_THERMOSTAT_TYPE;
  uint8_t newThermostatType = WebServer->httpServer->arg(input).toInt();
  ConfigManager->setElement(KEY_THERMOSTAT_TYPE, thermostatIndex, newThermostatType);

  if (thermostatIndex >= 0 && thermostatIndex < MAX_THERMOSTAT) {
    if (oldThermostatType == Supla::GUI::THERMOSTAT_OFF && newThermostatType != Supla::GUI::THERMOSTAT_OFF) {
      ConfigManager->setElement(KEY_THERMOSTAT_HISTERESIS, thermostatIndex, THERMOSTAT_DEFAULT_HISTERESIS);
      ConfigManager->setElement(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, thermostatIndex, THERMOSTAT_NO_TEMP_CHANNEL);
      ConfigManager->setElement(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL, thermostatIndex, THERMOSTAT_NO_TEMP_CHANNEL);

      switch (newThermostatType) {
        case Supla::GUI::THERMOSTAT_DOMESTIC_HOT_WATER:
          ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MIN, thermostatIndex, 5);
          ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MAX, thermostatIndex, 75);
          break;
        default:
          ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MIN, thermostatIndex, 5);
          ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MAX, thermostatIndex, 40);
          break;
      }
    }
    else if (newThermostatType != Supla::GUI::THERMOSTAT_OFF) {
      auto thermostat = Supla::GUI::thermostatArray[thermostatIndex];
      if (thermostat) {
        thermostat->setThermostatType(newThermostatType);
      }

      input = INPUT_THERMOSTAT_HISTERESIS;
      String histeresis = WebServer->httpServer->arg(input).c_str();
      ConfigManager->setElement(KEY_THERMOSTAT_HISTERESIS, thermostatIndex, histeresis.c_str());
      if (thermostat) {
        thermostat->setTemperatureHisteresis(static_cast<short>(histeresis.toDouble() * 100.0));
      }

      input = INPUT_THERMOSTAT_MAIN_THERMOMETER_CHANNEL;
      uint8_t thermomeetrChannel = WebServer->httpServer->arg(input).toInt();
      ConfigManager->setElement(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, thermostatIndex, thermomeetrChannel);
      if (thermostat) {
        thermostat->setMainThermometerChannelNo(thermomeetrChannel);
      }

      input = INPUT_THERMOSTAT_AUX_THERMOMETER_CHANNEL;
      thermomeetrChannel = WebServer->httpServer->arg(input).toInt();
      ConfigManager->setElement(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL, thermostatIndex, thermomeetrChannel);
      if (thermostat) {
        thermostat->setAuxThermometerChannelNo(thermomeetrChannel);
      }

      input = INPUT_THERMOSTAT_TEMPERATURE_MIN;
      thermomeetrChannel = WebServer->httpServer->arg(input).toInt();
      ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MIN, thermostatIndex, thermomeetrChannel);
      if (thermostat) {
        thermostat->setDefaultTemperatureRoomMin(SUPLA_CHANNELFNC_HVAC_THERMOSTAT, thermomeetrChannel * 100);
        //   thermostat->setDefaultTemperatureRoomMin(SUPLA_CHANNELFNC_HVAC_THERMOSTAT_AUTO, thermomeetrChannel * 100);
      }

      input = INPUT_THERMOSTAT_TEMPERATURE_MAX;
      thermomeetrChannel = WebServer->httpServer->arg(input).toInt();
      ConfigManager->setElement(KEY_THERMOSTAT_TEMPERATURE_MAX, thermostatIndex, thermomeetrChannel);
      if (thermostat) {
        thermostat->setDefaultTemperatureRoomMax(SUPLA_CHANNELFNC_HVAC_THERMOSTAT, thermomeetrChannel * 100);
        // thermostat->setDefaultTemperatureRoomMax(SUPLA_CHANNELFNC_HVAC_THERMOSTAT_AUTO, thermomeetrChannel * 100);
      }
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:

#ifdef SUPLA_THERMOSTAT
      if (oldThermostatType == Supla::GUI::THERMOSTAT_OFF && Supla::GUI::thermostatArray[thermostatIndex] == nullptr &&
          ConfigESP->configModeESP != Supla::DEVICE_MODE_CONFIG) {
        handleRelaySet(SaveResult::DATA_SAVED_RESTART_MODULE);
        ConfigESP->rebootESP();
      }
#endif

      handleRelaySet(SaveResult::DATA_SAVE);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelaySet(SaveResult::DATA_SAVED_RESTART_MODULE);
      break;
  }
}

void handleRelaySet(int save) {
  uint8_t gpio, selected;
  String nr_relay, massage, input;

  massage.reserve(MAX_MESSAGE_SIZE);
  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  WebServer->sendHeaderStart();

  if (!nr_relay.isEmpty()) {
    SuplaSaveResult(save);
    SuplaJavaScript(getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

    gpio = ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_RELAY);

    addForm(F("post"), getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));
    addFormHeader(String(S_RELAY_NR_SETTINGS) + (nr_relay.toInt() + 1));

    if (gpio != GPIO_VIRTUAL_RELAY) {
      selected = ConfigESP->getLevel(gpio);
      input = INPUT_RELAY_LEVEL;
      input += nr_relay;
      addListBox(INPUT_RELAY_LEVEL + nr_relay, S_STATE_CONTROL, LEVEL_P, 2, selected);

      input = INPUT_LIGHT_RELAY;
      selected = ConfigESP->getLightRelay(gpio);
      addCheckBox(input, S_LIGHT_RELAY, selected);
    }

    selected = ConfigESP->getMemory(gpio, nr_relay.toInt());
    input = INPUT_RELAY_MEMORY;
    input += nr_relay;
    addListBox(input, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);

    addFormHeaderEnd();

#ifdef SUPLA_RF_BRIDGE
    if (nr_relay.toInt() < MAX_BRIDGE_RF) {
      String value;

      addFormHeader(F("RF BRIDGE"));

      selected = ConfigManager->get(KEY_RF_BRIDGE_TYPE)->getElement(nr_relay.toInt()).toInt();
      addListBox(INPUT_RF_BRIDGE_TYPE, S_TYPE, RF_BRIDGE_TYPE_P, 2, selected);

      if (ConfigManager->get(KEY_RF_BRIDGE_TYPE)->getElement(nr_relay.toInt()).toInt() == Supla::GUI::RFBridgeType::TRANSMITTER) {
        value = ConfigManager->get(KEY_RF_BRIDGE_PROTOCOL)->getElement(nr_relay.toInt()).c_str();
        addTextBox(INPUT_RF_BRIDGE_PROTOCO, F("PROTOCO"), value, F("1..."), 0, 2, true);

        value = ConfigManager->get(KEY_RF_BRIDGE_PULSE_LENGTHINT)->getElement(nr_relay.toInt()).c_str();
        addTextBox(INPUT_RF_BRIDGE_PULSE_LENGTHIN, F("PULSE LENGTHINT"), value, F("320..."), 0, 4, true);

        value = ConfigManager->get(KEY_RF_BRIDGE_LENGTH)->getElement(nr_relay.toInt()).c_str();
        addTextBox(INPUT_RF_BRIDGE_LENGTH, F("LENGTH"), value, F("24..."), 0, 3, true);

        selected = ConfigManager->get(KEY_RF_BRIDGE_REPEAT)->getElement(nr_relay.toInt()).toInt();
        addCheckBox(INPUT_RF_BRIDGE_REPEAT, F("Powtarzaj[10min]"), selected);
      }

      value = ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr_relay.toInt()).c_str();
      addTextBox(INPUT_RF_BRIDGE_CODE_ON, S_ON, value, F(""), 0, 10, false);

      value = ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr_relay.toInt()).c_str();
      addTextBox(INPUT_RF_BRIDGE_CODE_OFF, S_OFF, value, F(""), 0, 10, false);

      // this->addKey(KEY_RF_BRIDGE_LENGTH, MAX_BRIDGE_RF * 3, 4);
      ///////// // this->addKey(KEY_RF_BRIDGE_TYPE, MAX_BRIDGE_RF * 2, 4);
      // this->addKey(KEY_RF_BRIDGE_PROTOCOL, MAX_BRIDGE_RF * 3, 4);
      // this->addKey(KEY_RF_BRIDGE_PULSE_LENGTHINT, MAX_BRIDGE_RF * 4, 4);

      addFormHeaderEnd();
    }
#endif

#if defined(SUPLA_LED)
    if (gpio != GPIO_VIRTUAL_RELAY) {
      addFormHeader(S_RELAY_ACTIVATION_STATUS);

      addListGPIOBox(INPUT_LED + nr_relay, S_LED, FUNCTION_LED, nr_relay.toInt());

      selected = ConfigESP->getLevel(ConfigESP->getGpio(nr_relay.toInt(), FUNCTION_LED));
      addListBox(INPUT_LEVEL_LED + nr_relay, S_STATE_CONTROL, LEVEL_P, 2, selected);

      addFormHeaderEnd();
    }
#endif

#ifdef SUPLA_THERMOSTAT
    auto thermostatIndex = nr_relay.toInt();

    if (thermostatIndex >= 0 && thermostatIndex < MAX_THERMOSTAT) {
      addFormHeader(S_THERMOSTAT);
      selected = ConfigManager->get(KEY_THERMOSTAT_TYPE)->getElement(thermostatIndex).toInt();
      addListBox(INPUT_THERMOSTAT_TYPE, S_TYPE, THERMOSTAT_TYPE_P, COUNT_ELEMENTS_PGM(THERMOSTAT_TYPE_P), selected);

      if (selected != Supla::GUI::THERMOSTAT_OFF) {
        if (thermostatIndex >= 0) {
          selected = ConfigManager->get(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL)->getElement(thermostatIndex).toInt();
          addListNumbersSensorBox(INPUT_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, S_MAIN_THERMOMETER_CHANNEL, selected);

          selected = ConfigManager->get(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL)->getElement(thermostatIndex).toInt();
          addListNumbersSensorBox(INPUT_THERMOSTAT_AUX_THERMOMETER_CHANNEL, S_AUX_THERMOMETER_CHANNEL, selected);

          String value = ConfigManager->get(KEY_THERMOSTAT_HISTERESIS)->getElement(thermostatIndex).c_str();
          addNumberBox(INPUT_THERMOSTAT_HISTERESIS, S_HISTERESIS, S_CELSIUS, false, value, true);

          value = ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MIN)->getElement(thermostatIndex).c_str();
          addNumberBox(INPUT_THERMOSTAT_TEMPERATURE_MIN, "MIN", S_CELSIUS, false, value, true);

          value = ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MAX)->getElement(thermostatIndex).c_str();
          addNumberBox(INPUT_THERMOSTAT_TEMPERATURE_MAX, "MAX", S_CELSIUS, false, value, true);
        }
      }
      addFormHeaderEnd();
    }
#endif

#if defined(SUPLA_DIRECT_LINKS)
    directLinksWebPage(nr_relay.toInt());
#endif

    addButtonSubmit(S_SAVE);
    addFormEnd();
  }
  addButton(S_RETURN, PATH_RELAY);

  WebServer->sendHeaderEnd();
}
#endif

#ifdef GUI_SENSOR_I2C_EXPENDER
void handleRelaySetMCP23017(int save) {
  uint8_t gpio, selected;
  String massage, input, nr_relay;

  massage.reserve(MAX_MESSAGE_SIZE);
  input.reserve(9);
  nr_relay.reserve(2);

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_relay.isEmpty())
    gpio = Expander->getGpioExpander(nr_relay.toInt(), FUNCTION_RELAY);
  else
    gpio = Expander->getGpioExpander(0, FUNCTION_RELAY);

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

  addForm(F("post"), getParameterRequest(PATH_RELAY_SET, ARG_PARM_NUMBER, nr_relay));

  if (!nr_relay.isEmpty()) {
    addFormHeader(String(S_RELAY_NR_SETTINGS) + (nr_relay.toInt() + 1));
  }
  else {
    addFormHeader(S_SETTINGS_FOR_RELAYS);
  }

  selected = ConfigESP->getLevel(gpio);
  input = INPUT_RELAY_LEVEL;
  addListBox(input, S_STATE_CONTROL, LEVEL_P, 2, selected);

  selected = ConfigESP->getMemory(gpio);
  input = INPUT_RELAY_MEMORY;
  addListBox(input, S_REACTION_AFTER_RESET, MEMORY_P, 3, selected);
  addFormHeaderEnd();

  if (!nr_relay.isEmpty()) {
#if defined(SUPLA_DIRECT_LINKS)
    directLinksWebPage(nr_relay.toInt());
#endif
  }

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_RELAY);

  WebServer->sendHeaderEnd();
}

void handleRelaySaveSetMCP23017() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  String input, nr_relay;
  uint8_t key, gpio, memory, level;

  input.reserve(9);
  nr_relay.reserve(2);

  input = INPUT_RELAY_MEMORY;
  memory = WebServer->httpServer->arg(input).toInt();

  input = INPUT_RELAY_LEVEL;
  level = WebServer->httpServer->arg(input).toInt();

  nr_relay = WebServer->httpServer->arg(ARG_PARM_NUMBER);

  if (!nr_relay.isEmpty()) {
    gpio = Expander->getGpioExpander(nr_relay.toInt(), FUNCTION_RELAY);
    key = KEY_GPIO + gpio;

    ConfigManager->setElement(key, MEMORY, memory);
    ConfigManager->setElement(key, LEVEL_RELAY, level);

#if defined(SUPLA_DIRECT_LINKS)
    directLinksWebPageSave(nr_relay.toInt());
#endif
  }
  else {
    for (gpio = 0; gpio <= OFF_GPIO; gpio++) {
      key = KEY_GPIO + gpio;
      ConfigManager->setElement(key, MEMORY, memory);
      ConfigManager->setElement(key, LEVEL_RELAY, level);
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleRelaySetMCP23017(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleRelaySetMCP23017(2);
      break;
  }
}
#endif

#ifdef SUPLA_DIRECT_LINKS
void directLinksWebPage(int nr) {
  if (nr <= MAX_DIRECT_LINK) {
    addFormHeader(S_DIRECT_LINKS);
    String massage = ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str();
    addTextBox(INPUT_DIRECT_LINK_ON, S_ON, massage, F("xx/xxxxxxxxx/turn-on"), 0, MAX_DIRECT_LINKS_SIZE, false);

    massage = ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str();
    addTextBox(INPUT_DIRECT_LINK_OFF, S_OFF, massage, F("xx/xxxxxxxxx/turn-off"), 0, MAX_DIRECT_LINKS_SIZE, false);

    addFormHeaderEnd();
  }
}

void directLinksWebPageSave(int nr) {
  if (nr <= MAX_DIRECT_LINK) {
    String input = INPUT_DIRECT_LINK_ON;
    ConfigManager->setElement(KEY_DIRECT_LINKS_ON, nr, WebServer->httpServer->arg(input).c_str());
    input = INPUT_DIRECT_LINK_OFF;
    ConfigManager->setElement(KEY_DIRECT_LINKS_OFF, nr, WebServer->httpServer->arg(input).c_str());
  }
}
#endif