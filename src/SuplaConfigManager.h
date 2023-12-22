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

#ifndef SuplaConfigManager_h
#define SuplaConfigManager_h

#ifdef ARDUINO_ARCH_ESP8266
#include "FS.h"
#include <LittleFS.h>
#elif ARDUINO_ARCH_ESP32
#include "SPIFFS.h"
#include <os.h>
#include <esp_wifi.h>
#include <time.h>
#endif

#include <supla/storage/key_value.h>
#include <supla/storage/storage.h>

#define CURENT_VERSION   1
#define CONFIG_FILE_PATH "/dat"

#define DEFAULT_HOSTNAME   "GUI Generic"
#define DEFAULT_LOGIN      "admin"
#define DEFAULT_LOGIN_PASS "pass"
#define DEFAULT_EMAIL      "email@address.com"

#define MAX_SSID                32
#define MAX_PASSWORD            64
#define MIN_PASSWORD            4
#define MAX_MLOGIN              32
#define MAX_MPASSWORD           64
#define MAX_HOSTNAME            32
#define MAX_SUPLA_SERVER        SUPLA_SERVER_NAME_MAXSIZE
#define MAX_EMAIL               SUPLA_EMAIL_MAXSIZE
#define MAX_DS18B20_ADDRESS_HEX 16
#define MAX_DS18B20_ADDRESS     8
#define MAX_DS18B20_NAME        8
#define MAX_TYPE_BUTTON         4
#define MAX_MONOSTABLE_TRIGGER  1
#define MAX_FUNCTION            1

#define MAX_DS18B20          20
#define MAX_PUSHOVER_MESSAGE 5
#define MAX_DIRECT_LINK      5
#define MAX_VIRTUAL_RELAY    10
#define MAX_BRIDGE_RF        10

#define MAX_THERMOSTAT       5

#define MAX_ANALOG_BUTTON    5
#define MAX_WAKE_ON_LAN      5

#ifdef ARDUINO_ARCH_ESP8266
#define MAX_GPIO 17
#elif ARDUINO_ARCH_ESP32
#define MAX_GPIO 39
#endif

#define DEFAULT_AT_MULTICLICK_TIME "0.25"
#define DEFAULT_AT_HOLD_TIME       "0.4"

enum _key
{
  KEY_SUPLA_GUID,
  KEY_SUPLA_AUTHKEY,
  KEY_WIFI_SSID,
  KEY_WIFI_PASS,
  KEY_LOGIN,
  KEY_LOGIN_PASS,
  KEY_HOST_NAME,
  KEY_SUPLA_SERVER,
  KEY_SUPLA_EMAIL,
  KEY_MAX_RELAY,
  KEY_MAX_BUTTON,
  KEY_MAX_LIMIT_SWITCH,
  KEY_MAX_DHT22,
  KEY_MAX_DHT11,
  KEY_MULTI_MAX_DS18B20,
  KEY_MAX_ROLLERSHUTTER,
  KEY_ALTITUDE_BMX280,
  KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT,
  KEY_MAX_IMPULSE_COUNTER,
  KEY_ACTIVE_SENSOR,
  KEY_BOARD,
  KEY_CFG_MODE,
  KEY_ADDR_DS18B20,
  KEY_NAME_SENSOR,
  KEY_GPIO,
  KEY_HD44780_TYPE = KEY_GPIO + MAX_GPIO + 1,
  KEY_OLED_ANIMATION,
  KEY_OLED_BACK_LIGHT_TIME,
  KEY_MAX_RGBW,

  KEY_PUSHOVER_TOKEN,
  KEY_PUSHOVER_USER,
  KEY_PUSHOVER_MASSAGE,
  KEY_CONDITIONS_SENSOR_TYPE,
  KEY_CONDITIONS_TYPE,
  KEY_CONDITIONS_MIN,
  KEY_CONDITIONS_MAX,
  KEY_HC_SR04_MAX_SENSOR_READ,
  KEY_DIRECT_LINKS_ON,
  KEY_DIRECT_LINKS_OFF,
  KEY_CORRECTION_TEMP,
  KEY_CORRECTION_HUMIDITY,
  KEY_ENABLE_GUI,
  KEY_ENABLE_SSL,
  KEY_OLED_BACK_LIGHT,
  KEY_DEEP_SLEEP_TIME,
  KEY_MAX_DIRECT_LINKS_SENSOR,
  KEY_DIRECT_LINKS_SENSOR,
  KEY_CONDITIONS_SENSOR_NUMBER,
  KEY_VIRTUAL_RELAY,
  KEY_VIRTUAL_RELAY_MEMORY,

