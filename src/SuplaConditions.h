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
#ifndef SuplaConditions_h
#define SuplaConditions_h

#include "SuplaDeviceGUI.h"

#if defined(GUI_SENSOR_SPI) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_OTHER) || defined(GUI_SENSOR_ANALOG) || \
    defined(SUPLA_RGBW)
#define GUI_ALL_SENSOR
#endif

#if defined(GUI_OTHER_ENERGY) || defined(GUI_SENSOR_I2C_ENERGY_METER) || defined(SUPLA_DIRECT_LINKS_MULTI_SENSOR)
#define GUI_ALL_ENERGY
#endif

#ifdef SUPLA_CONDITIONS

#include <supla/channel_element.h>
#include <supla/sensor/electricity_meter.h>
#include <supla/element_with_channel_actions.h>

#define PATH_CONDITIONS "conditions"

#define INPUT_CONDITIONING_MAX         "icm"
#define INPUT_CONDITIONS_TYPE_CLIENT   "ctc"
#define INPUT_CONDITIONS_CLIENT_NUMBER "cscn"
#define INPUT_CONDITIONS_SENSOR_TYPE   "cst"
#define INPUT_CONDITIONS_TYPE          "ct"
#define INPUT_CONDITIONS_MIN           "cmi"
#define INPUT_CONDITIONS_MAX           "cma"
#define INPUT_CONDITIONS_SENSOR_NUMBER "csc"

void createWebConditions();
void handleConditions(int save = 0);
void handleConditionsSave();

enum CONDITIONS
{
  EXECUTIVE_RELAY,
  EXECUTIVE_RGBW,
  EXECUTIVE_PUSHOVER,
  EXECUTIVE_ROLLER_SHUTTER,
  COUNT_EXECUTIVE_LIST
};

enum conditioningType
{
  CONDITION_ON_LESS,
  CONDITION_ON_GREATER,
  CONDITION_ON_LESS_HUMIDITY,
  CONDITION_ON_GREATER_HUMIDITY,
  CONDITION_ON_LESS_VOLTAGE,
  CONDITION_ON_GREATER_VOLTAGE,
  CONDITION_ON_LESS_CURRENT,
  CONDITION_ON_GREATER_CURRENT,
  CONDITION_ON_LESS_POWER_ACTIVE,
  CONDITION_ON_GREATER_POWER_ACTIVE,
  CONDITION_GPIO,
  CONDITION_COUNT
};

enum sensorList
{
  NO_SENSORS = 0,
  SENSOR_DS18B20,
  SENSOR_DHT11,
  SENSOR_DHT22,
  SENSOR_SI7021_SONOFF,
  SENSOR_HC_SR04,
  SENSOR_BME280,
  SENSOR_SHT3x,
  SENSOR_SI7021,
  SENSOR_MAX6675,
  SENSOR_NTC_10K,
  SENSOR_BMP280,
  SENSOR_MPX_5XXX,
  SENSOR_MPX_5XXX_PERCENT,
  SENSOR_ANALOG_READING_MAP,
  SENSOR_VL53L0X,
  SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR,
  SENSOR_HDC1080,
  SENSOR_HLW8012,
  SENSOR_PZEM_V3,
  SENSOR_CSE7766,
  SENSOR_BINARY,
  SENSOR_MAX31855,
  SENSOR_VINDRIKTNING_IKEA,
  SENSOR_PMSX003,
  SENSOR_ADE7953,
  SENSOR_BH1750,
  SENSOR_RGBW,
  SENSOR_RELAY,
  SENSOR_MAX44009,
  SENSOR_MS5611,
  SENSOR_AHTX0,
  COUNT_SENSOR_LIST
};

const char* const CONDITIONS_TYPE_P[] PROGMEM = {
#ifdef GUI_ALL_SENSOR
    S_CHANNEL_VALUE S_SPACE S_ON_LESS,
    S_CHANNEL_VALUE S_SPACE S_ON_GREATER,
    S_HUMIDITY S_SPACE S_ON_LESS,
    S_HUMIDITY S_SPACE S_ON_GREATER,
#else
    S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY,
#endif

#ifdef GUI_ALL_ENERGY
    S_VOLTAGE S_SPACE S_ON_LESS,
    S_VOLTAGE S_SPACE S_ON_GREATER,
    S_CURRENT S_SPACE S_ON_LESS,
    S_CURRENT S_SPACE S_ON_GREATER,
    S_POWER S_SPACE S_ON_LESS,
    S_POWER S_SPACE S_ON_GREATER,
#else
    S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY,
#endif
#if defined(SUPLA_LIMIT_SWITCH) || defined(SUPLA_RELAY)
    S_GPIO,
#else
    S_EMPTY,
#endif
};

namespace Supla {
namespace GUI {
namespace Conditions {

struct ConditionsStruct {
  int functionClient = -1;
  uint8_t nrClient;
  Supla::ActionHandler* client = nullptr;
  int functionSensor = -1;
  uint8_t nrSensor;
  Supla::ChannelElement* sensorElement = nullptr;
  Supla::ElementWithChannelActions* sensorElementWithChannelActions = nullptr;
  Supla::Sensor::ElectricityMeter* electricityMete = nullptr;
};

void addConditionsElement();
void addConditionsExecutive(int functionClient, const char* nameExecutive, Supla::ActionHandler* client, uint8_t nrClient = 0);
void addConditionsSensor(int functionSensor, const char* nameSensor, Supla::ChannelElement* sensor, uint8_t nrSensor = 0);
void addConditionsSensor(int functionSensor, const char* nameSensor, Supla::ElementWithChannelActions* sensor, uint8_t nrSensor = 0);
void addConditionsSensor(int functionSensor, const char* nameSensor, Supla::Sensor::ElectricityMeter* electricityMete, uint8_t nrSensor = 0);

void addConditions();
void addCondition(ConditionsStruct* condition, uint8_t nr, int actionON, int actionOFF);
void addConditionForSensorElementWithChannelActions(ConditionsStruct* sensor, uint8_t nr, int actionON, int actionOFF);
void addConditionForSensor(ConditionsStruct* sensor, uint8_t nr, int actionON, int actionOFF);
void addConditionForElectricityMeter(ConditionsStruct* electricityMete, uint8_t nr);

}  // namespace Conditions
}  // namespace GUI
}  // namespace Supla
#endif
#endif