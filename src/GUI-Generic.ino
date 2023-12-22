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

#ifdef SUPLA_PZEM_V_3
#include "src/sensor/PzemV3.h"
#include "src/sensor/three_phase_PzemV3.h"
#endif

#ifdef SUPLA_MPX_5XXX
#include <supla/sensor/percentage.h>
#endif

#ifdef ARDUINO_ARCH_ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
#include <supla/sensor/direct_link_sensor_thermometer.h>
#endif

#ifdef SUPLA_DIRECT_LINKS_MULTI_SENSOR
#include "src/sensor/DirectLinks.h"
#endif

#ifdef SUPLA_CC1101
#include "src/sensor/WmbusMeter.h"
#endif

#include "src/boneIO/boneIO.h"
#include "src/display/OledButtonController.h"

uint32_t last_loop{0};
#define LOOP_INTERVAL 16

void setup() {
  uint8_t nr, gpio;

  Serial.begin(74880);
  eeprom.setStateSavePeriod(5000);

  ConfigManager = new SuplaConfigManager();
  ConfigESP = new SuplaConfigESP();

  ImprovSerialComponent *improvSerialComponent = new ImprovSerialComponent();
  improvSerialComponent->enable();

#if defined(GUI_SENSOR_SPI) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_OTHER)
  ThermHygroMeterCorrectionHandler &correctionHandler = ThermHygroMeterCorrectionHandler::getInstance();
#endif

#ifdef SUPLA_BONEIO
  new Supla::boneIO();
#endif

#ifdef GUI_SENSOR_I2C_EXPENDER
  Expander = new Supla::Control::ConfigExpander();
#endif

#ifdef SUPLA_CONDITIONS
  Supla::GUI::Conditions::addConditionsElement();
#endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_ENERGY_METER)
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    bool force400khz = false;

    Wire.begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));
  }

#ifdef ARDUINO_ARCH_ESP32
  if (ConfigESP->getGpio(FUNCTION_SDA_2) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL_2) != OFF_GPIO) {
    Wire1.begin(ConfigESP->getGpio(FUNCTION_SDA_2), ConfigESP->getGpio(FUNCTION_SCL_2));
    Wire1.setClock(100000);
  }
#endif
#endif

#ifdef SUPLA_ACTION_TRIGGER
  Supla::GUI::actionTrigger = new Supla::GUI::ActionTrigger[Supla::GUI::calculateElementCountActionTrigger()];
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
  uint8_t rollershutters = ConfigManager->get(KEY_MAX_ROLLERSHUTTER)->getValueInt();

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RELAY)->getValueInt(); nr++) {
    if (rollershutters > 0) {
#ifdef SUPLA_ROLLERSHUTTER
      Supla::GUI::addRolleShutter(nr);
#endif
      rollershutters--;
      nr++;
    }
    else {
#ifdef SUPLA_RF_BRIDGE
      if (ConfigESP->getGpio(FUNCTION_RF_BRIDGE_TRANSMITTER) != OFF_GPIO &&
          ConfigManager->get(KEY_RF_BRIDGE_TYPE)->getElement(nr).toInt() == Supla::GUI::RFBridgeType::TRANSMITTER &&
          (strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).c_str(), "") != 0 ||
           strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).c_str(), "") != 0)) {
        Supla::GUI::addRelayBridge(nr);
      }
      else {
        Supla::GUI::addRelayOrThermostat(nr);
      }
#else
      Supla::GUI::addRelayOrThermostat(nr);
#endif

      if (ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
#ifdef SUPLA_RF_BRIDGE
        if (ConfigManager->get(KEY_RF_BRIDGE_TYPE)->getElement(nr).toInt() == Supla::GUI::RFBridgeType::RECEIVER &&
            (strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).c_str(), "") != 0 ||
             strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).c_str(), "") != 0)) {
          Supla::GUI::addButtonBridge(nr);
        }
#endif

#ifdef SUPLA_DIRECT_LINKS
        Supla::GUI::addDirectLinks(nr);
#endif
      }
    }
    delay(0);
  }
#endif

#ifdef SUPLA_LIMIT_SWITCH
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt(); nr++) {
    gpio = ConfigESP->getGpio(nr, FUNCTION_LIMIT_SWITCH);

    Supla::Sensor::Binary *binary = nullptr;

    if (gpio != OFF_GPIO) {
      binary = Supla::Control::GUI::Binary(gpio, ConfigESP->getPullUp(gpio), false, nr);
    }

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_BINARY, S_LIMIT_SWITCH, binary, nr);
#endif
  }
