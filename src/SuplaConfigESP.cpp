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
#include "SuplaConfigESP.h"
#include "SuplaDeviceGUI.h"

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = NORMAL_MODE;

  if (ConfigManager->isDeviceConfigured()) {
    if (strcmp(ConfigManager->get(KEY_SUPLA_GUID)->getValue(), "") == 0 || strcmp(ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue(), "") == 0) {
      clearEEPROM();
      ConfigManager->setGUIDandAUTHKEY();
    }

    if (strcmp(ConfigManager->get(KEY_LOGIN)->getValue(), "") == 0)
      ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);

    if (strcmp(ConfigManager->get(KEY_LOGIN_PASS)->getValue(), "") == 0)
      ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);

    if (strcmp(ConfigManager->get(KEY_HOST_NAME)->getValue(), "") == 0)
      ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);

    if (strcmp(ConfigManager->get(KEY_SUPLA_SERVER)->getValue(), "") == 0)
      ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);

    if (strcmp(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(), "") == 0)
      ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);

    if (strcmp(ConfigManager->get(KEY_ENABLE_GUI)->getValue(), "") == 0)
      ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());

    if (strcmp(ConfigManager->get(KEY_ENABLE_SSL)->getValue(), "") == 0)
      ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());

    if (strcmp(ConfigManager->get(KEY_BOARD)->getValue(), "") == 0)
      saveChooseTemplateBoard(getDefaultTamplateBoard());

    if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO)
      ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    configModeInit();
  }

  SuplaDevice.setStatusFuncImpl(&status_func);
}

bool SuplaConfigESP::getDefaultEnableSSL() {
#ifdef SUPLA_ENABLE_SSL
  return true;
#else
  return false;
#endif
}

bool SuplaConfigESP::getDefaultEnableGUI() {
#ifdef SUPLA_ENABLE_GUI
  return true;
#else
  return false;
#endif
}

uint8_t SuplaConfigESP::getDefaultTamplateBoard() {
#ifdef DEFAULT_TEMPLATE_BOARD
  return DEFAULT_TEMPLATE_BOARD;
#else
  return 0;
#endif
}

void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig) {
  uint8_t pinNumberConfig = _pinNumberConfig;
  uint8_t pinLedConfig = _pinLedConfig;
  uint8_t modeConfigButton = ConfigManager->get(KEY_CFG_MODE)->getValueInt();

  if (pinLedConfig != OFF_GPIO) {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, ConfigESP->getLevel(pinLedConfig) ? LOW : HIGH);
  }

  if (pinNumberConfig != OFF_GPIO) {
    bool pullUp = true, invertLogic = true;

    if (ConfigESP->getGpio(FUNCTION_BUTTON) != OFF_GPIO) {
      pullUp = ConfigESP->getPullUp(ConfigESP->getGpio(FUNCTION_BUTTON));
      invertLogic = ConfigESP->getInversed(ConfigESP->getGpio(FUNCTION_BUTTON));
    }

    Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, pullUp, invertLogic);
    buttonConfig->setMulticlickTime(800);
    buttonConfig->addAction(Supla::TURN_ON, *ConfigESP, Supla::ON_CLICK_1);

    if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
      buttonConfig->addAction(CONFIG_MODE_10_ON_PRESSES, *ConfigESP, Supla::ON_CLICK_10);
    }
    if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
      buttonConfig->setHoldTime(5000);
      buttonConfig->addAction(CONFIG_MODE_5SEK_HOLD, *ConfigESP, Supla::ON_HOLD);
    }
  }
}

