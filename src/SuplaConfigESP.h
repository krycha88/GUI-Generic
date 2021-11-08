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
#ifndef SuplaConfigESP_h
#define SuplaConfigESP_h

#include "GUI-Generic_Config.h"

#include "Arduino.h"
#include "EEPROM.h"
#include <supla/action_handler.h>
#include <supla/element.h>
#include "SuplaConfigManager.h"

#include <Ticker.h>

#ifdef ARDUINO_ARCH_ESP8266
#include <cont.h>
#include <user_interface.h>
#include <ESP8266WiFi.h>

#ifdef SUPLA_MDNS
#include <ESP8266mDNS.h>
#endif

#elif ARDUINO_ARCH_ESP32

#ifdef SUPLA_MDNS
#include <ESPmDNS.h>
#endif

#endif

enum _configModeESP
{
  NORMAL_MODE,
  CONFIG_MODE
};
enum _ConfigMode
{
  CONFIG_MODE_10_ON_PRESSES,
  CONFIG_MODE_5SEK_HOLD,
  FACTORYRESET
};

#ifdef ARDUINO_ARCH_ESP8266
#define OFF_GPIO MAX_GPIO + 1
#elif ARDUINO_ARCH_ESP32
#define OFF_GPIO MAX_GPIO + 1
#endif

#define OFF_GPIO_MCP23017    17
#define OFF_ADDRESS_MCP23017 4

#define GPIO_VIRTUAL_RELAY 99

typedef struct {
  int status;
  const char *msg;
} _supla_status;

class SuplaConfigESP : public Supla::ActionHandler, public Supla::Element {
 public:
  SuplaConfigESP();

  void addConfigESP(int _pinNumberConfig, int _pinLedConfig);
  void handleAction(int event, int action);
  void rebootESP();

  bool getDefaultEnableSSL();
  bool getDefaultEnableGUI();
  uint8_t getDefaultTamplateBoard();

  bool checkSSL();
  bool checkAvailableGUI();

  const char *getLastStatusMessageSupla();
  int getLastStatusSupla();

  void ledBlinking(int time);
  void ledBlinkingStop(void);

  String getMacAddress(bool formating);

  _configModeESP configModeESP;
  _supla_status supla_status;

  int getGpio(int nr, int function);
  int getGpio(int function) {
    return getGpio(0, function);
  }

  uint8_t getKeyGpio(uint8_t gpio);

  bool getLevel(uint8_t gpio);
  bool getPullUp(uint8_t gpio);
  bool getInversed(uint8_t gpio);

  uint8_t getMemory(uint8_t gpio, uint8_t nr = 0);
  uint8_t getAction(uint8_t gpio);
  uint8_t getEvent(uint8_t gpio);

  bool checkBusyCfg(int gpio);
  int checkBusyGpio(int gpio, int function);
  uint8_t countFreeGpio(uint8_t exception = 0);
  bool checkGpio(int gpio);

  void setLevel(uint8_t gpio, int level);
  void setMemory(uint8_t gpio, int memory, uint8_t nr = 0);

  void setPullUp(uint8_t gpio, int pullup);
  void setInversed(uint8_t gpio, int inversed);
  void setAction(uint8_t gpio, int action);
  void setEvent(uint8_t gpio, int event);

  void setGpio(uint8_t gpio, uint8_t nr, uint8_t function);
  void setGpio(uint8_t gpio, uint8_t function) {
    setGpio(gpio, 0, function);
  }

  void clearGpio(uint8_t gpio, uint8_t function = 0);
  void reset(bool forceReset = false);
  void factoryReset(bool forceReset = false);
  const String getConfigNameAP();

#ifdef SUPLA_MCP23017
  bool checkBusyGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function);
  uint8_t getGpioMCP23017(uint8_t nr, uint8_t function);
  uint8_t getAdressMCP23017(uint8_t nr, uint8_t function);
  void setGpioMCP23017(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function);
  void clearGpioMCP23017(uint8_t gpio, uint8_t nr, uint8_t function);
  void clearFunctionGpio(uint8_t function);

  bool checkActiveMCP23017(uint8_t function);
  uint8_t getFunctionMCP23017(uint8_t adress);
  uint8_t getNrMCP23017(uint8_t adress);
#endif
  void configModeInit();

 private:
  bool MDNSConfigured = false;
  bool APConfigured = false;
  void iterateAlways();
  void clearEEPROM();

  Ticker led;
};

void ledBlinkingTicker();
void status_func(int status, const char *msg);

uint32_t getFreeStackWatermark();
unsigned long FreeMem();
void checkRAM();

#endif  // SuplaConfigESP_h