#endif

#ifdef SUPLA_PUSHOVER
  for (uint8_t nr = 0; nr < MAX_PUSHOVER_MESSAGE; nr++) {
    Supla::GUI::addPushover(nr);
  }
#endif

#ifdef SUPLA_WAKE_ON_LAN
  for (nr = 0; nr < ConfigManager->get(KEY_WAKE_ON_LAN_MAX)->getValueInt(); nr++) {
    if (strcmp(ConfigManager->get(KEY_WAKE_ON_LAN_MAC)->getElement(nr).c_str(), "") != 0) {
      new Supla::Control::WakeOnLanRelay(ConfigManager->get(KEY_WAKE_ON_LAN_MAC)->getElement(nr).c_str());
    }
  }
#endif

#ifdef SUPLA_CONFIG
  Supla::GUI::addConfigESP(ConfigESP->getGpio(FUNCTION_CFG_BUTTON), ConfigESP->getGpio(FUNCTION_CFG_LED));
#endif

#ifdef SUPLA_DS18B20
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    Supla::GUI::addDS18B20MultiThermometer(ConfigESP->getGpio(FUNCTION_DS18B20));
  }
#endif

#ifdef SUPLA_DIRECT_LINKS_MULTI_SENSOR
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DIRECT_LINKS_SENSOR)->getValueInt(); nr++) {
    if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(), "") != 0) {
      switch (ConfigManager->get(KEY_DIRECT_LINKS_TYPE)->getElement(nr).toInt()) {
        case DIRECT_LINKS_TYPE_TEMP: {
          auto directLinkSensorThermometer = new Supla::Sensor::DirectLinksThermometer(
              ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(), ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR, S_DIRECT_LINKS_SENSOR_THERMOMETR,
                                                      directLinkSensorThermometer, nr);
#endif
          break;
        }
        case DIRECT_LINKS_TYPE_TEMP_HYGR:
          new Supla::Sensor::DirectLinksThermHygroMeter(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                                        ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;

        case DIRECT_LINKS_TYPE_PRESS:
          new Supla::Sensor::DirectLinksPressMeter(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                                   ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;

        case DIRECT_LINKS_TYPE_ONE_PHASE_ELECTRICITY_METER:
          new Supla::Sensor::DirectLinksOnePhaseElectricityMeter(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                                                 ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;

        case DIRECT_LINKS_TYPE_ELECTRICITY_METER:
          new Supla::Sensor::DirectLinksElectricityMeter(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                                         ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;

        case DIRECT_LINKS_TYPE_DISTANCE:
          new Supla::Sensor::DirectLinksDistance(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                                 ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;

        case DIRECT_LINKS_TYPE_DEPTH:
          new Supla::Sensor::DirectLinksDepth(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(),
                                              ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
          break;
      }
    }
  }
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DIRECT_LINKS_SENSOR)->getValueInt(); nr++) {
    if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str(), "") != 0) {
      auto directLinkSensorThermometer = new Supla::Sensor::DirectLinksSensorThermometer(ConfigManager->get(KEY_SUPLA_SERVER)->getValue());
      directLinkSensorThermometer->setUrl(ConfigManager->get(KEY_DIRECT_LINKS_SENSOR)->getElement(nr).c_str());

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR, S_DIRECT_LINKS_SENSOR_THERMOMETR,
                                                  directLinkSensorThermometer, nr);
#endif
    }
  }
#endif

#ifdef SUPLA_DHT11
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT11) != OFF_GPIO) {
      auto dht11 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT11), DHT11);
      correctionHandler.addThermHygroMeter(dht11);

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_DHT11, S_DHT11, dht11, nr);
#endif
    }
  }
#endif

