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
#include <supla/io.h>

#include "GUIGenericCommonDefined.h"
#include "GUI-Generic_Config.h"

#include "Arduino.h"
#include <stdint.h>
#include "EEPROM.h"
#include <supla/action_handler.h>
#include <supla/element.h>
#include "SuplaConfigManager.h"
#include <supla/actions.h>

#include <Ticker.h>

#ifdef ARDUINO_ARCH_ESP8266
#include <cont.h>
#include <user_interface.h>
#include <ESP8266WiFi.h>

#ifdef SUPLA_MDNS
#include <ESP8266mDNS.h>
#endif

#elif ARDUINO_ARCH_ESP32
#include <WiFi.h>
#ifdef SUPLA_MDNS
#include <ESPmDNS.h>
#endif

#endif

enum _ConfigMode
{
  CONFIG_MODE_10_ON_PRESSES,
  CONFIG_MODE_5SEK_HOLD,
  CONFIG_MODE_RESET,
  FACTORYRESET
};

#ifdef ARDUINO_ARCH_ESP8266
#define OFF_GPIO MAX_GPIO + 1
#elif ARDUINO_ARCH_ESP32
#define OFF_GPIO MAX_GPIO + 1
#endif

#define OFF_GPIO_EXPENDER    17
#define OFF_ADDRESS_MCP23017 4

#define GPIO_VIRTUAL_RELAY 77

typedef struct {
  int status;
  const char *msg;
} _supla_status;

enum ResetType
{
  RESET_NO_ERASE_DATA,
  RESET_DEVICE_DATA,
  RESET_FACTORY_DATA
};

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

  const char *getLastStatusMessageSupla();
  int getLastStatusSupla();

  void ledBlinking(int time);
  void ledBlinkingStop(void);

  String getMacAddress(bool formating);
  void getMacAddress(char *macAddress, bool formating);
  void getFreeHeapAsString(char *freeHeapStr);

  uint8_t configModeESP;
  _supla_status supla_status;

  int getGpio(int nr, int function);
  int getGpio(int function) {
    return getGpio(0, function);
  }

  HardwareSerial &getHardwareSerial(int8_t rxPin, int8_t txPin = -1);

  uint8_t getBaudRate(uint8_t gpio);
  void setBaudRate(uint8_t gpio, int baudRate);
  int getBaudRateSpeed(uint8_t gpio);

  uint8_t getNumberButton(uint8_t nr);
  uint8_t getNumberButtonAdditional(uint8_t functionButton, uint8_t nr = 0);
  uint8_t getKeyGpio(uint8_t gpio);

  bool getLevel(uint8_t gpio);
  bool getPullUp(uint8_t gpio);
  bool getInversed(uint8_t gpio);

  uint8_t getMemory(uint8_t gpio, uint8_t nr = 0);
  uint8_t getAction(uint8_t gpio);
  Supla::Action getActionInternal(uint8_t gpio);
  uint8_t getEvent(uint8_t gpio);
  uint8_t getLightRelay(uint8_t gpio);

  int getBrightnessLevelOLED();
  void setBrightnessLevelOLED(int newBrightness);

  bool checkBusyCfg(int gpio, int function);
  int checkBusyGpio(int gpio, int function);
  bool checkBusyGpio(int gpio);
  uint8_t countFreeGpio(uint8_t exception = 0);
  bool checkGpio(int gpio);

  void setLevel(uint8_t gpio, int level);
  void setMemory(uint8_t gpio, int memory, uint8_t nr = 0);

  void setPullUp(uint8_t gpio, int pullup);
  void setInversed(uint8_t gpio, int inversed);
  void setAction(uint8_t gpio, int action);
  void setEvent(uint8_t gpio, int event);
  void setLightRelay(uint8_t gpio, int type);

  void setNumberButton(uint8_t nr) {
    setNumberButton(nr, nr);
  }
  void setNumberButton(uint8_t nr, uint8_t nrButton);

  void setGpio(uint8_t gpio, uint8_t nr, uint8_t function);
  void setGpio(uint8_t gpio, uint8_t function) {
    setGpio(gpio, 0, function);
  }

  void clearGpio(uint8_t gpio, uint8_t function = 0, uint8_t nr = 0);

  void commonReset(const char *resetMessage, ResetType resetType, bool forceReset = false);

  const String getConfigNameAP();

  void configModeInit();
  void clearEEPROM();

 private:
  bool MDNSConfigured = false;
  void iterateAlways();

  Ticker led;
};

void ledBlinkingTicker();
void status_func(int status, const char *msg);
#endif  // SuplaConfigESP_h
