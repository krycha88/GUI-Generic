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
#include "../../GUIGenericCommonDefined.h"
#ifdef GUI_SENSOR_I2C_EXPENDER

#ifndef CONFIG_EXPANDER_H_
#define CONFIG_EXPANDER_H_

#include <supla/io.h>
#include "../../SuplaCommonPROGMEM.h"

#define MAX_EXPANDER_FOR_FUNCTION 32

const char GPIO_A0[] PROGMEM = "A0";
const char GPIO_A1[] PROGMEM = "A1";
const char GPIO_A2[] PROGMEM = "A2";
const char GPIO_A3[] PROGMEM = "A3";
const char GPIO_A4[] PROGMEM = "A4";
const char GPIO_A5[] PROGMEM = "A5";
const char GPIO_A6[] PROGMEM = "A6";
const char GPIO_A7[] PROGMEM = "A7";
const char GPIO_B0[] PROGMEM = "B0";
const char GPIO_B1[] PROGMEM = "B1";
const char GPIO_B2[] PROGMEM = "B2";
const char GPIO_B3[] PROGMEM = "B3";
const char GPIO_B4[] PROGMEM = "B4";
const char GPIO_B5[] PROGMEM = "B5";
const char GPIO_B6[] PROGMEM = "B6";
const char GPIO_B7[] PROGMEM = "B7";

const char GPIO_P00[] PROGMEM = "P0";
const char GPIO_P01[] PROGMEM = "P1";
const char GPIO_P02[] PROGMEM = "P2";
const char GPIO_P03[] PROGMEM = "P3";
const char GPIO_P04[] PROGMEM = "P4";
const char GPIO_P05[] PROGMEM = "P5";
const char GPIO_P06[] PROGMEM = "P6";
const char GPIO_P07[] PROGMEM = "P7";
const char GPIO_P10[] PROGMEM = "P10";
const char GPIO_P11[] PROGMEM = "P11";
const char GPIO_P12[] PROGMEM = "P12";
const char GPIO_P13[] PROGMEM = "P13";
const char GPIO_P14[] PROGMEM = "P14";
const char GPIO_P15[] PROGMEM = "P15";
const char GPIO_P16[] PROGMEM = "P16";
const char GPIO_P17[] PROGMEM = "P17";

const char* const GPIO_MCP23017_P[] PROGMEM = {GPIO_A0, GPIO_A1, GPIO_A2, GPIO_A3, GPIO_A4, GPIO_A5, GPIO_A6, GPIO_A7, GPIO_B0,
                                               GPIO_B1, GPIO_B2, GPIO_B3, GPIO_B4, GPIO_B5, GPIO_B6, GPIO_B7, S_EMPTY, OFF};

const char* const GPIO_PCF_PCF8575_P[] PROGMEM = {GPIO_P00, GPIO_P01, GPIO_P02, GPIO_P03, GPIO_P04, GPIO_P05, GPIO_P06, GPIO_P07, GPIO_P10,
                                                  GPIO_P11, GPIO_P12, GPIO_P13, GPIO_P14, GPIO_P15, GPIO_P16, GPIO_P17, S_EMPTY,  OFF};

const char* const GPIO_PCF_8574_P[] PROGMEM = {GPIO_P00, GPIO_P01, GPIO_P02, GPIO_P03, GPIO_P04, GPIO_P05, GPIO_P06, GPIO_P07, S_EMPTY,
                                               S_EMPTY,  S_EMPTY,  S_EMPTY,  S_EMPTY,  S_EMPTY,  S_EMPTY,  S_EMPTY,  S_EMPTY,  OFF};

const char* const EXPENDER_LIST_P[] PROGMEM = {
    OFF,
#ifdef SUPLA_MCP23017
    "MCP23017",
#else
    S_EMPTY,
#endif
#ifdef SUPLA_PCF8575
    "PCF8575",
#else
    S_EMPTY,
#endif
#ifdef SUPLA_PCF8574
    "PCF8574/A",
#else
    S_EMPTY,
#endif
#ifdef SUPLA_MCP23017
    "MCP23017 I2C2",
#else
    S_EMPTY,
#endif
#ifdef SUPLA_PCF8575
    "PCF8575  I2C2",
#else
    S_EMPTY,
#endif
#ifdef SUPLA_PCF8574
    "PCF8574/A  I2C2",
#else
    S_EMPTY,
#endif
};

enum
{
  EXPENDER_OFF,
  EXPENDER_MCP23017,
  EXPENDER_PCF8575,
  EXPENDER_PCF8574,
  EXPENDER_MCP23017_I2C2,
  EXPENDER_PCF8575_I2C2,
  EXPENDER_PCF8574_I2C2,
  EXPENDER_COUNT
};

namespace Supla {
namespace Control {
class ConfigExpander {
 public:
  ConfigExpander();
  bool checkBusyGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function);
  uint8_t getGpioExpander(uint8_t nr, uint8_t function);
  uint8_t getAdressExpander(uint8_t nr, uint8_t function);
  void setGpioExpander(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function);
  void clearGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function);
  void clearFunctionGpioExpander(uint8_t function);

  bool checkActiveExpander(uint8_t function);
  uint8_t getFunctionExpander(uint8_t adress);
  uint8_t getNrExpander(uint8_t adress);
  Supla::Io::Base* getIoExpender(uint8_t nr, uint8_t function);

 protected:
  struct IoExpender {
    Supla::Io::Base* io = {nullptr};
#ifdef ARDUINO_ARCH_ESP32
    Supla::Io::Base* io_Wire1 = {nullptr};
#endif
  };

  IoExpender ioExpender[4];
};
};  // namespace Control
};  // namespace Supla

#endif
#endif