#ifdef SUPLA_DHT22
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_DHT22) != OFF_GPIO) {
      auto dht22 = new Supla::Sensor::DHT(ConfigESP->getGpio(nr, FUNCTION_DHT22), DHT22);
      correctionHandler.addThermHygroMeter(dht22);

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_DHT22, S_DHT22, dht22, nr);
#endif
    }
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (ConfigESP->getGpio(FUNCTION_SI7021_SONOFF) != OFF_GPIO) {
    auto si7021sonoff = new Supla::Sensor::Si7021Sonoff(ConfigESP->getGpio(FUNCTION_SI7021_SONOFF));
    correctionHandler.addThermHygroMeter(si7021sonoff);
    improvSerialComponent->disable();

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_SI7021_SONOFF, S_SI7021_SONOFF, si7021sonoff);
#endif
  }
#endif

#ifdef SUPLA_HC_SR04
  if (ConfigESP->getGpio(FUNCTION_TRIG) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_ECHO) != OFF_GPIO) {
    Supla::Sensor::HC_SR04_NewPing *hcsr04;
    if (ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt() > 0) {
      hcsr04 = new Supla::Sensor::HC_SR04_NewPing(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO), 0,
                                                  ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(),
                                                  ConfigManager->get(KEY_HC_SR04_MAX_SENSOR_READ)->getValueInt(), 0);
    }
    else {
      hcsr04 = new Supla::Sensor::HC_SR04_NewPing(ConfigESP->getGpio(FUNCTION_TRIG), ConfigESP->getGpio(FUNCTION_ECHO));
    }

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_HC_SR04, S_HC_SR04, hcsr04);
#endif
  }
#endif

#ifdef GUI_SENSOR_SPI
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_MISO) != OFF_GPIO) {
#ifdef SUPLA_MAX6675
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX6675).toInt()) {
      auto thermocouple =
          new Supla::Sensor::MAX6675_K(ConfigESP->getGpio(FUNCTION_CLK), ConfigESP->getGpio(FUNCTION_CS), ConfigESP->getGpio(FUNCTION_MISO));
#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_MAX6675, S_MAX6675, thermocouple);
#endif
    }
#endif

#ifdef SUPLA_MAX31855
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX31855).toInt()) {
      auto thermocouple =
          new Supla::Sensor::MAX31855(ConfigESP->getGpio(FUNCTION_CLK), ConfigESP->getGpio(FUNCTION_CS), ConfigESP->getGpio(FUNCTION_MISO));
#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_MAX31855, S_MAX31855, thermocouple);
#endif
    }
#endif

#ifdef SUPLA_CC1101
    if (ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_SPI_CC1101).toInt()) {
      improvSerialComponent->disable();

      int indexOfSensorType = ConfigManager->get(KEY_WMBUS_SENSOR_TYPE)->getElement(WMBUS_CFG_SENSOR_TYPE).toInt();
      Serial.print("Index of sensor type:");
      Serial.println(indexOfSensorType);

      std::string sensorType = sensors_types[indexOfSensorType];
      Serial.print("Sensor type:");
      Serial.println(sensorType.c_str());

      std::string sensorId = ConfigManager->get(KEY_WMBUS_SENSOR_ID)->getValue();
      std::string sensorKey = ConfigManager->get(KEY_WMBUS_SENSOR_KEY)->getValue();
      Serial.print("Sensor id:");
      Serial.println(sensorId.c_str());

      std::vector<unsigned char> key;
      if (sensorKey.length() > 0) {
        key = std::vector<unsigned char>(sensorKey.begin(), sensorKey.end());
      }
      else {
        key = {};
        key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      }

      int mosi = ConfigESP->getGpio(FUNCTION_MOSI);
      int miso = ConfigESP->getGpio(FUNCTION_MISO);
      int clk = ConfigESP->getGpio(FUNCTION_CLK);
      int cs = ConfigESP->getGpio(FUNCTION_CS);
      int gdo0 = ConfigESP->getGpio(FUNCTION_GDO0);
      int gdo2 = ConfigESP->getGpio(FUNCTION_GDO2);
      Serial.print("GPIO: ");
      Serial.print(mosi);
      Serial.print(",");
      Serial.print(miso);
      Serial.print(",");
      Serial.print(clk);
      Serial.print(",");
      Serial.print(cs);
      Serial.print(",");
      Serial.print(gdo0);
      Serial.print(",");
      Serial.print(gdo2);
      Serial.println("GPIO END");

      meter = new Supla::Sensor::WmbusMeter(mosi, miso, clk, cs, gdo0, gdo2);
      attachInterrupt(gdo0, Supla::Sensor::WmbusMeterIterateAlways, RISING);

      meter->add_sensor(new Supla::Sensor::SensorInfo(sensorId, sensorType, "total_water_m3", key));
      meter->add_driver(new Amiplus());
      meter->add_driver(new Apator08());
      meter->add_driver(new Apator162());
      meter->add_driver(new ApatorEITN());
      meter->add_driver(new Bmeters());
      meter->add_driver(new C5isf());
      meter->add_driver(new Compact5());
      meter->add_driver(new Dme07());
      meter->add_driver(new Elf());
      meter->add_driver(new Evo868());
      meter->add_driver(new FhkvdataIII());
      meter->add_driver(new Hydrocalm3());
      meter->add_driver(new Hydrus());
      meter->add_driver(new Iperl());
      meter->add_driver(new Itron());
      meter->add_driver(new Izar());
      meter->add_driver(new Mkradio3());
      meter->add_driver(new Mkradio4());
      meter->add_driver(new Qheat());
      meter->add_driver(new Qwater());
      meter->add_driver(new Sharky774());
      meter->add_driver(new TopasESKR());
      meter->add_driver(new Ultrimis());
      meter->add_driver(new Unismart());
      meter->add_driver(new Vario451());
    }
