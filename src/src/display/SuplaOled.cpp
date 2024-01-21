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

#include "SuplaOled.h"
#include <supla/clock/clock.h>
#include <supla/sensor/therm_hygro_press_meter.h>
#include <supla/sensor/distance.h>

#ifdef SUPLA_OLED

struct oledStruct {
  uint8_t chanelSensor;
  bool forSecondaryValue;
  uint8_t nrRealy;
};

oledStruct* oled;

String getTempString(double temperature) {
  if (temperature == TEMPERATURE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(temperature, 1);
  }
}

String getHumidityString(double humidity) {
  if (humidity == HUMIDITY_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(humidity, 1);
  }
}

String getPressureString(double pressure) {
  if (pressure == PRESSURE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(pressure, 0);
  }
}

String getDistanceString(double distance) {
  if (distance == DISTANCE_NOT_AVAILABLE) {
    return S_ERROR;
  }
  else {
    return String(distance, 2).c_str();
  }
}

int getWidthUnit(OLEDDisplay* display, const String& value) {
  if (value == S_ERROR) {
    return (display->getWidth() / 2) + (display->getStringWidth(value) / 4.5);
  }

  return (display->getWidth() / 2) + (display->getStringWidth(value) / 2);
}

int getWidthValue(OLEDDisplay* display, const String& value) {
  if (value == S_ERROR) {
    return (display->getWidth() / 2) - (display->getStringWidth(value) / 5);
  }

  return (display->getWidth() / 2) - (display->getStringWidth(value) / 2);
}

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
#ifdef ARDUINO_ARCH_ESP32
  int dBm = 100;
  // WiFi.RSSI() dla ESP32 zawiesza całą pętlę
#else
  int dBm = WiFi.RSSI();
#endif
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void displayUiSignal(OLEDDisplay* display) {
  int x = display->getWidth() - 17;
  int y = 0;
  int value = getQuality();

  display->setColor(BLACK);
  display->fillRect(x, y, x + 46, 16);
  display->setColor(WHITE);
  if (value == -1) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + 1, y, "x");
  }
  else {
    if (value > 0)
      display->fillRect(x, y + 6, 3, 4);

    if (value >= 25)
      display->fillRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display->fillRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display->fillRect(x + 12, y, 3, 10);
  }
}

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void displayUiRelayState(OLEDDisplay* display) {
  int y = 0;
  int x = 0;

  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  size_t maxIterations = 8;
  size_t relaySize = Supla::GUI::relay.size();

  for (size_t i = 0; i < relaySize && i < maxIterations; i++) {
    if (Supla::GUI::relay[i] != nullptr) {
      if (Supla::GUI::relay[i]->isOn()) {
        display->setColor(WHITE);
        display->fillRect(x, y + 1, 10, 10);
        display->setColor(BLACK);
        display->drawString(x + 2, y, String(i + 1));
      }
      else {
        display->setColor(WHITE);
        display->drawString(x + 2, y, String(i + 1));
      }
      x += 15;
    }
    else {
      maxIterations++;
    }
  }
  // display->setColor(WHITE);
  // display->drawHorizontalLine(0, 14, display->getWidth());
}
#endif

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  displayUiSuplaClock(display);
  displayUiSignal(display);

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  if (Supla::GUI::relay.size()) {
    displayUiRelayState(display);
  }
#endif
}

void displayUiSuplaStatus(OLEDDisplay* display) {
  int x = 0;
  int y = display->getHeight() / 3;
  display->clear();

  displayUiSignal(display);

  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawStringMaxWidth(x, y, display->getWidth(), ConfigESP->supla_status.msg);
  display->display();
}

void displayUiSuplaClock(OLEDDisplay* display) {
  if (display->getWidth() > 64) {
    char clockBuff[6];
    auto suplaClock = SuplaDevice.getClock();

    if (suplaClock->isReady()) {
      sprintf_P(clockBuff, PSTR("%02d:%02d"), suplaClock->getHour(), suplaClock->getMin());
      display->setColor(WHITE);
      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(0, display->getHeight() - 10, String(clockBuff));
    }
  }
}

void displayConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setFont(ArialMT_Win1250_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, S_CONFIGURATION_MODE);
  display->drawString(0, 28, S_AP_NAME);
  display->drawString(0, 41, ConfigESP->getConfigNameAP());
  display->drawString(0, 54, S_IP_AP);
  display->display();
}

