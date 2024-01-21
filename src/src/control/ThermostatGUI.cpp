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

#if SUPLA_THERMOSTAT
#include "ThermostatGUI.h"

namespace Supla {
namespace Control {
namespace GUI {

ThermostatGUI::ThermostatGUI(uint8_t thermostatNumber, SuplaDeviceClass *sdc)
    : Supla::Control::HvacBase(new Supla::Control::InternalPinOutput(ConfigESP->getGpio(thermostatNumber, FUNCTION_RELAY),
                                                                     ConfigESP->getLevel(ConfigESP->getGpio(thermostatNumber, FUNCTION_RELAY)))),
      Supla::Protocol::ProtocolLayer(sdc) {
  setNumber(thermostatNumber);
  uint8_t pinLED = ConfigESP->getGpio(getNumber(), FUNCTION_LED);
  bool levelLed = ConfigESP->getLevel(pinLED);

  uint8_t mainThermometr = ConfigManager->get(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL)->getElement(getNumber()).toInt();
  uint8_t auxThermometr = ConfigManager->get(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL)->getElement(getNumber()).toInt();
  double histeresis = ConfigManager->get(KEY_THERMOSTAT_HISTERESIS)->getElement(getNumber()).toDouble();

  new Supla::Clock;

  HvacBase::setTemperatureHisteresis(histeresis * 100.0);

  if (mainThermometr != HvacBase::getChannelNumber()) {
    HvacBase::setMainThermometerChannelNo(mainThermometr);
  }

  if (auxThermometr != HvacBase::getChannelNumber()) {
    HvacBase::setAuxThermometerChannelNo(auxThermometr);

    HvacBase::setAuxThermometerType(SUPLA_HVAC_AUX_THERMOMETER_TYPE_FLOOR);
    HvacBase::setTemperatureAuxMin(500);   // 5 degrees
    HvacBase::setTemperatureAuxMax(7500);  // 75 degrees
  }

  // Configure thermostat parameters
  HvacBase::setTemperatureHisteresisMin(10);    // 0.1 degree
  HvacBase::setTemperatureHisteresisMax(4000);  // 10 degree
  HvacBase::setTemperatureAutoOffsetMin(200);   // 2 degrees
  HvacBase::setTemperatureAutoOffsetMax(1000);  // 10 degrees
  HvacBase::addAvailableAlgorithm(SUPLA_HVAC_ALGORITHM_ON_OFF_SETPOINT_MIDDLE);

  if (strcmp(ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MIN)->getElement(getNumber()).c_str(), "") != 0) {
    uint8_t temperatureMin = ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MIN)->getElement(getNumber()).toInt();
    HvacBase::setDefaultTemperatureRoomMin(SUPLA_CHANNELFNC_HVAC_THERMOSTAT, temperatureMin * 100);
  }
  if (strcmp(ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MAX)->getElement(getNumber()).c_str(), "") != 0) {
    uint8_t temperatureMax = ConfigManager->get(KEY_THERMOSTAT_TEMPERATURE_MAX)->getElement(getNumber()).toInt();
    HvacBase::setDefaultTemperatureRoomMax(SUPLA_CHANNELFNC_HVAC_THERMOSTAT, temperatureMax * 100);
  }
  uint8_t thermostatType = ConfigManager->get(KEY_THERMOSTAT_TYPE)->getElement(getNumber()).toInt();
  setThermostatType(thermostatType);

  if (pinLED != OFF_GPIO) {
    auto statusLed = new Supla::Control::InternalPinOutput(pinLED, levelLed);

    if (thermostatType == Supla::GUI::THERMOSTAT_COOL) {
      HvacBase::addAction(Supla::TURN_OFF, statusLed, Supla::ON_HVAC_STANDBY, true);
      HvacBase::addAction(Supla::TURN_OFF, statusLed, Supla::ON_HVAC_HEATING, true);
      HvacBase::addAction(Supla::TURN_ON, statusLed, Supla::ON_HVAC_COOLING, true);
    }
    else {
      HvacBase::addAction(Supla::TURN_OFF, statusLed, Supla::ON_HVAC_STANDBY, true);
      HvacBase::addAction(Supla::TURN_OFF, statusLed, Supla::ON_HVAC_COOLING, true);
      HvacBase::addAction(Supla::TURN_ON, statusLed, Supla::ON_HVAC_HEATING, true);
    }
  }