#endif
  }
#endif

#ifdef SUPLA_NTC_10K
  if (ConfigESP->getGpio(FUNCTION_NTC_10K) != OFF_GPIO) {
    auto ntc10k = new Supla::Sensor::NTC10K(ConfigESP->getGpio(FUNCTION_NTC_10K));
    correctionHandler.addThermHygroMeter(ntc10k);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_NTC_10K, S_NTC_10K, ntc10k);
#endif
  }
#endif

#ifdef SUPLA_MPX_5XXX
  if (ConfigESP->getGpio(FUNCTION_MPX_5XXX) != OFF_GPIO) {
    Supla::GUI::mpx = new Supla::Sensor::MPX_5XXX(ConfigESP->getGpio(FUNCTION_MPX_5XXX));
#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_MPX_5XXX, S_MPX_5XXX, Supla::GUI::mpx);
#endif

    Supla::Sensor::Percentage *mpxPercent;
    if (Supla::GUI::mpx->getThankHeight() != 0) {
      mpxPercent = new Supla::Sensor::Percentage(Supla::GUI::mpx, 0, Supla::GUI::mpx->getThankHeight() * 0.01);
    }
    else {
      mpxPercent = new Supla::Sensor::Percentage(Supla::GUI::mpx, 0, 100.0);
    }

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_MPX_5XXX_PERCENT, S_MPX_5XXX_PERCENT, mpxPercent);
#endif
  }
#endif

#ifdef SUPLA_ANALOG_READING_MAP
#ifdef ARDUINO_ARCH_ESP8266
  Supla::GUI::analog = new Supla::Sensor::AnalogRedingMap *[ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt()];
  gpio = ConfigESP->getGpio(FUNCTION_ANALOG_READING);

  if (gpio != OFF_GPIO) {
    for (nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
      Supla::GUI::analog[nr] = new Supla::Sensor::AnalogRedingMap(gpio);
#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_ANALOG_READING_MAP, S_ANALOG_READING_MAP, Supla::GUI::analog[nr], nr);
#endif
    }
  }
#endif
#ifdef ARDUINO_ARCH_ESP32
  Supla::GUI::analog = new Supla::Sensor::AnalogRedingMap *[ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt()];

  for (nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
    gpio = ConfigESP->getGpio(nr, FUNCTION_ANALOG_READING);
    if (gpio != OFF_GPIO) {
      Supla::GUI::analog[nr] = new Supla::Sensor::AnalogRedingMap(gpio);
#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_ANALOG_READING_MAP, S_ANALOG_READING_MAP, Supla::GUI::analog[nr], nr);
#endif
    }
  }
#endif
#endif

#ifdef SUPLA_VINDRIKTNING_IKEA
  if (ConfigESP->getGpio(FUNCTION_VINDRIKTNING_IKEA) != OFF_GPIO) {
#ifdef ARDUINO_ARCH_ESP32
    auto vindriktningIkea = new Supla::Sensor::VindriktningIkea(ConfigESP->getHardwareSerial(ConfigESP->getGpio(FUNCTION_VINDRIKTNING_IKEA)));
#else
    auto vindriktningIkea = new Supla::Sensor::VindriktningIkea(ConfigESP->getGpio(FUNCTION_VINDRIKTNING_IKEA));
#endif

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_VINDRIKTNING_IKEA, S_VINDRIKTNING_IKEA, vindriktningIkea);
#endif
    improvSerialComponent->disable();
  }
