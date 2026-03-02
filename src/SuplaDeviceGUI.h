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

#ifndef SuplaDeviceGUI_h
#define SuplaDeviceGUI_h

#include "GUI-Generic_Config.h"
#include "GUIGenericCommonDefined.h"

#include <SuplaDeviceExtensions.h>
#include <SuplaDevice.h>
#include "src/control/ControlGUI.h"

#include "GUIGenericCommon.h"
#include "SuplaTemplateBoard.h"

#include "SuplaConfigESP.h"

#include "SuplaCommonPROGMEM.h"

#include "SuplaConfigManager.h"
#include "SuplaWebPageRelay.h"
#include "SuplaWebPageControl.h"
#include "SuplaWebPageLimitSwitch.h"
#include "SuplaWebServer.h"
#include "SuplaWebPageConfig.h"

#include "SuplaWebPageDeviceSettings.h"

#include "SuplaWebPageSensors.h"
#include "SuplaWebPageSensorSpi.h"
#include "SuplaWebPageSensorI2c.h"
#include "SuplaWebPageSensor1Wire.h"
#include "SuplaWebPageSensorAnalog.h"
#include "SuplaWebPageOther.h"

#include "SuplaWebPageDownload.h"
#include "SuplaWebPageUpload.h"
#include "SuplaWebPageTools.h"

#include "Markup.h"
#include "SuplaConditions.h"
#include "SuplaWebPageHome.h"
#include <supla/device/status_led.h>
#include <supla/io/rgb_led.h>


#ifdef SUPLA_OLED
#include "src/display/OledButtonController.h"
#include "src/display/SuplaOled.h"
#endif

#include <vector>

#include <supla/control/button.h>
#include <supla/control/ButtonAnalog.h>
#include <supla/control/relay.h>
#include <supla/control/light_relay.h>
#include <supla/control/virtual_relay.h>
#include <supla/control/roller_shutter.h>

#ifdef SUPLA_DS18B20
#include "src/sensor/DS_18B20.h"
#endif

#if defined(SUPLA_DHT22) || defined(SUPLA_DHT11)
#include <supla/sensor/DHT.h>
#endif

#ifdef SUPLA_HC_SR04
#include "src/sensor/HC_SR04_NewPing.h"
#endif

#include <supla/sensor/binary.h>

#ifdef SUPLA_BME280
#include <supla/sensor/BME280.h>
#endif
#ifdef SUPLA_BMP280
#include <supla/sensor/BMP280.h>
#endif
#ifdef SUPLA_SI7021_SONOFF
#include "src/sensor/Si7021Sonoff.h"
#endif
#ifdef SUPLA_SHT3x
#include <supla/sensor/SHT3x.h>
#endif
#ifdef SUPLA_SHT_AUTODETECT
#include "src/sensor/SHTAutoDetect.h"
#endif
#ifdef SUPLA_SI7021
#include <supla/sensor/Si7021.h>
#endif
#ifdef SUPLA_MAX6675
#include "src/sensor/MAX6675K.h"
#endif
#ifdef SUPLA_MAX31855
#include <supla/sensor/MAX31855.h>
#endif
#ifdef SUPLA_IMPULSE_COUNTER
#include <supla/sensor/impulse_counter.h>
#include <supla/control/internal_pin_output.h>
#endif
#ifdef SUPLA_DEBUG_MODE
#include <supla/sensor/esp_free_heap.h>
#endif
#ifdef SUPLA_HLW8012
#include "src/sensor/HLW_8012.h"
#endif
#ifdef SUPLA_CSE7766
#include <supla/sensor/CSE_7766.h>
#endif

#include "src/control/PinStatusLedGUI.h"
#include "src/control/Pushover.h"

#ifdef SUPLA_RGBW
#include <supla/control/rgbw_leds.h>
#include <supla/control/rgb_leds.h>
#include <supla/control/rgbw_base.h>
#include <supla/control/dimmer_leds.h>
#include <supla/control/group_button_control_rgbw.h>
#include <map>
#endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_ENERGY_METER)
#include <Wire.h>
#endif

#include <supla/control/direct_links.h>

#ifdef SUPLA_NTC_10K
#include "src/sensor/NTC_10K.h"
#endif