void SuplaConfigESP::handleAction(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (event == Supla::ON_CLICK_10) {
      configModeInit();
    }
  }
  if (action == CONFIG_MODE_5SEK_HOLD) {
    if (event == Supla::ON_HOLD) {
      configModeInit();
    }
  }

  if (configModeESP == CONFIG_MODE) {
    if (event == Supla::ON_CLICK_1) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::rebootESP() {
  WiFi.disconnect(true);
  ESP.restart();
}

void SuplaConfigESP::configModeInit() {
  configModeESP = CONFIG_MODE;

  ledBlinking(100);

  Supla::GUI::enableWifiSSL(false);

  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(getConfigNameAP().c_str(), "");
  WiFi.begin(ConfigManager->get(KEY_WIFI_SSID)->getValue(), ConfigManager->get(KEY_WIFI_PASS)->getValue());
  Serial.println(F("Config Mode started"));
}

bool SuplaConfigESP::checkSSL() {
  return ConfigManager->get(KEY_ENABLE_SSL)->getValueInt();
}

void SuplaConfigESP::iterateAlways() {
  if (configModeESP == CONFIG_MODE) {
    if (!APConfigured) {
      APConfigured = WiFi.softAP(getConfigNameAP().c_str(), "");
      Serial.print(F("AP started IP: "));
      Serial.println(WiFi.softAPIP());
      Supla::GUI::crateWebServer();
    }

#ifdef SUPLA_MDNS
    if (WiFi.status() == WL_CONNECTED) {
      if (!MDNSConfigured) {
#ifdef ARDUINO_ARCH_ESP8266
        MDNSConfigured = MDNS.begin("supla", WiFi.localIP());
#elif ARDUINO_ARCH_ESP32
        MDNSConfigured = MDNS.begin("supla");
#endif
        if (MDNSConfigured) {
          Serial.print(F("MDNS started IP: "));
          Serial.println(WiFi.localIP());
          MDNS.addService("http", "tcp", 80);
        }
      }
#ifdef ARDUINO_ARCH_ESP8266
      MDNS.update();
#endif
    }
#endif
  }
}

const String SuplaConfigESP::getConfigNameAP() {
  String name = F("SUPLA-ESP8266-");
  return name += getMacAddress(false);
}
const char *SuplaConfigESP::getLastStatusMessageSupla() {
  return supla_status.msg;
}

int SuplaConfigESP::getLastStatusSupla() {
  return supla_status.status;
}

void SuplaConfigESP::ledBlinking(int time) {
  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO)
    led.attach_ms(time, ledBlinkingTicker);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO) {
    led.detach();
    digitalWrite(ConfigESP->getGpio(FUNCTION_CFG_LED), ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)) ? LOW : HIGH);
  }
}

String SuplaConfigESP::getMacAddress(bool formating) {
  byte mac[6];
  WiFi.macAddress(mac);
  char baseMacChr[18] = {0};

  if (formating)
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  else
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return String(baseMacChr);
}

void ledBlinkingTicker() {
  int val = digitalRead(ConfigESP->getGpio(FUNCTION_CFG_LED));
  digitalWrite(ConfigESP->getGpio(FUNCTION_CFG_LED), val == HIGH ? LOW : HIGH);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case STATUS_ALREADY_INITIALIZED:
      ConfigESP->supla_status.msg = S_STATUS_ALREADY_INITIALIZED;
      break;
    case STATUS_INVALID_GUID:
      ConfigESP->supla_status.msg = S_STATUS_INVALID_GUID;
      break;
    case STATUS_UNKNOWN_SERVER_ADDRESS:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_SERVER_ADDRESS;
      break;
    case STATUS_UNKNOWN_LOCATION_ID:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_LOCATION_ID;
      break;
    case STATUS_INITIALIZED:
      ConfigESP->supla_status.msg = S_STATUS_INITIALIZED;
      break;
    case STATUS_CHANNEL_LIMIT_EXCEEDED:
      ConfigESP->supla_status.msg = S_STATUS_CHANNEL_LIMIT_EXCEEDED;
      break;
    case STATUS_SERVER_DISCONNECTED:
      ConfigESP->supla_status.msg = S_STATUS_SERVER_DISCONNECTED;
      break;
    case STATUS_REGISTER_IN_PROGRESS:
      ConfigESP->supla_status.msg = S_STATUS_REGISTER_IN_PROGRESS;
      break;
    case STATUS_PROTOCOL_VERSION_ERROR:
      ConfigESP->supla_status.msg = S_STATUS_PROTOCOL_VERSION_ERROR;
      break;
    case STATUS_BAD_CREDENTIALS:
      ConfigESP->supla_status.msg = S_STATUS_BAD_CREDENTIALS;
      break;
    case STATUS_TEMPORARILY_UNAVAILABLE:
      ConfigESP->supla_status.msg = S_STATUS_TEMPORARILY_UNAVAILABLE;
      break;
    case STATUS_LOCATION_CONFLICT:
      ConfigESP->supla_status.msg = S_STATUS_LOCATION_CONFLICT;
      break;
    case STATUS_CHANNEL_CONFLICT:
      ConfigESP->supla_status.msg = S_STATUS_CHANNEL_CONFLICT;
      break;
    case STATUS_REGISTERED_AND_READY:
      ConfigESP->supla_status.msg = S_STATUS_REGISTERED_AND_READY;
      break;
    case STATUS_DEVICE_IS_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_DEVICE_IS_DISABLED;
      break;
    case STATUS_LOCATION_IS_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_LOCATION_IS_DISABLED;
      break;
    case STATUS_DEVICE_LIMIT_EXCEEDED:
      ConfigESP->supla_status.msg = S_STATUS_DEVICE_LIMIT_EXCEEDED;
      break;
    case STATUS_REGISTRATION_DISABLED:
      ConfigESP->supla_status.msg = S_STATUS_REGISTRATION_DISABLED;
      break;
    case STATUS_MISSING_CREDENTIALS:
      ConfigESP->supla_status.msg = S_STATUS_MISSING_CREDENTIALS;
      break;
    case STATUS_INVALID_AUTHKEY:
      ConfigESP->supla_status.msg = S_STATUS_INVALID_AUTHKEY;
      break;
    case STATUS_NO_LOCATION_AVAILABLE:
      ConfigESP->supla_status.msg = S_STATUS_NO_LOCATION_AVAILABLE;
      break;
    case STATUS_UNKNOWN_ERROR:
      ConfigESP->supla_status.msg = S_STATUS_UNKNOWN_ERROR;
      break;
    case STATUS_NETWORK_DISCONNECTED:
      ConfigESP->supla_status.msg = S_STATUS_NETWORK_DISCONNECTED;
      break;
    default:
      ConfigESP->supla_status.msg = msg;
      break;
  }
  ConfigESP->supla_status.status = status;

  static int lock;
  if (status == STATUS_REGISTERED_AND_READY && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinkingStop();
    lock = 0;
  }
  else if (status != STATUS_REGISTERED_AND_READY && lock == 0 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinking(500);
    lock = 1;
  }
}