void displayUiBlank(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Win1250_Plain_16);
  display->drawString(10, display->getHeight() / 2, F("SUPLA"));
}

void displayUiGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double value, const String& unit, const uint8_t* xbm) {
  displayUiGeneral(display, state, x, y, String(value), unit, xbm);
}

void displayUiGeneral(
    OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, const String& value, const String& unit, const uint8_t* xbm) {
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (xbm != NULL && display->getWidth() > 64) {
    int drawHeightIcon = display->getHeight() / 2 - 10;
    display->drawXbm(x + 0, y + drawHeightIcon, 32, 32, xbm);
  }

  String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
  if (!name.isEmpty()) {
    display->setFont(ArialMT_Win1250_Plain_10);
    display->drawString(x + getWidthValue(display, name), y + display->getHeight() / 2 - 12, name);
  }

  display->setFont(ArialMT_Win1250_Plain_24);
  display->drawString(x + getWidthValue(display, value), y + display->getHeight() / 2 - 2, value);

  if (!unit.isEmpty()) {
    uint8_t widthUnit = getWidthUnit(display, value);

    display->setFont(ArialMT_Win1250_Plain_16);
    display->drawString(x + widthUnit, y + display->getHeight() / 2 + 5, unit);
  }
}

void displayTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);
  double lastTemperature = getTemperatureFromChannelThermometr(channel);

  displayUiGeneral(display, state, x, y, getTempString(lastTemperature), "°C", temp_bits);
}

void displayDoubleHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel && channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
    double lastHumidit = channel->getValueDoubleSecond();

    displayUiGeneral(display, state, x, y, getHumidityString(lastHumidit), "%", humidity_bits);
  }
}

void displayPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel && channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
    double lastPressure = channel->getValueDouble();

    displayUiGeneral(display, state, x, y, getPressureString(lastPressure), "hPa", pressure_bits);
  }
}

void displayDistance(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel && channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
    double distance = channel->getValueDouble();

    displayUiGeneral(display, state, x, y, getDistanceString(distance), "m", distance_bits);
  }
}

void displayGeneral(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel) {
    if (oled[state->currentFrame].forSecondaryValue) {
      displayUiGeneral(display, state, x, y, channel->getValueDoubleSecond());
    }
    else {
      displayUiGeneral(display, state, x, y, channel->getValueDouble());
      channel->getValueDouble();
    }
  }
}

void displayEnergyVoltage(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel) {
    TSuplaChannelExtendedValue* extValue = channel->getExtValue();
    if (extValue == nullptr)
      return;

    TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
    if (emValue->m_count < 1 || emValue == nullptr)
      return;

    _supla_int_t flags = channel->getFlags();
    double averageVoltage;

    if (flags & (SUPLA_CHANNEL_FLAG_PHASE2_UNSUPPORTED | SUPLA_CHANNEL_FLAG_PHASE3_UNSUPPORTED)) {
      averageVoltage = (emValue->m[0].voltage[0]);
    }
    else {
      averageVoltage = (emValue->m[0].voltage[0] + emValue->m[0].voltage[1] + emValue->m[0].voltage[2]) / 3;
    }

    displayUiGeneral(display, state, x, y, String(averageVoltage / 100.0, 0), "V");
  }
}

void displayEnergyCurrent(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel) {
    TSuplaChannelExtendedValue* extValue = channel->getExtValue();
    if (extValue == nullptr)
      return;

    TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
    if (emValue->m_count < 1 || emValue == nullptr)
      return;

    double sumCurrent = emValue->m[0].current[0] + emValue->m[0].current[1] + emValue->m[0].current[2];
    displayUiGeneral(display, state, x, y, String(sumCurrent / 1000.0, 1), "A");
  }
}

void displayEnergyPowerActive(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  auto channel = getChanelByChannelNumber(oled[state->currentFrame].chanelSensor);

  if (channel) {
    TSuplaChannelExtendedValue* extValue = channel->getExtValue();
    if (extValue == nullptr)
      return;

    TElectricityMeter_ExtendedValue_V2* emValue = reinterpret_cast<TElectricityMeter_ExtendedValue_V2*>(extValue->value);
    if (emValue->m_count < 1 || emValue == nullptr)
      return;

    double sumPowerActive = emValue->m[0].power_active[0] + emValue->m[0].power_active[1] + emValue->m[0].power_active[2];
    displayUiGeneral(display, state, x, y, String(sumPowerActive / 100000.0, 1), "W");
  }
}