  KEY_RF_BRIDGE_CODE_ON,
  KEY_RF_BRIDGE_CODE_OFF,
  KEY_RF_BRIDGE_LENGTH,
  KEY_RF_BRIDGE_TYPE,
  KEY_RF_BRIDGE_PROTOCOL,
  KEY_RF_BRIDGE_PULSE_LENGTHINT,
  KEY_RF_BRIDGE_REPEAT,

  KEY_AT_MULTICLICK_TIME,
  KEY_AT_HOLD_TIME,

  KEY_NUMBER_BUTTON,
  KEY_ANALOG_BUTTON,
  KEY_ANALOG_INPUT_EXPECTED,

  KEY_MAX_ANALOG_READING,

  KEY_FORCE_RESTART_ESP,
  KEY_ACTIVE_EXPENDER,

  KEY_DIRECT_LINKS_TYPE,

  KEY_PUSHOVER_SOUND,

  KEY_WAKE_ON_LAN_MAX,
  KEY_WAKE_ON_LAN_MAC,

  KEY_CONDITIONS_CLIENT_TYPE,
  KEY_CONDITIONS_CLIENT_TYPE_NUMBER,
  KEY_MAX_CONDITIONS,

  KEY_EXPANDER_NUMBER_BUTTON,
  KEY_NUMBER_BUTTON_ADDITIONAL,
  // KEY_VERSION_CONFIG,
  KEY_ALTITUDE_MS5611,
  KEY_ACTIVE_SENSOR_2,

  KEY_THERMOSTAT_TYPE,
  KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL,
  KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL,
  KEY_THERMOSTAT_HISTERESIS,
  KEY_THERMOSTAT_TEMPERATURE_MIN,
  KEY_THERMOSTAT_TEMPERATURE_MAX,
  KEY_WMBUS_SENSOR_TYPE,
  KEY_WMBUS_SENSOR_ID,
  KEY_WMBUS_SENSOR_KEY,
  OPTION_COUNT
};

#define SEPARATOR ','

enum _settings
{
  NR,
  FUNCTION,
  LEVEL_RELAY,
  PULL_UP_BUTTON,
  MEMORY,
  ACTION_BUTTON,
  EVENT_BUTTON,
  CFG,
  MCP23017_NR_1,
  MCP23017_FUNCTION_1,
  MCP23017_NR_2,
  MCP23017_FUNCTION_2,
  MCP23017_NR_3,
  MCP23017_FUNCTION_3,
  MCP23017_NR_4,
  MCP23017_FUNCTION_4,
  INVERSED_BUTTON,
  CFG_LED
};

enum _function
{
  FUNCTION_OFF,
  FUNCTION_RELAY,
  FUNCTION_BUTTON,
  FUNCTION_LIMIT_SWITCH,
  FUNCTION_CFG_LED,
  FUNCTION_CFG_BUTTON,
  FUNCTION_DS18B20,
  FUNCTION_DHT11,
  FUNCTION_DHT22,
  FUNCTION_SDA,
  FUNCTION_SCL,
  FUNCTION_TRIG,
  FUNCTION_ECHO,
  FUNCTION_SI7021_SONOFF,
  FUNCTION_CLK,
  FUNCTION_CS,
  FUNCTION_MISO, // MISO
  FUNCTION_IMPULSE_COUNTER,
  FUNCTION_CF,
  FUNCTION_CF1,
  FUNCTION_SEL,
  FUNCTION_LED,
  FUNCTION_RGBW_RED,
  FUNCTION_RGBW_GREEN,
  FUNCTION_RGBW_BLUE,
  FUNCTION_RGBW_BRIGHTNESS,
  FUNCTION_NTC_10K,
  FUNCTION_PZEM_RX,
  FUNCTION_PZEM_TX,
  FUNCTION_CSE7766_RX,
  FUNCTION_MPX_5XXX,
  FUNCTION_ANALOG_READING,
  FUNCTION_BUTTON_STOP,
  FUNCTION_RF_BRIDGE_TRANSMITTER,
  FUNCTION_RF_BRIDGE_RECEIVE,
  FUNCTION_VINDRIKTNING_IKEA,
  FUNCTION_PMSX003_RX,
  FUNCTION_PMSX003_TX,
  FUNCTION_ADE7953_IRQ,
  FUNCTION_SDM_RX,
  FUNCTION_SDM_TX,
  FUNCTION_SDA_2,
  FUNCTION_SCL_2,
  FUNCTION_MOSI,
  FUNCTION_GDO0,
  FUNCTION_GDO2
};

