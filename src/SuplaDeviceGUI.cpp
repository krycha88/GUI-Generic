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
#include "SuplaConfigManager.h"
#include "SuplaGuiWiFi.h"

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012)
#define TIME_SAVE_PERIOD_SEK                 30   // the time is given in seconds
#define TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK 600  // 10min
#define STORAGE_OFFSET                       0
#include <supla/storage/eeprom.h>
Supla::Eeprom eeprom(STORAGE_OFFSET);
#endif
namespace Supla {
namespace GUI {
void begin() {
#ifdef DEBUG_MODE
  new Supla::Sensor::EspFreeHeap();
#endif

  Supla::GUIESPWifi *wifi = new Supla::GUIESPWifi(ConfigManager->get(KEY_WIFI_SSID)->getValue(), ConfigManager->get(KEY_WIFI_PASS)->getValue());

  wifi->enableBuffer(true);
  wifi->enableSSL(false);

  String supla_hostname = ConfigManager->get(KEY_HOST_NAME)->getValue();
  supla_hostname.replace(" ", "_");
  wifi->setHostName(supla_hostname.c_str());

  SuplaDevice.setName((char *)ConfigManager->get(KEY_HOST_NAME)->getValue());

#ifdef BUILD_VERSION
  SuplaDevice.setSwVersion(BUILD_VERSION);
#endif

  SuplaDevice.begin((char *)ConfigManager->get(KEY_SUPLA_GUID)->getValue(),      // Global Unique Identifier
                    (char *)ConfigManager->get(KEY_SUPLA_SERVER)->getValue(),    // SUPLA server address
                    (char *)ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(),     // Email address used to login to Supla Cloud
                    (char *)ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue());  // Authorization key

  ConfigManager->showAllValue();

  WebServer->begin();
}

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void addRelayButton(int pinRelay, int pinButton, bool highIsOn) {
  if (pinRelay != OFF_GPIO) {
    relay.push_back(new Supla::Control::Relay(pinRelay, highIsOn));

    int size = relay.size() - 1;

    switch (ConfigESP->getMemory(size + 1, FUNCTION_RELAY)) {
      case MEMORY_RELAY_OFF:
        relay[size]->setDefaultStateOff();
        break;
      case MEMORY_RELAY_ON:
        relay[size]->setDefaultStateOn();
        break;
      case MEMORY_RELAY_RESTORE:
        relay[size]->setDefaultStateRestore();
        break;
    }

    relay[size]->keepTurnOnDuration();
    eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_SEK * 1000);

    if (pinButton != OFF_GPIO) {
      button.push_back(new Supla::Control::Button(pinButton, true));
      button[size]->addAction(ConfigESP->getAction(size + 1, FUNCTION_BUTTON), *relay[size], ConfigESP->getLevel(size + 1, FUNCTION_BUTTON));
      button[size]->setSwNoiseFilterDelay(50);
    }
  }
}
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber) {
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    for (int i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); ++i) {
      uint8_t ds_key = KEY_DS + i;
      sensorDS.push_back(new DS18B20(pinNumber, ConfigManager->get(ds_key)->getValueBin(MAX_DS18B20_ADDRESS)));
      supla_log(LOG_DEBUG, "Index %d - address %s", i, ConfigManager->get(ds_key)->getValue());
    }
  }
  else {
    sensorDS.push_back(new DS18B20(ConfigESP->getGpio(FUNCTION_DS18B20)));
  }
}
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig, int modeConfigButton, bool highIsOn) {
  ConfigESP->addConfigESP(pinNumberConfig, pinLedConfig, modeConfigButton, highIsOn);
}
#endif

#ifdef SUPLA_ROLLERSHUTTER
std::vector<Supla::Control::RollerShutter *> RollerShutterRelay;
std::vector<Supla::Control::Button *> RollerShutterButtonOpen;
std::vector<Supla::Control::Button *> RollerShutterButtonClose;