void displayThermostat(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  uint8_t mainThermometr = 0;
  uint8_t auxThermometr = 0;
  uint8_t thermostatIndex = oled[state->currentFrame].nrRealy;
  uint8_t channelSensor = oled[state->currentFrame].chanelSensor;
  int8_t shiftWhenAddedRelay = 0;
  int centerYPosition = y + display->getHeight() / 2 - 8;

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  if (getCountActiveThermostat() < Supla::GUI::relay.size()) {
    shiftWhenAddedRelay = 12;
  }
#endif

  auto channel = getChanelByChannelNumber(channelSensor);

  if (channel && thermostatIndex >= 0) {
    mainThermometr = ConfigManager->get(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL)->getElement(thermostatIndex).toInt();
    auxThermometr = ConfigManager->get(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL)->getElement(thermostatIndex).toInt();

    auto channelMainThermometr = getChanelByChannelNumber(mainThermometr);
    double mainTemperature = getTemperatureFromChannelThermometr(channelMainThermometr);

    double setpointTemperature = 0;
    uint8_t thermostatType = ConfigManager->get(KEY_THERMOSTAT_TYPE)->getElement(thermostatIndex).toInt();
    if (thermostatType == Supla::GUI::THERMOSTAT_COOL) {
      setpointTemperature = channel->getHvacSetpointTemperatureCool() / 100.0;
    }
    else {
      setpointTemperature = channel->getHvacSetpointTemperatureHeat() / 100.0;
    }

    display->setColor(WHITE);
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    if (auxThermometr != channelSensor) {
      auto channelAuxThermometr = getChanelByChannelNumber(auxThermometr);
      double auxTemperature = getTemperatureFromChannelThermometr(channelAuxThermometr);

      display->setFont(ArialMT_Win1250_Plain_24);
      display->drawString(x + 15, centerYPosition, getTempString(mainTemperature));
      display->drawString((x + 14) + display->getWidth() / 2, centerYPosition, getTempString(auxTemperature));
    }
    else {
      display->setFont(ArialMT_Win1250_Plain_24);
      display->drawString(x + getWidthValue(display, getTempString(mainTemperature)) - 6, centerYPosition,
                          getTempString(mainTemperature) + S_CELSIUS);
    }

    if (channel->getHvacMode() != SUPLA_HVAC_MODE_OFF) {
      if (channel->getHvacIsOn()) {
        display->setColor(WHITE);
        int16_t x0 = 0, y0 = 10;
        int16_t x1 = 10, y1 = 10;
        int16_t x2 = 5, y2 = 5;

        if (thermostatType == Supla::GUI::THERMOSTAT_COOL) {
          y0 = 0;
          y1 = 0;
        }

        y0 += shiftWhenAddedRelay + 13;
        y1 += shiftWhenAddedRelay + 13;
        y2 += shiftWhenAddedRelay + 13;

        display->fillTriangle(x0, y0, x1, y1, x2, y2);
      }

      if (channel->isHvacFlagWeeklySchedule()) {
        display->setFont(ArialMT_Plain_10);
        display->drawString(display->getWidth() / 2 + 20, 0 + shiftWhenAddedRelay, String("P"));
      }
      else {
        display->setFont(ArialMT_Plain_10);
        display->drawString(display->getWidth() / 2 + 20, 0 + shiftWhenAddedRelay, String("M"));
      }

      display->setFont(ArialMT_Win1250_Plain_10);
      display->drawString(display->getWidth() - 47, display->getHeight() - 10, String("set"));

      display->setFont(ArialMT_Plain_16);
      display->drawString(display->getWidth() - 30, display->getHeight() - 15, getTempString(setpointTemperature).c_str());
    }

    String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
    if (!name.isEmpty()) {
      display->setFont(ArialMT_Win1250_Plain_10);
      display->drawString(0, 0 + shiftWhenAddedRelay, name);
    }
  }
}

Supla::Channel* getChanelByChannelNumber(int channelNumber) {
  Supla::Channel* foundChannel = nullptr;

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      Supla::Channel* currentChannel = element->getChannel();

      if (currentChannel->getChannelNumber() == channelNumber) {
        foundChannel = currentChannel;
        break;
      }
    }

    if (element->getSecondaryChannel()) {
      Supla::Channel* currentChannel = element->getSecondaryChannel();

      if (currentChannel->getChannelNumber() == channelNumber) {
        foundChannel = currentChannel;
        break;
      }
    }
  }

  return foundChannel;
}

