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
#include "ConfigExpander.h"

#ifdef GUI_SENSOR_I2C_EXPENDER
#include "../../SuplaDeviceGUI.h"

#include "ExpanderPCF8575.h"
#include "ExpanderPCF8574.h"
#include "ExpanderMCP23017.h"

namespace Supla {
namespace Control {

ConfigExpander::ConfigExpander() {
}

bool ConfigExpander::checkBusyGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;
  uint8_t address = OFF_ADDRESS_MCP23017, maxNr;
  uint8_t type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (type == EXPENDER_PCF8574 || type == EXPENDER_PCF8574_I2C2) {
    maxNr = 8;
  }
  else {
    maxNr = 16;
  }

  if (nr < maxNr) {
    maxNr = 0;
  }

  for (uint8_t gpio = maxNr; gpio <= OFF_GPIO_EXPENDER; gpio++) {
    address = getAdressExpander(gpio, function);

    if (address != OFF_ADDRESS_MCP23017) {
      break;
    }
  }

  if (ConfigManager->get(key)->getElement(getFunctionExpander(address)).toInt() != FUNCTION_OFF) {
    return false;
  }
  return true;
}

uint8_t ConfigExpander::getGpioExpander(uint8_t nr, uint8_t function) {
  uint8_t key, address;
  for (uint8_t gpio = 0; gpio <= OFF_GPIO_EXPENDER; gpio++) {
    key = KEY_GPIO + gpio;
    address = getAdressExpander(nr, function);

    if (address != OFF_ADDRESS_MCP23017) {
      if (ConfigManager->get(key)->getElement(getFunctionExpander(address)).toInt() == function)
        if (ConfigManager->get(key)->getElement(getNrExpander(address)).toInt() == nr)
          return gpio;
    }
    delay(0);
  }
  return OFF_GPIO_EXPENDER;
}

uint8_t ConfigExpander::getAdressExpander(uint8_t nr, uint8_t function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO_EXPENDER; gpio++) {
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

void ConfigExpander::setGpioExpander(uint8_t gpio, uint8_t adress, uint8_t nr, uint8_t function) {
  uint8_t key;
  key = KEY_GPIO + gpio;

  ConfigManager->setElement(key, getNrExpander(adress), nr);
  ConfigManager->setElement(key, getFunctionExpander(adress), function);

  // dla MCP23017 zawsze ustawiać taką samą wartość level, memory, action, event jak dla pierwszego elementu
  ConfigESP->setLevel(gpio, ConfigESP->getLevel(gpio));
  ConfigESP->setMemory(gpio, ConfigESP->getMemory(gpio));
  ConfigESP->setPullUp(gpio, ConfigESP->getPullUp(gpio));
  ConfigESP->setInversed(gpio, ConfigESP->getInversed(gpio));
  ConfigESP->setAction(gpio, ConfigESP->getAction(gpio));
  ConfigESP->setEvent(gpio, ConfigESP->getEvent(gpio));
}

void ConfigExpander::clearGpioExpander(uint8_t gpio, uint8_t nr, uint8_t function) {
  uint8_t key = KEY_GPIO + gpio;
  uint8_t adress = getAdressExpander(nr, function);
  uint8_t type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (getNrExpander(adress) != OFF_GPIO_EXPENDER)
    ConfigManager->setElement(key, getNrExpander(adress), 0);
  if (getFunctionExpander(adress) != OFF_GPIO_EXPENDER)
    ConfigManager->setElement(key, getFunctionExpander(adress), FUNCTION_OFF);

  if (function == FUNCTION_BUTTON) {
    if (type == EXPENDER_PCF8575 || type == EXPENDER_PCF8575_I2C2) {
      ConfigESP->setPullUp(gpio, false);
    }
    else {
      ConfigESP->setPullUp(gpio, true);
    }

    ConfigESP->setInversed(gpio, true);
    ConfigESP->setAction(gpio, Supla::GUI::Action::TOGGLE);
    ConfigESP->setEvent(gpio, Supla::GUI::Event::ON_PRESS);
  }

  if (function == FUNCTION_LIMIT_SWITCH) {
    ConfigESP->setPullUp(gpio, false);
  }

  if (function == FUNCTION_RELAY) {
    ConfigESP->setLevel(gpio, true);
    ConfigESP->setMemory(gpio, 0);
  }
}

void ConfigExpander::clearFunctionGpioExpander(uint8_t function) {
  for (uint8_t gpio = 0; gpio <= OFF_GPIO; gpio++) {
    uint8_t key = KEY_GPIO + gpio;

    if (ConfigManager->get(key)->getElement(FUNCTION).toInt() == function) {
      ConfigManager->setElement(key, NR, 0);
      ConfigManager->setElement(key, FUNCTION, FUNCTION_OFF);
    }

    clearGpioExpander(gpio, gpio, function);

    delay(0);
  }
}

bool ConfigExpander::checkActiveExpander(uint8_t function) {
  // return (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt() != FUNCTION_OFF ||
  //         ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_PCF857X).toInt() != FUNCTION_OFF) &&
  //        ((ConfigESP->getGpio(function) >= 100 || ConfigESP->getGpio(function) == OFF_GPIO));

  return ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();
}

uint8_t ConfigExpander::getFunctionExpander(uint8_t adress) {
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
  return OFF_GPIO_EXPENDER;
}

uint8_t ConfigExpander::getNrExpander(uint8_t adress) {
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
  return OFF_GPIO_EXPENDER;
}

Supla::Io::Base *ConfigExpander::getIoExpender(uint8_t nr, uint8_t function) {
  Supla::Io::Base *io = nullptr;
  uint8_t address = getAdressExpander(nr, function);
  uint8_t addressHex = 0x20 | (address & 0x07);

  if (address != OFF_ADDRESS_MCP23017) {
    switch (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt()) {
#ifdef SUPLA_PCF8575
      case EXPENDER_PCF8575:
        if (ioExpender[address].io == nullptr) {
          ioExpender[address].io = new Supla::Control::ExpanderPCF8575(&Wire, addressHex);
        }
        io = ioExpender[address].io;
        break;
#endif
#ifdef SUPLA_PCF8574
      case EXPENDER_PCF8574:
        if (ioExpender[address].io == nullptr) {
          ioExpender[address].io = new Supla::Control::ExpanderPCF8574(&Wire, addressHex);
        }
        io = ioExpender[address].io;
        break;
#endif
#ifdef SUPLA_MCP23017
      case EXPENDER_MCP23017:
        if (ioExpender[address].io == nullptr) {
          ioExpender[address].io = new Supla::Control::ExpanderMCP23017(&Wire, addressHex);
        }
        io = ioExpender[address].io;
        break;
#endif

#if defined(ARDUINO_ARCH_ESP32) && SOC_HP_I2C_NUM > 1
#ifdef SUPLA_PCF8575
      case EXPENDER_PCF8575_I2C2:
        if (ioExpender[address].io_Wire1 == nullptr) {
          ioExpender[address].io_Wire1 = new Supla::Control::ExpanderPCF8575(&Wire1, addressHex);
        }
        io = ioExpender[address].io_Wire1;
        break;
#endif
#ifdef SUPLA_PCF8574
      case EXPENDER_PCF8574_I2C2:
        if (ioExpender[address].io_Wire1 == nullptr) {
          ioExpender[address].io_Wire1 = new Supla::Control::ExpanderPCF8574(&Wire1, addressHex);
        }
        io = ioExpender[address].io_Wire1;
        break;
#endif
#ifdef SUPLA_MCP23017
      case EXPENDER_MCP23017_I2C2:
        if (ioExpender[address].io_Wire1 == nullptr) {
          ioExpender[address].io_Wire1 = new Supla::Control::ExpanderMCP23017(&Wire1, addressHex);
        }
        io = ioExpender[address].io_Wire1;
        break;
#endif
#endif
    }
  }

  return io;
}

}  // namespace Control
}  // namespace Supla
#endif