#endif

#ifdef SUPLA_PMSX003
  if (ConfigESP->getGpio(FUNCTION_PMSX003_RX) != OFF_GPIO) {
    Supla::Sensor::PMSx003 *pms;

    if (ConfigESP->getGpio(FUNCTION_PMSX003_TX) != OFF_GPIO) {
      pms = new Supla::Sensor::PMSx003(ConfigESP->getGpio(FUNCTION_PMSX003_RX), ConfigESP->getGpio(FUNCTION_PMSX003_TX));
    }
    else {
      pms = new Supla::Sensor::PMSx003(ConfigESP->getGpio(FUNCTION_PMSX003_RX));
    }

    new Supla::Sensor::PMS_PM01(pms);
    auto pmsPM25 = new Supla::Sensor::PMS_PM25(pms);
    new Supla::Sensor::PMS_PM10(pms);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_PMSX003, S_PMSX003_PM25, pmsPM25);
#endif
  }
#endif

#ifdef SUPLA_RGBW
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_RGBW)->getValueInt(); nr++) {
    Supla::GUI::addRGBWLeds(nr);
  }
#endif

#ifdef SUPLA_IMPULSE_COUNTER
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_IMPULSE_COUNTER)->getValueInt(); nr++) {
    if (ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER) != OFF_GPIO) {
      Supla::GUI::addImpulseCounter(nr);
    }
  }
#endif

#ifdef SUPLA_HLW8012
  if (ConfigESP->getGpio(FUNCTION_CF) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CF1) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SEL) != OFF_GPIO) {
    Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
    improvSerialComponent->disable();
  }
#endif

#ifdef SUPLA_PZEM_V_3
  Supla::Sensor::ElectricityMeter *PZEMv3 = nullptr;
  int8_t pinRX1 = ConfigESP->getGpio(1, FUNCTION_PZEM_RX);
  int8_t pinTX1 = ConfigESP->getGpio(1, FUNCTION_PZEM_TX);
  int8_t pinRX2 = ConfigESP->getGpio(2, FUNCTION_PZEM_RX);
  int8_t pinTX2 = ConfigESP->getGpio(2, FUNCTION_PZEM_TX);
  int8_t pinRX3 = ConfigESP->getGpio(3, FUNCTION_PZEM_RX);
  int8_t pinTX3 = ConfigESP->getGpio(3, FUNCTION_PZEM_TX);

  if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO && pinRX2 != OFF_GPIO && pinTX2 != OFF_GPIO && pinRX3 != OFF_GPIO && pinTX3 != OFF_GPIO) {
#ifdef ARDUINO_ARCH_ESP32
    PZEMv3 = new Supla::Sensor::CustomThreePhasePZEMv3(&Serial, pinRX1, pinTX1, &Serial1, pinRX2, pinTX2, &Serial2, pinRX3, pinTX3);
#else
    PZEMv3 = new Supla::Sensor::CustomThreePhasePZEMv3(pinRX1, pinTX1, pinRX2, pinTX2, pinRX3, pinTX3);
#endif
  }
  else if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO && pinTX2 != OFF_GPIO && pinTX3 != OFF_GPIO) {
#ifdef ARDUINO_ARCH_ESP32
    PZEMv3 = new Supla::Sensor::CustomThreePhasePZEMv3(&Serial, pinRX1, pinTX1, &Serial1, pinRX1, pinTX2, &Serial2, pinRX1, pinTX3);
#else
    PZEMv3 = new Supla::Sensor::CustomThreePhasePZEMv3(pinRX1, pinTX1, pinRX1, pinTX2, pinRX1, pinTX3);
#endif
  }
  else if (pinRX1 != OFF_GPIO && pinTX1 != OFF_GPIO) {
#ifdef ARDUINO_ARCH_ESP32
    PZEMv3 = new Supla::Sensor::PZEMv3(&Serial, pinRX1, pinTX1);
#else
    PZEMv3 = new Supla::Sensor::PZEMv3(pinRX1, pinTX1);
#endif
  }

  if (PZEMv3) {
    improvSerialComponent->disable();
#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_PZEM_V3, S_PZEM_V3, PZEMv3);
#endif
  }