int SuplaConfigESP::getGpio(int nr, int function) {
  if (function == FUNCTION_RELAY && ConfigManager->get(KEY_VIRTUAL_RELAY)->getElement(nr).toInt()) {
    return GPIO_VIRTUAL_RELAY;
  }

  nr++;
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;
    if (function == FUNCTION_CFG_BUTTON) {
      if (checkBusyCfg(gpio)) {
        return gpio;
      }
    }
    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      if (ConfigManager->get(key)->getElement(NR).toInt() == nr) {
        return gpio;
      }
    }
    // return OFF_GPIO;
    //"Pin 100 - 115"
    // Pin 116 - 131"
#ifdef SUPLA_MCP23017
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF &&
        ((function == FUNCTION_RELAY) || (function == FUNCTION_BUTTON) || (function == FUNCTION_LIMIT_SWITCH))) {
      switch (getAdressMCP23017(nr, function)) {
        case 0:
          if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_1).toInt() == function &&
              ConfigManager->get(key)->getElement(MCP23017_NR_1).toInt() == nr) {
            return gpio + 100;
          }
          break;
        case 1:
          if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_2).toInt() == function &&
              ConfigManager->get(key)->getElement(MCP23017_NR_2).toInt() == nr) {
            return gpio + 100 + 16;
          }
          break;
        case 2:
          if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_3).toInt() == function &&
              ConfigManager->get(key)->getElement(MCP23017_NR_3).toInt() == nr) {
            return gpio + 100 + 16 + 16;
          }
          break;
        case 3:
          if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_4).toInt() == function &&
              ConfigManager->get(key)->getElement(MCP23017_NR_4).toInt() == nr) {
            return gpio + 100 + 16 + 16 + 16;
          }
          break;
      }
    }
#endif
    delay(0);
  }
  return OFF_GPIO;
}

uint8_t SuplaConfigESP::getKeyGpio(uint8_t gpio) {
  if ((gpio > 99) && (gpio < 116))
    return KEY_GPIO + gpio - 100;
  if ((gpio > 115) && (gpio < 132))
    return KEY_GPIO + gpio - 116;
  if ((gpio > 131) && (gpio < 148))
    return KEY_GPIO + gpio - 132;
  if ((gpio > 147) && (gpio < 164))
    return KEY_GPIO + gpio - 148;

  return KEY_GPIO + gpio;
}