  HvacBase::setButtonTemperatureStep(10);

#ifndef SUPLA_OLED
  Supla::GUI::addButtonToRelay(thermostatNumber, this, this);
#else
  if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt() == 0) {
    Supla::GUI::addButtonToRelay(thermostatNumber, this, this);
  }
#endif
}

void ThermostatGUI::notifyConfigChange(int channelNumber) {
  if (HvacBase::getChannelNumber() == channelNumber) {
    auto function = HvacBase::getChannelFunction();

    switch (function) {
      case SUPLA_CHANNELFNC_HVAC_THERMOSTAT: {
        if (HvacBase::isHeatingSubfunction()) {
          ConfigManager->setElement(KEY_THERMOSTAT_TYPE, getNumber(), Supla::GUI::THERMOSTAT_HEAT);
        }
        if (HvacBase::isCoolingSubfunction()) {
          ConfigManager->setElement(KEY_THERMOSTAT_TYPE, getNumber(), Supla::GUI::THERMOSTAT_COOL);
        }
        break;
      }
      case SUPLA_CHANNELFNC_HVAC_THERMOSTAT_DIFFERENTIAL:
      case SUPLA_CHANNELFNC_HVAC_DOMESTIC_HOT_WATER: {
        if (HvacBase::isDomesticHotWaterFunctionSupported()) {
          ConfigManager->setElement(KEY_THERMOSTAT_TYPE, getNumber(), Supla::GUI::THERMOSTAT_DOMESTIC_HOT_WATER);
        }
        if (HvacBase::isDifferentialFunctionSupported()) {
          ConfigManager->setElement(KEY_THERMOSTAT_TYPE, getNumber(), Supla::GUI::THERMOSTAT_DIFFERENTIAL);
        }
        break;
      }
      case SUPLA_CHANNELFNC_HVAC_THERMOSTAT_AUTO: {
        ConfigManager->setElement(KEY_THERMOSTAT_TYPE, getNumber(), Supla::GUI::THERMOSTAT_AUTO);
        break;
      }
    }

    ConfigManager->setElement(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, getNumber(), static_cast<int>(HvacBase::getMainThermometerChannelNo()));
    ConfigManager->setElement(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL, getNumber(), static_cast<int>(HvacBase::getAuxThermometerChannelNo()));
    ConfigManager->setElement(KEY_THERMOSTAT_HISTERESIS, getNumber(), static_cast<double>(HvacBase::getTemperatureHisteresis() / 100.0));
    ConfigManager->save();
  }
}

void ThermostatGUI::setThermostatType(uint8_t thermostatType) {
  switch (thermostatType) {
    case Supla::GUI::THERMOSTAT_HEAT:
      HvacBase::getChannel()->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
      HvacBase::setDefaultSubfunction(SUPLA_HVAC_SUBFUNCTION_HEAT);
      break;

    case Supla::GUI::THERMOSTAT_COOL:
      HvacBase::getChannel()->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
      HvacBase::setDefaultSubfunction(SUPLA_HVAC_SUBFUNCTION_COOL);
      break;

    case Supla::GUI::THERMOSTAT_AUTO:
      HvacBase::getChannel()->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT_AUTO);
      break;

    case Supla::GUI::THERMOSTAT_DOMESTIC_HOT_WATER:
      HvacBase::enableDomesticHotWaterFunctionSupport();
      HvacBase::getChannel()->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_DOMESTIC_HOT_WATER);
      break;

    case Supla::GUI::THERMOSTAT_DIFFERENTIAL:
      HvacBase::getChannel()->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT_DIFFERENTIAL);
      break;

    default:
      break;
  }
}

};  // namespace GUI
};  // namespace Control
};  // namespace Supla

#ifdef SUPLA_THERMOSTAT
namespace Supla {
namespace GUI {
std::array<Supla::Control::GUI::ThermostatGUI *, MAX_THERMOSTAT> thermostatArray;
}
}  // namespace Supla
#endif

#endif