#endif

#ifdef SUPLA_CSE7766
  if (ConfigESP->getGpio(FUNCTION_CSE7766_RX) != OFF_GPIO) {
    Supla::GUI::addCSE7766(ConfigESP->getGpio(FUNCTION_CSE7766_RX));
    improvSerialComponent->disable();
  }
#endif

#if defined(SUPLA_MODBUS_SDM) || defined(SUPLA_MODBUS_SDM_ONE_PHASE)
  if (ConfigESP->getGpio(FUNCTION_SDM_RX) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SDM_TX) != OFF_GPIO) {
#if defined(SUPLA_MODBUS_SDM)
#ifdef ARDUINO_ARCH_ESP32
    Supla::GUI::smd = new Supla::Sensor::SDM630(
        ConfigESP->getHardwareSerial(ConfigESP->getGpio(FUNCTION_SDM_RX), ConfigESP->getGpio(FUNCTION_SDM_TX)), ConfigESP->getGpio(FUNCTION_SDM_RX),
        ConfigESP->getGpio(FUNCTION_SDM_TX), ConfigESP->getBaudRateSpeed(ConfigESP->getGpio(FUNCTION_SDM_RX)));
#else
    Supla::GUI::smd = new Supla::Sensor::SDM630(ConfigESP->getGpio(FUNCTION_SDM_RX), ConfigESP->getGpio(FUNCTION_SDM_TX),
                                                ConfigESP->getBaudRateSpeed(ConfigESP->getGpio(FUNCTION_SDM_RX)));

#endif
    if (ConfigESP->getBaudRate(ConfigESP->getGpio(FUNCTION_SDM_RX)) == BAUDRATE_38400) {
      Supla::GUI::smd->setRefreshRate(30);
    }
    else {
      Supla::GUI::smd->setRefreshRate(60);
    }
#endif

#if defined(SUPLA_MODBUS_SDM_ONE_PHASE)
#ifdef ARDUINO_ARCH_ESP32
    Supla::GUI::smd120 = new Supla::Sensor::SDM120(
        ConfigESP->getHardwareSerial(ConfigESP->getGpio(FUNCTION_SDM_RX), ConfigESP->getGpio(FUNCTION_SDM_TX)), ConfigESP->getGpio(FUNCTION_SDM_RX),
        ConfigESP->getGpio(FUNCTION_SDM_TX), ConfigESP->getBaudRateSpeed(ConfigESP->getGpio(FUNCTION_SDM_RX)));
#else
    Supla::GUI::smd120 = new Supla::Sensor::SDM120(ConfigESP->getGpio(FUNCTION_SDM_RX), ConfigESP->getGpio(FUNCTION_SDM_TX),
                                                   ConfigESP->getBaudRateSpeed(ConfigESP->getGpio(FUNCTION_SDM_RX)));

#endif
    if (ConfigESP->getBaudRate(ConfigESP->getGpio(FUNCTION_SDM_RX)) == BAUDRATE_38400) {
      Supla::GUI::smd120->setRefreshRate(10);
    }
    else {
      Supla::GUI::smd120->setRefreshRate(60);
    }

#endif
    improvSerialComponent->disable();
  }
#endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_ENERGY_METER)
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    bool force400khz = false;

#ifdef SUPLA_BME280
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
      Supla::Sensor::BME280 *bme280 = nullptr;

      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt()) {
        case BMx280_ADDRESS_0X76:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BME280, S_BME280, bme280);
#endif
          break;
        case BMx280_ADDRESS_0X77:
          bme280 = new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BME280, S_BME280, bme280);
#endif
          break;
        case BMx280_ADDRESS_0X76_AND_0X77:
          bme280 = new Supla::Sensor::BME280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          Supla::Sensor::BME280 *bme280_1 = new Supla::Sensor::BME280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BME280, S_BME280, bme280);
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BME280, S_BME280, bme280_1, 1);
#endif
          break;
      }
      if (bme280) {
        correctionHandler.addThermHygroMeter(bme280);
      }
    }
#endif

#ifdef SUPLA_BMP280
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt()) {
      Supla::Sensor::BMP280 *bmp280 = nullptr;

      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt()) {
        case BMx280_ADDRESS_0X76:
          bmp280 = new Supla::Sensor::BMP280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BMP280, S_BMP280, bmp280);
