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

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include <supla/tools.h>

SuplaWebServer::SuplaWebServer() {
  isRunningWebServer = false;
}

void SuplaWebServer::begin() {
  if (isRunningWebServer) {
    return;
  }

#ifdef ARDUINO_ARCH_ESP8266
  httpServer = new ESP8266WebServer(80);
#elif ARDUINO_ARCH_ESP32
  httpServer = new ESP32WebServer(80);
#endif

#ifdef SUPLA_OTA
  httpUpdater = new HTTPUpdateServer();
  httpUpdater->setup();
#endif

  this->createWebServer();
  httpServer->onNotFound(std::bind(&SuplaWebServer::handleNotFound, this));
  httpServer->begin();
  isRunningWebServer = true;
}

void SuplaWebServer::iterateAlways() {
  if (isRunningWebServer) {
    httpServer->handleClient();
  }
}

void SuplaWebServer::createWebServer() {
  createWebPageHome();
  createWebPageDeviceSettings();

#ifdef GUI_RELAY
  createWebPageRelay();
#endif
#ifdef GUI_CONTROL
  createWebPageControl();
#endif
#ifdef SUPLA_LIMIT_SWITCH
  createWebPageLimitSwitch();
#endif
#ifdef SUPLA_CONFIG
  createWebPageConfig();
#endif

  createWebUpload();
  createWebTools();

#ifdef GUI_OTHER
  createWebPageOther();
#endif
  createWebPageSensors();

#ifdef SUPLA_CONDITIONS
  createWebConditions();
#endif
}

void SuplaWebServer::sendHeaderStart() {
  if (!chunkedSendHeader) {
    // printFreeMemory();
    chunkedSendHeader = true;
    char buf[512] = {};
    char freeHeapStr[10];
    ConfigESP->getFreeHeapAsString(freeHeapStr);

#ifdef ARDUINO_ARCH_ESP8266
    tcpCleanup();
#endif
    httpServer->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
    httpServer->sendHeader(F("Pragma"), F("no-cache"));
    httpServer->sendHeader(F("Expires"), F("-1"));
    httpServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
    httpServer->chunkedResponseModeStart(200, F("text/html"));

    httpServer->sendContent_P(HTTP_META);
    httpServer->sendContent_P(HTTP_FAVICON);
    httpServer->sendContent_P(HTTP_STYLE);
    httpServer->sendContent_P(HTTP_SCRIPT);
    httpServer->sendContent_P(HTTP_DIV_START);
    httpServer->sendContent_P(HTTP_IFRAMES);
    httpServer->sendContent_P(HTTP_LOGO);

    WebServer->sendContent(F("<h1>"));
    WebServer->sendContent(ConfigManager->get(KEY_HOST_NAME)->getValue());
    WebServer->sendContent(F("</h1><span>LAST STATE: "));
    WebServer->sendContent(ConfigESP->getLastStatusMessageSupla());
    WebServer->sendContent(F("<br>Firmware: "));
    WebServer->sendContent(Supla::Channel::reg_dev.SoftVer);
    WebServer->sendContent(F("<br>GUID: "));

    generateHexString(Supla::Channel::reg_dev.GUID, buf, SUPLA_GUID_SIZE);
    WebServer->sendContent(buf);

    WebServer->sendContent(F("<br>MAC: "));
    ConfigESP->getMacAddress(buf, true);
    WebServer->sendContent(buf);
    WebServer->sendContent(F("</span>"));

    WebServer->sendContent(F("<span>"));
    WebServer->sendContent(F("Free Mem: "));
    WebServer->sendContent(freeHeapStr);
    WebServer->sendContent(F("KB<br>Mode: "));

    if (ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL) {
      WebServer->sendContent(F("NORMAL"));
    }
    else {
      WebServer->sendContent(F("CONFIG"));
    }
    WebServer->sendContent(F("</span>"));
  }
}

void SuplaWebServer::sendContent(const String& content) {
  if (chunkedSendHeader) {
    if (!content.isEmpty()) {
      httpServer->sendContent(content);
    }
  }
}

