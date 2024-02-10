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
#include "ControlGUI.h"
#include "../../SuplaDeviceGUI.h"

namespace Supla {
namespace Control {
namespace GUI {

Supla::Control::Button *Button(int pin, bool pullUp, bool invertLogic, uint8_t nr) {
  Supla::Control::Button *button = nullptr;

#ifdef GUI_SENSOR_I2C_EXPENDER
  if (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_BUTTON).toInt() != EXPENDER_OFF) {
    button = new Supla::Control::Button(Expander->getIoExpender(nr, FUNCTION_BUTTON), pin, pullUp, invertLogic);
  }
#endif

  if (!button) {
    button = new Supla::Control::Button(pin, pullUp, invertLogic);
  }

  int muliclickTimeMs = ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValueFloat() * 1000;
  int holdTimeMs = ConfigManager->get(KEY_AT_HOLD_TIME)->getValueFloat() * 1000;

  button->setHoldTime(holdTimeMs);
  button->setMulticlickTime(muliclickTimeMs);
  button->repeatOnHoldEvery(250);
  button->setSwNoiseFilterDelay(50);
  button->dontUseOnLoadConfig();

  return button;
}

Supla::Control::Relay *Relay(int pin, bool highIsOn, uint8_t nr) {
  Supla::Control::Relay *relay = nullptr;

#ifdef GUI_SENSOR_I2C_EXPENDER
  if (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_RELAY).toInt() != EXPENDER_OFF) {
    relay = new Supla::Control::Relay(Expander->getIoExpender(nr, FUNCTION_RELAY), pin, highIsOn);
  }
#endif
  if (relay == nullptr) {
    relay = new Supla::Control::Relay(pin, highIsOn);
  }

  return relay;
}

Supla::Control::RollerShutter *RollerShutter(int pinUp, int pinDown, bool highIsOn, uint8_t nr) {
  Supla::Control::RollerShutter *rollerShutterRelay = nullptr;

#ifdef GUI_SENSOR_I2C_EXPENDER
  if (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_RELAY).toInt() != EXPENDER_OFF) {
    rollerShutterRelay = new Supla::Control::RollerShutter(Expander->getIoExpender(nr, FUNCTION_RELAY), pinUp, pinDown, highIsOn);
  }
#endif
  if (rollerShutterRelay == nullptr) {
    rollerShutterRelay = new Supla::Control::RollerShutter(pinUp, pinDown, highIsOn);
  }

  return rollerShutterRelay;
}

Supla::Sensor::Binary *Binary(int pin, bool pullUp, bool invertLogic, uint8_t nr) {
  Supla::Sensor::Binary *binary = nullptr;
#ifdef GUI_SENSOR_I2C_EXPENDER
  if (ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(FUNCTION_LIMIT_SWITCH).toInt() != EXPENDER_OFF) {
    binary = new Supla::Sensor::Binary(Expander->getIoExpender(nr, FUNCTION_LIMIT_SWITCH), pin, pullUp);
  }
#endif
  if (binary == nullptr) {
    binary = new Supla::Sensor::Binary(pin, pullUp);
  }

  return binary;
}

}  // namespace GUI
}  // namespace Control
}  // namespace Supla
