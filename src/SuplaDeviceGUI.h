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

#include <DoubleResetDetector.h>
#include <SPI.h>

#include <SuplaDevice.h>

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"
#include "SuplaWebPageRelay.h"
#include "SuplaWebPageSensor.h"

#include "SuplaWebPageDownload.h"
#include "SuplaWebPageUpload.h"
#include "SuplaWebPageTools.h"
#include "GUIGenericCommon.h"
#include "SuplaCommonPROGMEM.h"
#include "Markup.h"
#include "SuplaOled.h"

#include <vector>

#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla/control/roller_shutter.h>

#include "SuplaSensorDS18B20.h"
#include <supla/sensor/DHT.h>
#include <supla/sensor/HC_SR04.h>
#include <supla/sensor/binary.h>

#ifdef SUPLA_BME280
#include <supla/sensor/BME280.h>
#endif
#ifdef SUPLA_SI7021_SONOFF
#include <supla/sensor/Si7021_sonoff.h>
#endif
#ifdef SUPLA_BME280
#include <supla/sensor/BME280.h>
#endif
#ifdef SUPLA_SHT3x
#include <supla/sensor/SHT3x.h>
#endif
#ifdef SUPLA_SI7021
#include <supla/sensor/Si7021.h>
#endif
#ifdef SUPLA_MAX6675
#include <supla/sensor/MAXThermocouple.h>
#endif
#ifdef SUPLA_IMPULSE_COUNTER
#include <supla/sensor/impulse_counter.h>
#endif
#ifdef DEBUG_MODE
#include <supla/sensor/esp_free_heap.h>
#endif

#include <supla/sensor/HJ101.h>
#include <supla/control/pin_status_led.h>

#ifdef SUPLA_RGBW
#include <supla/control/rgbw_leds.h>
#include <supla/control/rgb_leds.h>
#include <supla/control/dimmer_leds.h>
#endif

#include <Wire.h>

#include <supla/control/pushover.h>
#include <supla/control/direct_links.h>

namespace Supla {
namespace GUI {

void begin();

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER)
void addRelayButton(uint8_t nr);

extern std::vector<Supla::Control::Relay *> relay;
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber);

extern std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig, int modeConfigButton, bool highIsOn);
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(uint8_t nr);
void addRolleShutterMomentary(uint8_t nr);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
extern std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;
void addImpulseCounter(int pin, bool lowToHigh, bool inputPullup, unsigned int debounceDelay);
#endif

#ifdef SUPLA_RGBW
void addRGBWLeds(uint8_t nr);
#endif

#ifdef SUPLA_HLW8012
extern Supla::Sensor::HJ101 *counterHLW8012;
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL);
#endif

void addConditionsTurnON(int function, Supla::ChannelElement *client);
void addConditionsTurnOFF(int function, Supla::ChannelElement *client);

};  // namespace GUI
};  // namespace Supla

extern SuplaConfigManager *ConfigManager;
extern SuplaConfigESP *ConfigESP;
extern SuplaWebServer *WebServer;

#endif  // SuplaDeviceGUI_h