double getTemperatureFromChannelThermometr(Supla::Channel* channelThermometr) {
  if (channelThermometr) {
    if (channelThermometr->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
      return channelThermometr->getValueDouble();
    }
    else if (channelThermometr->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
      return channelThermometr->getValueDoubleFirst();
    }
  }
  return TEMPERATURE_NOT_AVAILABLE;
}

SuplaOled::SuplaOled() {
}

void SuplaOled::onInit() {
  if ((ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) ||
      (ConfigESP->getGpio(FUNCTION_SDA_2) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL_2) != OFF_GPIO)) {
    HW_I2C i2cBus = (ConfigESP->getGpio(FUNCTION_SDA_2) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL_2) != OFF_GPIO) ? I2C_TWO : I2C_ONE;

    switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      case OLED_SSD1306_0_96:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64, i2cBus);
        break;
      case OLED_SH1106_1_3:
        display = new SH1106Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64, i2cBus);
        break;
      case OLED_SSD1306_0_66:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_64_48, i2cBus);
        break;
    }

    SuplaDevice.addClock(new Supla::Clock);

    overlays[0] = {msOverlay};
    int maxFrame = getCountSensorChannels() + getCountActiveThermostat();

    if (maxFrame == 0) {
      maxFrame = 1;
    }

    ui = new OLEDDisplayUi(display);
    frames = new FrameCallback[maxFrame];
    oled = new oledStruct[maxFrame];

    uint8_t nr = 0;
    for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();

        if (getCountActiveThermostat() != 0) {
          if (channel->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
            nr++;
          }

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_HVAC) {
            frames[getFrameCount()] = {displayThermostat};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].nrRealy = nr;
            nr++;

            setFrameCount(getFrameCount() + 1);
          }
        }
        else {
          auto channel = element->getChannel();

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
            frames[getFrameCount()] = {displayTemperature};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            setFrameCount(getFrameCount() + 1);
          }

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
            frames[getFrameCount()] = {displayTemperature};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            setFrameCount(getFrameCount() + 1);
            frames[getFrameCount()] = {displayDoubleHumidity};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            setFrameCount(getFrameCount() + 1);
          }

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
            frames[getFrameCount()] = {displayGeneral};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].forSecondaryValue = true;
            setFrameCount(getFrameCount() + 1);
          }

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
            frames[getFrameCount()] = {displayDistance};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].forSecondaryValue = false;
            setFrameCount(getFrameCount() + 1);
          }

          if (channel->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
            frames[getFrameCount()] = {displayEnergyVoltage};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].forSecondaryValue = false;
            setFrameCount(getFrameCount() + 1);

            frames[getFrameCount()] = {displayEnergyCurrent};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].forSecondaryValue = false;
            setFrameCount(getFrameCount() + 1);

            frames[getFrameCount()] = {displayEnergyPowerActive};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            oled[getFrameCount()].forSecondaryValue = false;
            setFrameCount(getFrameCount() + 1);
          }
          if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
            frames[getFrameCount()] = {displayPressure};
            oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
            setFrameCount(getFrameCount() + 1);
          }

          if (element->getSecondaryChannel()) {
            auto channel = element->getSecondaryChannel();
            if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
              frames[getFrameCount()] = {displayPressure};
              oled[getFrameCount()].chanelSensor = channel->getChannelNumber();
              setFrameCount(getFrameCount() + 1);
            }
          }
        }
      }
    }

    if (getFrameCount() == 0) {
      frames[getFrameCount()] = {displayUiBlank};
      setFrameCount(getFrameCount() + 1);
    }

    setupAnimate();

    ui->setTargetFPS(60);
    ui->setFrameAnimation(SLIDE_LEFT);

    ui->setFrames(frames, getFrameCount());
    ui->setOverlays(overlays, overlaysCount);
    ui->init();

    if (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
      display->setBrightness(255);
      timeLastChangeOled = millis();
    }
    else {
      display->setBrightness((ConfigManager->get(KEY_OLED_BACK_LIGHT)->getValueInt() / 100.0) * 255);
    }

    display->flipScreenVertically();
    display->setFontTableLookupFunction(&utf8win1250);
  }

  enableDisplay(true);
}