void SuplaWebServer::sendContent(double content) {
  this->sendContent(String(content).c_str());
};

void SuplaWebServer::sendHeaderEnd() {
  if (chunkedSendHeader) {
    // sendHeader();
    addButton(S_RESTART, getParameterRequest("", PATH_REBOT, "3"));
    addButton(S_TOOLS, PATH_TOOLS);
    httpServer->sendContent_P(HTTP_DIV_END);
    httpServer->chunkedResponseFinalize();

#ifdef ARDUINO_ARCH_ESP8266
    tcpCleanup();
#endif
    httpServer->client().flush();
    httpServer->client().stop();
    chunkedSendHeader = false;
    //printFreeMemory();
  }
}

void printFreeMemory() {
  size_t freeMemory = ESP.getFreeHeap();

  Serial.print("Free memory: ");
  Serial.print(freeMemory);
  Serial.println(" bytes");
}

void SuplaWebServer::handleNotFound() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  httpServer->sendHeader("Location", PATH_START, true);
  httpServer->send(302, "text/plane", "");
  handlePageHome();
}

bool SuplaWebServer::isLoggedIn(bool force) {
  if (ConfigESP->configModeESP == Supla::DEVICE_MODE_NORMAL || force) {
    if (strcmp(ConfigManager->get(KEY_LOGIN)->getValue(), "") != 0 && strcmp(ConfigManager->get(KEY_LOGIN_PASS)->getValue(), "") != 0 &&
        !httpServer->authenticate(ConfigManager->get(KEY_LOGIN)->getValue(), ConfigManager->get(KEY_LOGIN_PASS)->getValue())) {
      httpServer->requestAuthentication();
      return false;
    }
  }
  return true;
}

bool SuplaWebServer::saveGPIO(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t gpio = OFF_GPIO, _gpio = OFF_GPIO, _function = FUNCTION_OFF, _nr = 0, current_value = 0, key = KEY_GPIO;
  String input;
  input.reserve(16);
  input = _input + nr;

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

#ifdef GUI_SENSOR_I2C_EXPENDER
  ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, static_cast<int>(WebServer->httpServer->arg(INPUT_EXPENDER_TYPE).toInt()));
#endif

  gpio = ConfigESP->getGpio(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  // VIRTUAL RELAY
  if (function == FUNCTION_RELAY && _gpio == GPIO_VIRTUAL_RELAY) {
    if (gpio != GPIO_VIRTUAL_RELAY) {
      ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
      ConfigESP->clearGpio(gpio, function, nr);
    }

    ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, true);
    ConfigESP->setNumberButton(nr);

    if (input_max != "\n") {
      current_value = WebServer->httpServer->arg(input_max).toInt();
      if (nr >= current_value) {
        ConfigManager->setElement(KEY_VIRTUAL_RELAY, nr, false);
      }
    }

    return true;
  }

#ifdef ARDUINO_ARCH_ESP8266
  // ANALOG BUTTON
  if (function == FUNCTION_BUTTON && _gpio == A0) {
    if (gpio != A0) {
      ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, false);
      ConfigESP->clearGpio(gpio, function, nr);
    }

    ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, true);

    if (input_max != "\n") {
      current_value = WebServer->httpServer->arg(input_max).toInt();
      if (nr >= current_value) {
        ConfigManager->setElement(KEY_ANALOG_BUTTON, nr, false);
      }
    }
    return true;
  }