#endif
          break;
        case BMx280_ADDRESS_0X77:
          bmp280 = new Supla::Sensor::BMP280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BMP280, S_BMP280, bmp280);
#endif
          break;
        case BMx280_ADDRESS_0X76_AND_0X77:
          bmp280 = new Supla::Sensor::BMP280(0x76, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());
          Supla::Sensor::BMP280 *bmp280_1 = new Supla::Sensor::BMP280(0x77, ConfigManager->get(KEY_ALTITUDE_BMX280)->getValueInt());

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BMP280, S_BMP280, bmp280);
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_BMP280, S_BMP280, bmp280_1, 1);
#endif
          break;
      }
      if (bmp280) {
        correctionHandler.addThermHygroMeter(bmp280);
      }
    }
#endif

#ifdef SUPLA_SHT3x
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
      Supla::Sensor::SHT3x *sht3x = nullptr;

      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
        case SHT3x_ADDRESS_0X44:
          sht3x = new Supla::Sensor::SHT3x(0x44);

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_SHT3x, S_SHT3X, sht3x);
#endif
          break;
        case SHT3x_ADDRESS_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x45);

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_SHT3x, S_SHT3X, sht3x);
#endif
          break;
        case SHT3x_ADDRESS_0X44_AND_0X45:
          sht3x = new Supla::Sensor::SHT3x(0x44);
          Supla::Sensor::SHT3x *sht3x_1 = new Supla::Sensor::SHT3x(0x45);

#ifdef SUPLA_CONDITIONS
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_SHT3x, S_SHT3X, sht3x);
          Supla::GUI::Conditions::addConditionsSensor(SENSOR_SHT3x, S_SHT3X, sht3x_1, 1);
#endif
          break;
      }
      if (sht3x) {
        correctionHandler.addThermHygroMeter(sht3x);
      }
    }
#endif

#ifdef SUPLA_SHT_AUTODETECT
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt()) {
      Supla::Sensor::SHTAutoDetect *shtAutoDetect = new Supla::Sensor::SHTAutoDetect();

      correctionHandler.addThermHygroMeter(shtAutoDetect);
#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_SHT3x, S_SHT3X, shtAutoDetect);
#endif
    }
#endif

#ifdef SUPLA_SI7021
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt()) {
      auto si7021 = new Supla::Sensor::Si7021();
      correctionHandler.addThermHygroMeter(si7021);

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_SI7021, S_SI702, si7021);
#endif
    }
#endif

#ifdef SUPLA_VL53L0X
    Supla::Sensor::VL_53L0X *vl53l0x = nullptr;

    switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_VL53L0X).toInt()) {
      case STATE_VL53L0X::STATE_VL53L0X_SENSE_DEFAULT:
        vl53l0x = new Supla::Sensor::VL_53L0X(VL53L0X_I2C_ADDR, Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT);
        break;
      case STATE_VL53L0X::STATE_VL53L0X_SENSE_LONG_RANGE:
        vl53l0x = new Supla::Sensor::VL_53L0X(VL53L0X_I2C_ADDR, Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
        break;
      case STATE_VL53L0X::STATE_VL53L0X_SENSE_HIGH_SPEED:
        vl53l0x = new Supla::Sensor::VL_53L0X(VL53L0X_I2C_ADDR, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
        break;
      case STATE_VL53L0X::STATE_VL53L0X_SENSE_HIGH_ACCURACY:
        vl53l0x = new Supla::Sensor::VL_53L0X(VL53L0X_I2C_ADDR, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);
        break;
    }
    if (vl53l0x) {
      force400khz = true;

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_VL53L0X, S_VL53L0X, vl53l0x);
#endif
    }
#endif

#ifdef SUPLA_HDC1080
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HDC1080).toInt()) {
      auto hdc1080 = new Supla::Sensor::HDC1080();
      force400khz = true;

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_HDC1080, S_HDC1080, hdc1080);
#endif
    }
#endif

#ifdef SUPLA_BH1750
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BH1750).toInt()) {
      auto bh1750 = new Supla::Sensor::BH1750();

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_BH1750, S_BH1750, bh1750);
#endif
    }
#endif