bool SuplaConfigESP::getLevel(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(LEVEL_RELAY).toInt();
}

bool SuplaConfigESP::getPullUp(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(PULL_UP_BUTTON).toInt();
}

bool SuplaConfigESP::getInversed(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(INVERSED_BUTTON).toInt();
}

uint8_t SuplaConfigESP::getMemory(uint8_t gpio, uint8_t nr) {
  if (gpio == GPIO_VIRTUAL_RELAY) {
    return ConfigManager->get(KEY_VIRTUAL_RELAY_MEMORY)->getElement(nr).toInt();
  }
  return ConfigManager->get(getKeyGpio(gpio))->getElement(MEMORY).toInt();
}

uint8_t SuplaConfigESP::getAction(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(ACTION_BUTTON).toInt();
}

uint8_t SuplaConfigESP::getEvent(uint8_t gpio) {
  return ConfigManager->get(getKeyGpio(gpio))->getElement(EVENT_BUTTON).toInt();
}

bool SuplaConfigESP::checkBusyCfg(int gpio) {
  uint8_t key = KEY_GPIO + gpio;
  if (ConfigManager->get(key)->getElement(CFG).toInt() == 1) {
    return true;
  }
  return false;
}

int SuplaConfigESP::checkBusyGpio(int gpio, int function) {
  if (!checkGpio(gpio)) {
    return false;
  }
  else {
    uint8_t key = KEY_GPIO + gpio;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_BUTTON)
      if (function == FUNCTION_CFG_BUTTON)
        return true;

    if (checkBusyCfg(gpio))
      if (function == FUNCTION_BUTTON)
        return true;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() != FUNCTION_OFF || checkBusyCfg(gpio))
      return false;

    return true;
  }
}

void SuplaConfigESP::setLevel(uint8_t gpio, int level) {
  ConfigManager->setElement(getKeyGpio(gpio), LEVEL_RELAY, level);
}
void SuplaConfigESP::setMemory(uint8_t gpio, int memory, uint8_t nr) {
  if (gpio == GPIO_VIRTUAL_RELAY) {
    ConfigManager->setElement(KEY_VIRTUAL_RELAY_MEMORY, nr, memory);
  }
  else {
    ConfigManager->setElement(getKeyGpio(gpio), MEMORY, memory);
  }
}

void SuplaConfigESP::setPullUp(uint8_t gpio, int pullup) {
  ConfigManager->setElement(getKeyGpio(gpio), PULL_UP_BUTTON, pullup);
}

void SuplaConfigESP::setInversed(uint8_t gpio, int inversed) {
  ConfigManager->setElement(getKeyGpio(gpio), INVERSED_BUTTON, inversed);
}

void SuplaConfigESP::setAction(uint8_t gpio, int action) {
  ConfigManager->setElement(getKeyGpio(gpio), ACTION_BUTTON, action);
}

void SuplaConfigESP::setEvent(uint8_t gpio, int event) {
  ConfigManager->setElement(getKeyGpio(gpio), EVENT_BUTTON, event);
}

void SuplaConfigESP::setGpio(uint8_t gpio, uint8_t nr, uint8_t function) {
  uint8_t key;
  nr++;
  key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 1);
    return;
  }

  ConfigManager->setElement(key, NR, nr);
  ConfigManager->setElement(key, FUNCTION, function);

  /*setLevel(gpio, ConfigESP->getLevel(gpio));
   setMemory(gpio, ConfigESP->getMemory(gpio));
   setPullUp(gpio, ConfigESP->getPullUp(gpio));
   setInversed(gpio, ConfigESP->getInversed(gpio));
   setAction(gpio, ConfigESP->getAction(gpio));
   setEvent(gpio, ConfigESP->getEvent(gpio));
   */
}

void SuplaConfigESP::clearGpio(uint8_t gpio, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    ConfigManager->setElement(key, CFG, 0);
    return;
  }

  ConfigManager->setElement(key, NR, 0);
  ConfigManager->setElement(key, FUNCTION, FUNCTION_OFF);

  if (function == FUNCTION_BUTTON || function == FUNCTION_BUTTON_STOP) {
    setPullUp(gpio, true);
    setInversed(gpio, true);

    setAction(gpio, Supla::Action::TOGGLE);
    setEvent(gpio, Supla::Event::ON_CHANGE);
  }
  if (function == FUNCTION_RELAY) {
    setLevel(gpio, LOW);
    setMemory(gpio, MEMORY_RELAY_RESTORE);
  }
}