void SuplaOled::setupAnimate() {
  if (getFrameCount() == 1) {
    ui->disableAllIndicators();
    ui->disableAutoTransition();
  }
  else if (getFrameCount() <= 6) {
    ui->setIndicatorPosition(BOTTOM);
    ui->setIndicatorDirection(LEFT_RIGHT);

    if (ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() > 0) {
      ui->enableAutoTransition();
      ui->setTimePerFrame(ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() * 1000);
    }
    else {
      ui->disableAutoTransition();
      ui->setTimePerTransition(250);
    }
  }
  else {
    ui->disableAllIndicators();
  }
}

void SuplaOled::iterateAlways() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    if (ConfigESP->configModeESP == Supla::DEVICE_MODE_CONFIG) {
      displayConfigMode(display);
      return;
    }

    if (ConfigESP->getLastStatusSupla() == STATUS_REGISTERED_AND_READY || ConfigESP->getLastStatusSupla() == STATUS_NETWORK_DISCONNECTED ||
        ConfigESP->getLastStatusSupla() == STATUS_INITIALIZED || ConfigESP->getLastStatusSupla() == STATUS_REGISTER_IN_PROGRESS) {
      // setupAnimate();

      if (millis() - timeLastChangeOled > (unsigned long)(ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() * 1000) &&
          this->isDisplayEnabled() && ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
        this->enableDisplay(false);

        if (getFrameCount() > 1 && ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() > 0) {
          ui->enableAutoTransition();
          ui->setTimePerFrame(ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() * 1000);
        }
      }

      int remainingTimeBudget = ui->update();

      if (remainingTimeBudget > 0)
        delay(remainingTimeBudget);
    }
    else {
      displayUiSuplaStatus(display);
    }
  }
}

void SuplaOled::handleAction(int event, int action) {
  if (action == OLED_NEXT_FRAME) {
    ui->nextFrame();
  }
}

void SuplaOled::enableDisplay(bool isOn) {
  if (!oledON && isOn && ConfigESP->getBrightnessLevelOLED() != 100) {
    display->setBrightness(255);
  }

  if (isOn) {
    ui->disableAutoTransition();
  }
  else {
    int brightnessLevel = ConfigManager->get(KEY_OLED_BACK_LIGHT)->getValueInt();

    if (brightnessLevel != 100) {
      display->setBrightness((brightnessLevel / 100.0) * 255);
    }
  }

  oledON = isOn;
  timeLastChangeOled = millis();
}

// In ESP8266 Arduino core v2.3.0 missing bsearch: https://github.com/esp8266/Arduino/issues/2314
// Part of GNU C Library
void* gnu_c_bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
  size_t l, u, idx;
  const void* p;
  int comparison;

  l = 0;
  u = nmemb;
  while (l < u) {
    idx = (l + u) / 2;

    p = (void*)(((const char*)base) + (idx * size));
    comparison = (*compar)(key, p);
    if (comparison < 0)
      u = idx;
    else if (comparison > 0)
      l = idx + 1;
    else
      return (void*)p;
  }

  return NULL;
}

// compare function for bsearch
int charset_table_cmp(const void* p_key, const void* p_item) {
  const uint16_t key = *(uint16_t*)p_key;
  const uint16_t item = (*((char*)p_item)) << 8 | *(((char*)p_item) + 1);

  if (key < item)
    return -1;
  else if (key == item)
    return 0;
  else
    return 1;
}

// convert utf-8 character to windows-1250
// if utf-8 char continue with next byte, returns 0, otherwise windows-1250 char
// for unconvertable char returns 0
char utf8win1250(const uint8_t ch) {
  static uint16_t uChar;
  static uint8_t len;
  char* found;

  if ((ch & 0x80) == 0x00) {
    uChar = len = 0;
    return ch;
  }
  else if ((ch & 0xE0) == 0xC0) {
    uChar = ch & 0x1F;
    len = 1;
    return 0;
  }
  else if ((ch & 0xF0) == 0xE0) {
    uChar = ch & 0x0F;
    len = 2;
    return 0;
  }
  else if ((ch & 0xF8) == 0xF0) {
    uChar = ch & 0x07;
    len = 3;
    return 0;
  }
  else if ((ch & 0xC0) == 0x80 && len > 0) {
    uChar = (uChar << 6) | (ch & 0x7F);
    len--;
    if (len > 0)
      return 0;
  }
  else {
    uChar = len = 0;
    return 0;
  }

  found = (char*)gnu_c_bsearch(&uChar, utf8_win1250_table + 1, utf8_win1250_table[0], 3 * sizeof(char), charset_table_cmp);

  if (found != NULL) {
    uChar = len = 0;
    return *(found + 2);  // return win1250 char at 3rd position;
  }

  return 0;
}

#endif