#endif

  key = KEY_GPIO + _gpio;

  if (function == FUNCTION_CFG_BUTTON) {
    _function = ConfigManager->get(key)->getElement(CFG).toInt();
  }
  else if (function == FUNCTION_CFG_LED) {
    _function = ConfigManager->get(key)->getElement(CFG_LED).toInt();
  }
  else {
    _function = ConfigManager->get(key)->getElement(FUNCTION).toInt();
    _nr = ConfigManager->get(key)->getElement(NR).toInt() - 1;
  }

  if (_gpio == OFF_GPIO) {
    ConfigESP->clearGpio(gpio, function, nr);
  }

  if (_gpio != OFF_GPIO) {
    if (_function == FUNCTION_OFF) {
      ConfigESP->clearGpio(gpio, function, nr);
      ConfigESP->clearGpio(_gpio, function, nr);
      ConfigESP->setGpio(_gpio, nr, function);

#ifdef SUPLA_ROLLERSHUTTER
      if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr) {
        // if (nr % 2 == 0) {
        ConfigESP->setEvent(_gpio, Supla::GUI::Event::ON_PRESS);
        ConfigESP->setAction(_gpio, Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE);
        //  }
      }
#endif
    }
    else if (function == FUNCTION_CFG_BUTTON || function == FUNCTION_CFG_LED) {
      ConfigESP->setGpio(_gpio, function);
    }
    else if (gpio == _gpio && _function == function && _nr == nr) {
      ConfigESP->setGpio(_gpio, nr, function);
    }
    else {
      return false;
    }
  }

  if (input_max != "\n") {
    current_value = WebServer->httpServer->arg(input_max).toInt();
    if ((ConfigManager->get(key)->getElement(NR).toInt() - 1) >= current_value) {
      ConfigESP->clearGpio(gpio, function, nr);
    }
  }

  return true;
}

#ifdef GUI_SENSOR_I2C_EXPENDER
bool SuplaWebServer::saveGpioMCP23017(const String& _input, uint8_t function, uint8_t nr, const String& input_max) {
  uint8_t key, _address, gpio, _gpio, _function, _nr, _type, shiftAddress;
  String input = _input + "mcp" + nr;

  if (nr >= MAX_EXPANDER_FOR_FUNCTION) {
    return saveGPIO(_input, function, nr, input_max);
  }

  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") == 0) {
    return true;
  }

  _type = WebServer->httpServer->arg(INPUT_EXPENDER_TYPE).toInt();
  ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, _type);

  if (_type == FUNCTION_OFF) {
    Expander->clearFunctionGpioExpander(function);
    return true;
  }

  if (_type == EXPENDER_PCF8574 || _type == EXPENDER_PCF8574_I2C2) {
    shiftAddress = 8;
  }
  else {
    shiftAddress = 16;
  }

  if (nr < shiftAddress)
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + 0).toInt();
  else
    _address = WebServer->httpServer->arg(String(INPUT_ADRESS_MCP23017) + shiftAddress).toInt();

  gpio = Expander->getGpioExpander(nr, function);
  _gpio = WebServer->httpServer->arg(input).toInt();

  key = KEY_GPIO + _gpio;
  _function = ConfigManager->get(key)->getElement(Expander->getFunctionExpander(_address)).toInt();
  _nr = ConfigManager->get(key)->getElement(Expander->getNrExpander(_address)).toInt();

  if (_gpio == OFF_GPIO_EXPENDER || _address == OFF_ADDRESS_MCP23017) {
    Expander->clearGpioExpander(gpio, nr, function);
  }
  else if (_function == FUNCTION_OFF) {
    Expander->clearGpioExpander(gpio, nr, function);
    Expander->clearGpioExpander(_gpio, nr, function);
    Expander->setGpioExpander(_gpio, _address, nr, function);

    if (function == FUNCTION_BUTTON)
      ConfigESP->setNumberButton(nr);
#ifdef SUPLA_ROLLERSHUTTER
    if (ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt() * 2 > nr) {
      if (nr % 2 == 0) {
        ConfigESP->setEvent(_gpio, Supla::GUI::Event::ON_PRESS);
        ConfigESP->setAction(_gpio, Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE);
      }
    }
#endif
  }
  else if (gpio == _gpio && function == _function && nr == _nr) {
    Expander->setGpioExpander(_gpio, _address, nr, function);
  }
  else {
    return false;
  }
  return true;
}
#endif

#ifdef ARDUINO_ARCH_ESP8266

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup() {
  while (tcp_tw_pcbs != NULL) {
    tcp_abort(tcp_tw_pcbs);
  }
}
#endif