void addRolleShutter(int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn) {
  RollerShutterRelay.push_back(new Supla::Control::RollerShutter(pinRelayUp, pinRelayDown, highIsOn));
  if (pinButtonUp != OFF_GPIO)
    RollerShutterButtonOpen.push_back(new Supla::Control::Button(pinButtonUp, true, true));
  if (pinButtonDown != OFF_GPIO)
    RollerShutterButtonClose.push_back(new Supla::Control::Button(pinButtonDown, true, true));
  int size = RollerShutterRelay.size() - 1;
  if (pinButtonUp != OFF_GPIO && pinButtonDown != OFF_GPIO) {
    RollerShutterButtonOpen[size]->addAction(Supla::OPEN_OR_STOP, *RollerShutterRelay[size], Supla::ON_PRESS);
    RollerShutterButtonClose[size]->addAction(Supla::CLOSE_OR_STOP, *RollerShutterRelay[size], Supla::ON_PRESS);
  }
  else if ((pinButtonUp == OFF_GPIO && pinButtonDown != OFF_GPIO) || (pinButtonUp != OFF_GPIO && pinButtonDown == OFF_GPIO)) {
    RollerShutterButtonOpen[size]->addAction(Supla::STEP_BY_STEP, *RollerShutterRelay[size], Supla::ON_PRESS);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_SEK * 1000);
}

void addRolleShutterMomentary(int pinRelayUp, int pinRelayDown, int pinButtonUp, int pinButtonDown, bool highIsOn) {
  RollerShutterRelay.push_back(new Supla::Control::RollerShutter(pinRelayUp, pinRelayDown, highIsOn));
  if (pinButtonUp != OFF_GPIO)
    RollerShutterButtonOpen.push_back(new Supla::Control::Button(pinButtonUp, true, true));
  if (pinButtonDown != OFF_GPIO)
    RollerShutterButtonClose.push_back(new Supla::Control::Button(pinButtonDown, true, true));
  int size = RollerShutterRelay.size() - 1;
  if (pinButtonUp != OFF_GPIO && pinButtonDown != OFF_GPIO) {
    RollerShutterButtonOpen[size]->addAction(Supla::OPEN, *RollerShutterRelay[size], Supla::ON_PRESS);
    RollerShutterButtonOpen[size]->addAction(Supla::STOP, *RollerShutterRelay[size], Supla::ON_RELEASE);

    RollerShutterButtonClose[size]->addAction(Supla::CLOSE, *RollerShutterRelay[size], Supla::ON_PRESS);
    RollerShutterButtonClose[size]->addAction(Supla::STOP, *RollerShutterRelay[size], Supla::ON_RELEASE);
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_SEK * 1000);
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;

void addImpulseCounter(int pin, bool lowToHigh, bool inputPullup, unsigned int debounceDelay) {
  impulseCounter.push_back(new Supla::Sensor::ImpulseCounter(pin, lowToHigh, inputPullup, debounceDelay));
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
std::vector<Supla::Control::Relay *> relay;
std::vector<Supla::Control::Button *> button;
#endif

#ifdef SUPLA_DS18B20
std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_BME280
std::vector<Supla::Sensor::BME280 *> sensorBme280;
#endif

#ifdef SUPLA_SI7021_SONOFF
std::vector<Supla::Sensor::Si7021Sonoff *> sensorSi7021Sonoff;
#endif

#ifdef SUPLA_DHT22
std::vector<Supla::Sensor::DHT *> sensorDHT22;
#endif

#ifdef SUPLA_MAX6675
std::vector<Supla::Sensor::MAX6675_K *> sensorMAX6675_K;
#endif

#ifdef SUPLA_OLED
SuplaOled *oled;
#endif

#ifdef SUPLA_HLW8012
Supla::Sensor::HJ101 *counterHLW8012;

void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL) {
  counterHLW8012 = new Supla::Sensor::HJ101(pinCF, pinCF1, pinSEL);
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif
}  // namespace GUI
}  // namespace Supla

SuplaConfigManager *ConfigManager = new SuplaConfigManager();
SuplaConfigESP *ConfigESP = new SuplaConfigESP();
SuplaWebServer *WebServer = new SuplaWebServer();