#ifdef SUPLA_MS5611
    if (ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_I2C_MS5611).toInt()) {
      auto ms5611 = new Supla::Sensor::MS5611Sensor(ConfigManager->get(KEY_ALTITUDE_MS5611)->getValueInt());
      /*
      #ifdef SUPLA_CONDITIONS
            Supla::GUI::Conditions::addConditionsSensor(SENSOR_MS5611, S_MS5611, ms5611);
      #endif
      */
    }
#endif

#ifdef SUPLA_MAX44009
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MAX44009).toInt()) {
      auto max4409 = new Supla::Sensor::MAX_44009();

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_MAX44009, S_MAX44009, max4409);
#endif
    }
#endif

#ifdef SUPLA_OLED
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      SuplaOled *oled = new SuplaOled();
#ifdef SUPLA_BUTTON
#ifdef SUPLA_THERMOSTAT
      new Supla::Control::GUI::OledButtonController(oled, Supla::GUI::thermostatArray);
#else
      new Supla::Control::GUI::OledButtonController(oled);
#endif
#endif
    }
#endif

#ifdef SUPLA_LCD_HD44780
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt()) {
      uint8_t lcdCols = 16, lcdRows = 2, addressLCD = 0x20;

      switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt()) {
        case HD44780_ADDRESS_0X20:
          addressLCD = 0x20;
          break;
        case HD44780_ADDRESS_0X21:
          addressLCD = 0x21;
          break;
        case HD44780_ADDRESS_0X22:
          addressLCD = 0x22;
          break;
        case HD44780_ADDRESS_0X23:
          addressLCD = 0x23;
          break;
        case HD44780_ADDRESS_0X24:
          addressLCD = 0x24;
          break;
        case HD44780_ADDRESS_0X25:
          addressLCD = 0x25;
          break;
        case HD44780_ADDRESS_0X26:
          addressLCD = 0x26;
          break;
        case HD44780_ADDRESS_0X27:
          addressLCD = 0x27;
          break;
        case HD44780_ADDRESS_0X38:
          addressLCD = 0x38;
          break;
        case HD44780_ADDRESS_0X3F:
          addressLCD = 0x3F;
          break;
      }

      switch (ConfigManager->get(KEY_HD44780_TYPE)->getValueInt()) {
        case LCD_2X16:
          lcdCols = 16;
          lcdRows = 2;
          break;
        case LCD_2X20:
          lcdCols = 20;
          lcdRows = 2;
          break;
        case LCD_4X16:
          lcdCols = 16;
          lcdRows = 4;
          break;
        case LCD_4X20:
          lcdCols = 20;
          lcdRows = 4;
          break;
      }

      SuplaLCD *lcd = new SuplaLCD(addressLCD, lcdCols, lcdRows);
      lcd->setup(ConfigManager, ConfigESP);
    }
#endif

#ifdef SUPLA_ADE7953
    if (ConfigESP->getGpio(FUNCTION_ADE7953_IRQ) != OFF_GPIO) {
      Supla::GUI::addADE7953(ConfigESP->getGpio(FUNCTION_ADE7953_IRQ));
    }
#endif

    if (force400khz)
      Wire.setClock(400000);
  }
#endif

#ifdef SUPLA_ACTION_TRIGGER
  for (nr = 0; nr < Supla::GUI::calculateElementCountActionTrigger(); nr++) {
    Supla::GUI::addButtonActionTrigger(nr);
  }
  delete Supla::GUI::actionTrigger;
#endif

#ifdef DEBUG_MODE
  new Supla::Sensor::EspFreeHeap();
#endif

#ifdef SUPLA_DEEP_SLEEP
  if (ConfigManager->get(KEY_DEEP_SLEEP_TIME)->getValueInt() > 0) {
    new Supla::Control::DeepSleep(ConfigManager->get(KEY_DEEP_SLEEP_TIME)->getValueInt() * 60);
  }
#endif

#ifdef SUPLA_CONDITIONS
  Supla::GUI::Conditions::addConditions();
#endif

  Supla::GUI::begin();
}

void loop() {
  const uint32_t now = millis();
  SuplaDevice.iterate();

  uint32_t delay_time = LOOP_INTERVAL;
  if (now - last_loop < LOOP_INTERVAL)
    delay_time = LOOP_INTERVAL - (now - last_loop);

  delay(delay_time);

  last_loop = now;
}