#ifdef SUPLA_MPX_5XXX
#include <supla/sensor/MPX_5xxx.h>
#endif

#include <supla/correction.h>

#ifdef SUPLA_ANALOG_READING_KPOP
#include "src/sensor/AnalogReading.h"
#endif

#ifdef SUPLA_VL53L0X
#include <supla/sensor/VL_53L0X.h>
#endif

#ifdef SUPLA_RF_BRIDGE
#include <supla/control/RFBridgeRelay.h>
#include <supla/control/RFBridgeVirtualRelay.h>
#include <supla/control/RFBridgeReceive.h>
#endif

#ifdef SUPLA_HDC1080
#include <supla/sensor/HDC1080.h>
#endif

#include <supla/control/action_trigger.h>

#include <supla/condition.h>
#include <supla/sensor/electricity_meter.h>

#ifdef SUPLA_LCD_HD44780
#include "src/display/SuplaLCD.h"
#endif

#ifdef SUPLA_BH1750_KPOP
#include "src/sensor/BH_1750.h"
#endif

#ifdef SUPLA_MS5611
#include <supla/sensor/MS5611.h>
#endif

#ifdef SUPLA_MAX44009_KPOP
#include "src/sensor/MAX_44009.h"
#endif

#include "src/improv/improv_serial_component.h"
#include "src/network/SuplaGuiWiFi.h"

#ifdef SUPLA_VINDRIKTNING_IKEA_KPOP
#include "src/sensor/VindriktningIkea.h"
#endif

#ifdef SUPLA_PMSX003_KPOP
#include "src/sensor/PMSx003.h"
#endif

#ifdef SUPLA_WT32_ETH01_LAN8720
#include "src/network/SuplaGuiWt32_eth01.h"
#elif defined(SUPLA_ETH01_LAN8720)
#include "src/network/SuplaGuiEth01.h"
#endif

#ifdef SUPLA_ADE7953
#include "src/sensor/ADE7953.h"
#endif

#if defined(SUPLA_BONEIO_32x10A) || defined(SUPLA_BONEIO_24x16A)
#include "src/boneIO/boneIO.h"
#endif

#ifdef GUI_SENSOR_I2C_EXPENDER
#include "src/expander/ConfigExpander.h"
#include "src/expander/ExpanderPCF8574.h"
#include "src/expander/ExpanderPCF8575.h"
#include "src/expander/ExpanderMCP23017.h"
#endif

#ifdef SUPLA_WAKE_ON_LAN
#include "src/control/WakeOnLanRelay.h"
#endif

#ifdef SUPLA_MODBUS_SDM
#include "src/sensor/SDM_630.h"
#endif

#ifdef SUPLA_MODBUS_SDM_ONE_PHASE
#include "src/sensor/SDM_120.h"
#endif

#ifdef SUPLA_MODBUS_SDM_72_V2
#include "src/sensor/SDM_72.h"
#endif

#ifdef SUPLA_DEEP_SLEEP
#include "src/control/deepSleep.h"
#endif

#ifdef SUPLA_THERMOSTAT
#include "src/control/ThermostatGUI.h"
#endif

#ifdef SUPLA_CC1101
#include "src/sensor/WmbusMeter.h"
#include <Drivers/drivers.h>
#include <SensorInfo.h>
#endif

#ifdef SUPLA_AHTX0
#include "src/sensor/AHTX0.h"
#endif

#ifdef SUPLA_SPS30_KPOP
#include "src/sensor/SPS30.h"
#endif

#ifdef SUPLA_INA219
#include "src/sensor/INA_219.h"
#endif

#ifdef SUPLA_INA226
#include "src/sensor/INA_226.h"
#endif

#ifdef SUPLA_INA228
#include "src/sensor/INA_228.h"
#endif

#ifdef SUPLA_INA236
#include "src/sensor/INA_236.h"
#endif

#ifdef SUPLA_INA260
#include "src/sensor/INA_260.h"
#endif

#ifdef SUPLA_INA229
#include "src/sensor/INA_229.h"
#endif

#ifdef SUPLA_INA238
#include "src/sensor/INA_238.h"
#endif

#ifdef SUPLA_INA239
#include "src/sensor/INA_239.h"
#endif

