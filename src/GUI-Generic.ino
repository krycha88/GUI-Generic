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
#include "SuplaDeviceGUI.h"

#include <supla/control/MCP23017/supla_mcp23017.h>

#define DRD_TIMEOUT 5  // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0  // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
  Serial.begin(74880);

  if (drd.detectDoubleReset()) {
    drd.stop();
    ConfigESP->factoryReset();
  }

  uint8_t nr, gpio;

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  uint8_t rollershutters = ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt();

  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
#ifdef SUPLA_ROLLERSHUTTER
      if (rollershutters > 0) {
#ifdef SUPLA_BUTTON
        if (ConfigESP->getLevel(nr, FUNCTION_BUTTON) == Supla::ON_CHANGE && ConfigESP->getLevel(nr + 1, FUNCTION_BUTTON) == Supla::ON_CHANGE) {
          Supla::GUI::addRolleShutterMomentary(nr);
        }
        else {
#endif
          Supla::GUI::addRolleShutter(nr);
#ifdef SUPLA_BUTTON
        }
#endif
        rollershutters--;
        nr++;
      }
      else {
#endif
        Supla::GUI::addRelayButton(nr);
#ifdef SUPLA_ROLLERSHUTTER
      }
#endif
    }
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH) != OFF_GPIO) {
      new Supla::Sensor::Binary(ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH), true);
    }
  }
#endif

#ifdef SUPLA_CONFIG
  Supla::GUI::addConfigESP(ConfigESP->getGpio(FUNCTION_CFG_BUTTON), ConfigESP->getGpio(FUNCTION_CFG_LED),
                           ConfigManager->get(KEY_CFG_MODE)->getValueInt(), ConfigESP->getLevel(FUNCTION_CFG_LED));
#endif

#ifdef SUPLA_DS18B20
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    Supla::GUI::addDS18B20MultiThermometer(ConfigESP->getGpio(FUNCTION_DS18B20));
  }
#endif

#ifdef SUPLA_DHT11
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT11) != OFF_GPIO) {
      auto dht11 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT11), DHT11);

      if (nr == 1) {
        Supla::GUI::addConditionsTurnON(SENSOR_DHT11, dht11);
        Supla::GUI::addConditionsTurnOFF(SENSOR_DHT11, dht11);
      }
    }
  }
#endif

#ifdef SUPLA_DHT22
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT22) != OFF_GPIO) {
      auto dht22 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT22), DHT22);

      if (nr == 1) {
        Supla::GUI::addConditionsTurnON(SENSOR_DHT22, dht22);
        Supla::GUI::addConditionsTurnOFF(SENSOR_DHT22, dht22);
      }
    }
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) != OFF_GPIO) {
    auto si7021sonoff = new Supla::Sensor::Si7021Sonoff(ConfigESP->getGpio(FUNCTION_SI7021_SONOFF));
    Supla::GUI::addConditionsTurnON(SENSOR_SI7021_SONOFF, si7021sonoff);
    Supla::GUI::addConditionsTurnOFF(SENSOR_SI7021_SONOFF, si7021sonoff);
  }
#endif

#ifdef SUPLA_HC_SR04
  if (ConfigESP->getGpio(FUNCTION_TRIG) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_ECHO) != OFF_GPIO) {
    Supla::Sensor::HC_SR04 *hcsr04;
    if (ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt() > 0) {
      hcsr04 = new Supla::Sensor::HC_SR04(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO), 0,
                                          ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(),
                                          ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(), 0);
    }
    else {
      hcsr04 = new Supla::Sensor::HC_SR04(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO));
    }

    Supla::GUI::addConditionsTurnON(SENSOR_HC_SR04, hcsr04);
    Supla::GUI::addConditionsTurnOFF(SENSOR_HC_SR04, hcsr04);
  }
#endif

#ifdef SUPLA_MAX6675
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    auto thermocouple =
        new Supla::Sensor::MAXThermocouple(ConfigESP->getGpio(FUNCTION_CLK), ConfigESP->getGpio(FUNCTION_CS), ConfigESP->getGpio(FUNCTION_D0));
    Supla::GUI::addConditionsTurnON(SENSOR_MAX6675, thermocouple);
    Supla::GUI::addConditionsTurnOFF(SENSOR_MAX6675, thermocouple);
  }
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  if (ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt() > 0) {
    for (nr = 1; nr <= ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
      if (ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER) != OFF_GPIO) {
        Supla::GUI::addImpulseCounter(ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER), ConfigESP->getLevel(nr, FUNCTION_IMPULSE_COUNTER),
                                      ConfigESP->getMemory(nr, FUNCTION_IMPULSE_COUNTER),
                                      ConfigManager->get(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT)->getValueInt());
      }
    }
  }

#endif

#ifdef SUPLA_HLW8012
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
  }
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x) || defined(SUPLA_HTU21D) || defined(SUPLA_SHT71) || \
    defined(SUPLA_BH1750) || defined(SUPLA_MAX44009) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017)
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    Wire.begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));

#ifdef SUPLA_BME280
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
      Supla::Sensor::BME280 *bme280;
      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
        case BME280_ADDRESS_0X76:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
          break;
        case BME280_ADDRESS_0X77:
          bme280 = new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
          break;
        case BME280_ADDRESS_0X76_AND_0X77:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
          new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BME280)->getValueInt());
          break;
      }
      Supla::GUI::addConditionsTurnON(SENSOR_BME280, bme280);
      Supla::GUI::addConditionsTurnOFF(SENSOR_BME280, bme280);
    }
#endif

#ifdef SUPLA_SHT3x

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
      Supla::Sensor::SHT3x *sht3x;
      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
        case SHT3x_ADDRESS_0X44:
          sht3x = new Supla::Sensor::SHT3x(0x44);
          break;
        case SHT3x_ADDRESS_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x45);
          break;
        case SHT3x_ADDRESS_0X44_AND_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x44);
          new Supla::Sensor::SHT3x(0x45);
          break;
      }
      Supla::GUI::addConditionsTurnON(SENSOR_SHT3x, sht3x);
      Supla::GUI::addConditionsTurnOFF(SENSOR_SHT3x, sht3x);
    }
#endif

#ifdef SUPLA_SI7021
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt()) {
      auto si7021 = new Supla::Sensor::Si7021();
      Supla::GUI::addConditionsTurnON(SENSOR_SI7021, si7021);
      Supla::GUI::addConditionsTurnOFF(SENSOR_SI7021, si7021);
    }
#endif

#ifdef SUPLA_MCP23017
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt()) {
      if (!mcp1.begin(0))
        Serial.println(F("MCP23017 1 not found!"));  // begin(uint8_t address)  "Pin 100 - 115"
      if (!mcp2.begin(1))
        Serial.println(F("MCP23017 2 not found!"));  // begin(uint8_t address)  "Pin 116 - 131"
      if (!mcp3.begin(2))
        Serial.println(F("MCP23017 3 not found!"));  // begin(uint8_t address)  "Pin 132 - 147"
      if (!mcp4.begin(3))
        Serial.println(F("MCP23017 4 not found!"));  // begin(uint8_t address)  "Pin 148 - 163"
    }
#endif

#ifdef SUPLA_OLED
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      SuplaOled *oled = new SuplaOled();
      oled->addButtonOled(ConfigESP->getGpio(FUNCTION_CFG_BUTTON));
    }
#endif
  }
#endif

#ifdef SUPLA_RGBW
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    Supla::GUI::addRGBWLeds(nr);
  }
#endif

  Supla::GUI::begin();
}

void loop() {
  SuplaDevice.iterate();
  delay(25);
  drd.loop();
}
