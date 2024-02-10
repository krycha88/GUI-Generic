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

#include <SPI.h>

#include <SuplaDeviceExtensions.h>
#include <SuplaDevice.h>
#include <supla/storage/littlefs_config.h>
#include "src/control/ControlGUI.h"

#include "GUIGenericCommon.h"
#include "SuplaTemplateBoard.h"

#include "SuplaConfigESP.h"

#include "SuplaCommonPROGMEM.h"
#include "src/boneIO/boneIO.h"

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

#ifdef SUPLA_OLED
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
#include <supla/sensor/DHT.h>

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
#include <supla/sensor/Si7021_sonoff.h>
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
#include <supla/sensor/HLW_8012.h>
#endif
#ifdef SUPLA_CSE7766
#include <supla/sensor/CSE_7766.h>
#endif

#include "src/control/PinStatusLedGUI.h"
#include "src/control/Pushover.h"

#ifdef SUPLA_RGBW
#include <supla/control/rgbw_leds.h>
#include <supla/control/rgb_leds.h>
#include <supla/control/dimmer_leds.h>
#endif

#include <Wire.h>

#include <supla/control/direct_links.h>

#ifdef SUPLA_NTC_10K
#include "src/sensor/NTC_10K.h"
#endif

#ifdef SUPLA_MPX_5XXX
#include <supla/sensor/MPX_5xxx.h>
#endif

#include <supla/correction.h>

#ifdef SUPLA_ANALOG_READING_MAP
#include <supla/sensor/AnalogReadingMap.h>
#endif

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

#ifdef SUPLA_BH1750
#include <supla/sensor/BH_1750.h>
#endif

#ifdef SUPLA_MS5611
#include <supla/sensor/MS5611.h>
#endif

#ifdef SUPLA_MAX44009
#include "src/sensor/MAX_44009.h"
#endif

#include "src/improv/improv_serial_component.h"
#include "src/network/SuplaGuiWiFi.h"

#ifdef SUPLA_VINDRIKTNING_IKEA
#include "src/sensor/VindriktningIkea.h"
#endif

#include "src/sensor/PMSx003.h"

#ifdef SUPLA_WT32_ETH01_LAN8720
#include "src/network/SuplaGuiWt32_eth01.h"
#elif defined(SUPLA_ETH01_LAN8720)
#include "src/network/SuplaGuiEth01.h"
#endif

#ifdef SUPLA_ADE7953
#include "src/sensor/ADE7953.h"
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

#include "src/storage/SPIFFS_config.h"

#define TIME_SAVE_PERIOD_SEK                 30   // the time is given in seconds
#define TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK 600  // 10min
#define STORAGE_OFFSET                       0
#include <supla/storage/eeprom.h>

extern Supla::Eeprom eeprom;

namespace Supla {
namespace GUI {

void begin();
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
void addActionTriggerRelatedChannel(
    uint8_t nr, Supla::Control::Button *button, int eventButton, Supla::Element *element);
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
void setRGBWButton(uint8_t nr, Supla::Control::RGBWBase *rgbw);
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

#ifdef SUPLA_ANALOG_READING_MAP
extern Supla::Sensor::AnalogRedingMap **analog;
#endif

#ifdef SUPLA_ANALOG_READING_KPOP
extern Supla::Sensor::AnalogReding **analog;
#endif

#ifdef SUPLA_MODBUS_SDM
extern Supla::Sensor::SDM630 *smd;
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