// #ifdef SUPLA_MDNS
// #ifdef ARDUINO_ARCH_ESP8266
// #include <cont.h>
// #include <user_interface.h>
// #include <ESP8266WiFi.h>
// #include <ESP8266mDNS.h>
// #elif ARDUINO_ARCH_ESP32
// #include <WiFi.h>
// #include <ESPmDNS.h>
// #endif
// #endif

#include "src/storage/SPIFFS_config.h"

#define TIME_SAVE_PERIOD_SEK                 30   // the time is given in seconds
#define TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK 600  // 10min
#define STORAGE_OFFSET                       0
#include <supla/storage/eeprom.h>

extern Supla::Eeprom eeprom;

namespace Supla {
namespace GUI {

void begin();
void setupPreConfiguredSettingsIfAvailable();
void setupConnection();
void enableConnectionSSL(bool value);
void crateWebServer();

void addRelayOrThermostat(int nr);

#ifdef SUPLA_RELAY
void addRelay(uint8_t nr);
void addButtonToRelay(uint8_t nrRelay, Supla::Control::Relay *relay);
void addButtonToRelay(uint8_t nrRelay,
                      Supla::Element *element = nullptr,
                      Supla::ActionHandler *client = nullptr,
                      Supla::Control::Relay *relay = nullptr);
#endif

#ifdef SUPLA_ACTION_TRIGGER
struct ActionTrigger {
  bool active = false;
};

extern ActionTrigger *actionTrigger;

void addButtonActionTrigger(uint8_t nr);
void addActionTriggerRelatedChannel(uint8_t nr, Supla::Control::Button *button, int eventButton, Supla::Element *element = nullptr);
int calculateElementCountActionTrigger();
#endif

#if defined(SUPLA_RF_BRIDGE)
void addRelayBridge(uint8_t nr);
void addButtonBridge(uint8_t nr);
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_RF_BRIDGE)
extern std::vector<Supla::Control::Relay *> relay;
#endif

#ifdef SUPLA_PUSHOVER
void addPushover(uint8_t nr);
#endif

#if defined(SUPLA_DIRECT_LINKS)
void addDirectLinks(uint8_t nr);
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber);

extern std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig);
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(uint8_t nr);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
extern std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;
void addImpulseCounter(uint8_t nr);
#endif

#ifdef SUPLA_RGBW
void addRGBWLeds(uint8_t nr);
bool isRGBWButtonGroupOverloaded(uint8_t nrButton);
void setRGBWDefaultState(Supla::Control::RGBWBase *rgbw, uint8_t memory);
#endif

#ifdef SUPLA_HLW8012
extern Supla::Sensor::HLW_8012 *counterHLW8012;
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL);
#endif

#ifdef SUPLA_CSE7766
extern Supla::Sensor::CSE_7766 *counterCSE7766;
void addCSE7766(int8_t pinRX);
#endif

#ifdef SUPLA_ADE7953
extern Supla::Sensor::ADE7953 *couterADE7953;
void addADE7953(int8_t pinIRQ);
#endif

#ifdef SUPLA_MPX_5XXX
extern Supla::Sensor::MPX_5XXX *mpx;
#endif

#ifdef SUPLA_ANALOG_READING_KPOP
extern std::vector<Supla::Sensor::AnalogReading *> analogSensorData;
#endif

#ifdef SUPLA_MODBUS_SDM
extern Supla::Sensor::SDM630 *smd;
#endif

#ifdef SUPLA_MODBUS_SDM_72_V2
extern Supla::Sensor::SDM72V2 *smd;
#endif

#ifdef SUPLA_MODBUS_SDM_ONE_PHASE
extern Supla::Sensor::SDM120 *smd120;
#endif

};  // namespace GUI
};  // namespace Supla

extern SuplaConfigManager *ConfigManager;
extern SuplaConfigESP *ConfigESP;
extern SuplaWebServer *WebServer;

#ifdef GUI_SENSOR_I2C_EXPENDER
extern Supla::Control::ConfigExpander *Expander;
#endif

#ifdef SUPLA_WT32_ETH01_LAN8720
extern Supla::WT32_ETH01 *eth;
#elif defined(SUPLA_ETH01_LAN8720)
extern Supla::GUI_ETH01 *eth;
#else
extern Supla::GUIESPWifi *wifi;
#endif

#endif  // SuplaDeviceGUI_h