enum _e_onfig
{
  E_CONFIG_OK,
  E_CONFIG_FS_ACCESS,
  E_CONFIG_FILE_NOT_FOUND,
  E_CONFIG_FILE_OPEN,
  E_CONFIG_PARSE_ERROR,
  E_CONFIG_MAX
};

enum _button_additional
{
  BUTTON_OLED,
  BUTTON_LCD,
  BUTTON_ADD_1,
  BUTTON_ADD_2,
  BUTTON_ADD_3,
  BUTTON_ADD_4,
  BUTTON_ADD_5,
  BUTTON_RGBW
};

#define CONFIG_MAX_OPTIONS 200

class ConfigOption {
 public:
  ConfigOption(uint8_t key, const char *value, int maxLength, bool loadKey);
  uint8_t getKey();
  const char *getValue();
  int getValueInt();
  float getValueFloat();
  bool getValueBool();
  const char *getValueHex(size_t size);
  int getValueElement(int element);

  int getLength();
  void setLength(int maxLength);
  bool getLoadKey();

  void setValue(const char *value);
  const String getElement(int index);
  const String replaceElement(int index, int value);
  const String replaceElement(int index, const char *newvalue);

 private:
  uint8_t _key;
  char *_value;
  uint16_t _maxLength;
  bool _loadKey;
};

class SuplaConfigManager : public Supla::KeyValue {
 public:
  explicit SuplaConfigManager();
  bool SPIFFSbegin();
  bool migrationConfig();
  uint8_t addKey(uint8_t key, int maxLength, bool loadKey = true);
  uint8_t addKey(uint8_t key, const char *value, int maxLength, bool loadKey = true);
  uint8_t deleteKey(uint8_t key);

  uint8_t load(bool configParse = true);
  uint8_t save();
  void showAllValue();
  void deleteAllValues();
  void deleteDeviceValues();
  void deleteWifiSuplaAdminValues();
  void deleteGPIODeviceValues();

  ConfigOption *get(uint8_t key);
  bool set(uint8_t key, int value);
  bool set(uint8_t key, const char *value);

  bool setElement(uint8_t key, int index, int newvalue);
  bool setElement(uint8_t key, int index, double newvalue);
  bool setElement(uint8_t key, int index, const String &newvalue);

  bool isDeviceConfigured();
  void setGUIDandAUTHKEY();

  bool init() override;

  // Supla protocol config
  virtual bool setSuplaServer(const char *server) override;
  virtual bool setEmail(const char *email) override;
  virtual bool setAuthKey(const char *authkey) override;
  virtual bool setGUID(const char *guid) override;
  virtual bool setDeviceName(const char *name) override;

  virtual bool getSuplaServer(char *result) override;
  virtual bool getEmail(char *result) override;
  virtual bool getGUID(char *result) override;
  virtual bool getAuthKey(char *result) override;
  virtual bool getDeviceName(char *result) override;

  // WiFi config
  bool setWiFiSSID(const char *ssid) override;
  bool setWiFiPassword(const char *password) override;

  bool getWiFiSSID(char *result) override;
  bool getWiFiPassword(char *result) override;

  bool getUInt8(const char *key, uint8_t *result) override;

 private:
  int _optionCount;
  ConfigOption *_options[CONFIG_MAX_OPTIONS];

  bool setElementInternal(uint8_t key, int index, const String &newvalue);
};
#endif