uint8_t SuplaConfigESP::countFreeGpio(uint8_t exception) {
  uint8_t count = 0;

#ifdef SUPLA_MCP23017
  if (ConfigESP->checkActiveMCP23017(FUNCTION_RELAY)) {
    return 32;
  }
#endif

  for (uint8_t gpio = 0; gpio < OFF_GPIO; gpio++) {
    if (checkGpio(gpio)) {
      uint8_t key = KEY_GPIO + gpio;
      if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == FUNCTION_OFF ||
          ConfigManager->get(key)->getElement(FUNCTION).toInt() == exception) {
        count++;
      }
    }
    delay(0);
  }
  return count;
}

bool SuplaConfigESP::checkGpio(int gpio) {
  if (
#ifdef ARDUINO_ARCH_ESP8266
      gpio == 6 || gpio == 7 || gpio == 8 || gpio == 11
#ifdef ARDUINO_ESP8266_GENERIC
      || gpio == 9 || gpio == 10
#endif
#elif ARDUINO_ARCH_ESP32
      gpio == 6 || gpio == 7 || gpio == 8 || gpio == 9 || gpio == 10 || gpio == 11 || gpio == 20 || gpio == 24 || gpio == 28 || gpio == 29 ||
      gpio == 30 || gpio == 31 || gpio == 37 || gpio == 38
#endif
  ) {
    return false;
  }

  return true;
}

#ifdef SUPLA_MCP23017
bool SuplaConfigESP::checkBusyGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function) {
  if (gpio == OFF_GPIO_MCP23017) {
    return true;
  }
  else if (gpio == 16) {
    return false;
  }
  else {
    uint8_t key = KEY_GPIO + gpio;
    uint8_t address = ConfigESP->getAdressMCP23017(nr, function);

    if (nr <= 16)
      address = ConfigESP->getAdressMCP23017(1, function);
    if (nr >= 17)
      address = ConfigESP->getAdressMCP23017(17, function);

    if (address == OFF_GPIO_MCP23017) {
      return true;
    }

    if (ConfigManager->get(key)->getElement(getFunctionMCP23017(address)).toInt() != FUNCTION_OFF) {
      return false;
    }
  }
  return true;
}

uint8_t SuplaConfigESP::getGpioMCP23017(uint8_t nr, uint8_t function) {
  uint8_t key, address;
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    key = KEY_GPIO + gpio;
    address = getAdressMCP23017(nr, function);

    if (address != OFF_ADDRESS_MCP23017) {
      if (ConfigManager->get(key)->getElement(getFunctionMCP23017(address)).toInt() == function)
        if (ConfigManager->get(key)->getElement(getNrMCP23017(address)).toInt() == nr)
          return gpio;
    }
    delay(0);
  }
  return OFF_GPIO_MCP23017;
}

uint8_t SuplaConfigESP::getAdressMCP23017(uint8_t nr, uint8_t function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO_MCP23017; gpio++) {
    uint8_t key = KEY_GPIO + gpio;

    if (ConfigManager->get(key)->getElement(MCP23017_NR_1).toInt() == nr)
      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_1).toInt() == function)
        return 0;

    if (ConfigManager->get(key)->getElement(MCP23017_NR_2).toInt() == nr)
      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_2).toInt() == function)
        return 1;

    if (ConfigManager->get(key)->getElement(MCP23017_NR_3).toInt() == nr)
      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_3).toInt() == function)
        return 2;

    if (ConfigManager->get(key)->getElement(MCP23017_NR_4).toInt() == nr)
      if (ConfigManager->get(key)->getElement(MCP23017_FUNCTION_4).toInt() == function)
        return 3;
    delay(0);
  }
  return OFF_ADDRESS_MCP23017;
}

void SuplaConfigESP::setGpioMCP23017(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function) {
  uint8_t key;
  key = KEY_GPIO + gpio;

  ConfigManager->setElement(key, getNrMCP23017(adress), nr);
  ConfigManager->setElement(key, getFunctionMCP23017(adress), function);

  // dla MCP23017 zawsze ustawiać taką samą wartość level, memory, action, event jak dla pierwszego elementu
  setLevel(gpio, ConfigESP->getLevel(gpio));
  setMemory(gpio, ConfigESP->getMemory(gpio));
  setPullUp(gpio, ConfigESP->getPullUp(gpio));
  setInversed(gpio, ConfigESP->getInversed(gpio));
  setAction(gpio, ConfigESP->getAction(gpio));
  setEvent(gpio, ConfigESP->getEvent(gpio));
}

void SuplaConfigESP::clearGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;
  uint8_t adress = getAdressMCP23017(nr, function);

  if (getNrMCP23017(adress) != OFF_GPIO_MCP23017)
    ConfigManager->setElement(key, getNrMCP23017(adress), 0);
  if (getFunctionMCP23017(adress) != OFF_GPIO_MCP23017)
    ConfigManager->setElement(key, getFunctionMCP23017(adress), FUNCTION_OFF);

  if (function == FUNCTION_BUTTON) {
    setPullUp(gpio, true);
    setInversed(gpio, true);
    setAction(gpio, Supla::Action::TOGGLE);
    setEvent(gpio, Supla::Event::ON_CHANGE);
  }
  if (function == FUNCTION_RELAY) {
    setLevel(gpio, true);
    setMemory(gpio, 0);
  }
}

void SuplaConfigESP::clearFunctionGpio(uint8_t function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      ConfigManager->setElement(key, NR, 0);
      ConfigManager->setElement(key, FUNCTION, FUNCTION_OFF);
    }
    delay(0);
  }
}

bool SuplaConfigESP::checkActiveMCP23017(uint8_t function) {
  return ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF &&
         ((ConfigESP->getGpio(function) >= 100 || ConfigESP->getGpio(function) == OFF_GPIO));
}

uint8_t SuplaConfigESP::getFunctionMCP23017(uint8_t adress) {
  switch (adress) {
    case 0:
      return MCP23017_FUNCTION_1;
      break;
    case 1:
      return MCP23017_FUNCTION_2;
      break;
    case 2:
      return MCP23017_FUNCTION_3;
      break;
    case 3:
      return MCP23017_FUNCTION_4;
      break;
  }
  return OFF_GPIO_MCP23017;
}

uint8_t SuplaConfigESP::getNrMCP23017(uint8_t adress) {
  switch (adress) {
    case 0:
      return MCP23017_NR_1;
      break;
    case 1:
      return MCP23017_NR_2;
      break;
    case 2:
      return MCP23017_NR_3;
      break;
    case 3:
      return MCP23017_NR_4;
      break;
  }
  return OFF_GPIO_MCP23017;
}
#endif

void SuplaConfigESP::factoryReset(bool forceReset) {
  delay(2000);
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) != HIGH || forceReset) {
    Serial.println(F("FACTORY RESET!!!"));

    clearEEPROM();

    ConfigManager->deleteAllValues();

    ConfigManager->set(KEY_SUPLA_SERVER, DEFAULT_SERVER);
    ConfigManager->set(KEY_SUPLA_EMAIL, DEFAULT_EMAIL);
    ConfigManager->set(KEY_HOST_NAME, DEFAULT_HOSTNAME);
    ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());
    ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());
    saveChooseTemplateBoard(getDefaultTamplateBoard());
    ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    if (!forceReset) {
      rebootESP();
    }
  }
}
void SuplaConfigESP::reset(bool forceReset) {
  delay(2000);
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) != HIGH || forceReset) {
    Serial.println(F("DEVICES CONFIGURATION RESET!"));

    clearEEPROM();
    ConfigManager->deleteDeviceValues();

    ConfigManager->set(KEY_ENABLE_GUI, getDefaultEnableGUI());
    ConfigManager->set(KEY_ENABLE_SSL, getDefaultEnableSSL());
    saveChooseTemplateBoard(getDefaultTamplateBoard());
    ConfigESP->setGpio(0, FUNCTION_CFG_BUTTON);

    ConfigManager->save();

    if (!forceReset) {
      rebootESP();
    }
  }
}

void SuplaConfigESP::clearEEPROM() {
  EEPROM.begin(1024);
  delay(15);
  for (int i = 1; i < 1024; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}
