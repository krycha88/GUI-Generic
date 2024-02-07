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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <IPAddress.h>
#include <supla-common/proto.h>

#include "SuplaConfigManager.h"
#include "SuplaDeviceGUI.h"

#define CONFIG_MAX_SIZE 1024  // Change this to your desired max size

namespace Supla {
const char ConfigFileName[] = "/supla-dev.cfg";
const char CustomCAFileName[] = "/custom_ca.pem";
};  // namespace Supla

ConfigOption::ConfigOption(uint8_t key, const char *value, int maxLength, bool loadKey)
    : _key(key), _value(nullptr), _maxLength(maxLength), _loadKey(loadKey) {
  if (maxLength > 0) {
    _maxLength = maxLength + 1;

    if (_loadKey) {
      _value = new char[_maxLength];
      setValue(value);
    }
  }
}

uint8_t ConfigOption::getKey() {
  return _key;
}

const char *ConfigOption::getValue() {
  if (_value)
    return _value;
  else
    return "";
}

int ConfigOption::getValueInt() {
  return atoi(this->getValue());
}

float ConfigOption::getValueFloat() {
  return atof(this->getValue());
}

bool ConfigOption::getValueBool() {
  return atoi(this->getValue());
}

void ConfigOption::getValueHex(char *buffer, size_t bufferSize) {
  size_t a, b;

  buffer[0] = 0;
  b = 0;

  for (a = 0; a < sizeof(_value) && b + 2 < bufferSize; a++) {
    snprintf(&buffer[b], 3, "%02X", (unsigned char)_value[a]);  // NOLINT
    b += 2;
  }
}

int ConfigOption::getValueElement(int element) {
  return this->getValue()[element] - 48;
}

int ConfigOption::getLength() {
  return _maxLength;
}

void ConfigOption::setLength(int maxLength) {
  if (maxLength > 0) {
    char *oldValue = new char[_maxLength];
    strncpy(oldValue, this->getValue(), _maxLength);
    oldValue[_maxLength - 1] = '\0';

    _maxLength = maxLength + 1;
    _value = new char[_maxLength];
    setValue(oldValue);
  }
  else {
    _maxLength = maxLength;
  }
}

bool ConfigOption::getLoadKey() {
  return _loadKey;
}

const String ConfigOption::getElement(int index) {
  String data = this->getValue();
  // data.reserve(_maxLength);
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == SEPARATOR || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

const String ConfigOption::replaceElement(int index, int newvalue) {
  String data = this->getValue();
  data.reserve(_maxLength);
  int lenght = _maxLength;
  String table;
  for (int i = 0; i <= lenght; i++) {
    if (i == index) {
      table += newvalue;
    }
    else {
      table += this->getElement(i);
    }

    if (i < lenght - 1)
      table += SEPARATOR;
  }
  return table;
}

const String ConfigOption::replaceElement(int index, const char *newvalue) {
  int lenght = _maxLength;
  String table;
  for (int i = 0; i <= lenght; i++) {
    if (i == index) {
      table += newvalue;
    }
    else {
      table += this->getElement(i);
    }
    if (i < lenght - 1)
      table += SEPARATOR;
  }
  return table;
}

void ConfigOption::setValue(const char *value) {
  if (value && _value) {
    size_t size = getLength();
    strncpy(_value, value, size);
    _value[size - 1] = '\0';
  }
}

//
// class SuplaConfigManager
//
SuplaConfigManager::SuplaConfigManager() {
  Supla::Storage::SetConfigInstance(this);

  if (SPIFFSbegin()) {
    _optionCount = OPTION_COUNT;

    // Serial.println("SPIFFS all file:");
    // Dir dir = SPIFFS.openDir("");
    // while (dir.next()) {
    //   Serial.println(dir.fileName());
    //   Serial.println(dir.fileSize());
    // }

    this->addKey(KEY_SUPLA_GUID, SUPLA_GUID_SIZE);
    this->addKey(KEY_SUPLA_AUTHKEY, SUPLA_AUTHKEY_SIZE);
    this->addKey(KEY_WIFI_SSID, MAX_SSID);
    this->addKey(KEY_WIFI_PASS, MAX_PASSWORD);
    this->addKey(KEY_LOGIN, MAX_MLOGIN);
    this->addKey(KEY_LOGIN_PASS, MAX_MPASSWORD);
    this->addKey(KEY_HOST_NAME, DEFAULT_HOSTNAME, MAX_HOSTNAME);
    this->addKey(KEY_SUPLA_SERVER, DEFAULT_SERVER, MAX_SUPLA_SERVER);
    this->addKey(KEY_SUPLA_EMAIL, DEFAULT_EMAIL, MAX_EMAIL);

    this->addKey(KEY_CFG_MODE, 2);
    this->addKey(KEY_ENABLE_GUI, 1);
    this->addKey(KEY_ENABLE_SSL, 1);

    this->addKey(KEY_BOARD, 2);

#ifdef ARDUINO_ARCH_ESP8266
    uint8_t nr, key;
    for (nr = 0; nr <= MAX_GPIO; nr++) {
      key = KEY_GPIO + nr;
      this->addKey(key, 36);
    }
#elif ARDUINO_ARCH_ESP32
    uint8_t nr, key;
    for (nr = 0; nr <= MAX_GPIO; nr++) {
      key = KEY_GPIO + nr;
      this->addKey(key, 36);
    }
#endif

#ifdef SUPLA_RELAY
    this->addKey(KEY_MAX_RELAY, "1", 2);
    this->addKey(KEY_VIRTUAL_RELAY, MAX_VIRTUAL_RELAY * 2);
    this->addKey(KEY_VIRTUAL_RELAY_MEMORY, MAX_VIRTUAL_RELAY * 2);
#else
    this->addKey(KEY_MAX_RELAY, 2, false);
    this->addKey(KEY_VIRTUAL_RELAY, MAX_GPIO * 2, false);
    this->addKey(KEY_VIRTUAL_RELAY_MEMORY, MAX_GPIO * 2, false);

#endif

#ifdef SUPLA_CONDITIONS
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4);
    this->addKey(KEY_CONDITIONS_SENSOR_NUMBER, "0", MAX_GPIO * 3);
    this->addKey(KEY_CONDITIONS_CLIENT_TYPE, MAX_GPIO * 2);
    this->addKey(KEY_CONDITIONS_CLIENT_TYPE_NUMBER, "0", MAX_GPIO * 3);
    this->addKey(KEY_MAX_CONDITIONS, "1", 2);

#else
    this->addKey(KEY_CONDITIONS_SENSOR_TYPE, MAX_GPIO * 2, false);
    this->addKey(KEY_CONDITIONS_TYPE, MAX_GPIO * 1, false);
    this->addKey(KEY_CONDITIONS_MIN, MAX_GPIO * 4, false);
    this->addKey(KEY_CONDITIONS_MAX, MAX_GPIO * 4, false);
    this->addKey(KEY_CONDITIONS_SENSOR_NUMBER, MAX_GPIO * 3, false);
    this->addKey(KEY_CONDITIONS_CLIENT_TYPE, MAX_GPIO * 2, false);
    this->addKey(KEY_CONDITIONS_CLIENT_TYPE_NUMBER, "0", MAX_GPIO * 3, false);
    this->addKey(KEY_MAX_CONDITIONS, "1", 2, false);
#endif

#ifdef SUPLA_BUTTON
    this->addKey(KEY_MAX_BUTTON, "1", 2);
    this->addKey(KEY_ANALOG_BUTTON, 2 * MAX_ANALOG_BUTTON);
    this->addKey(KEY_ANALOG_INPUT_EXPECTED, 5 * MAX_ANALOG_BUTTON);
    this->addKey(KEY_NUMBER_BUTTON, MAX_GPIO * 2);
    this->addKey(KEY_AT_HOLD_TIME, DEFAULT_AT_HOLD_TIME, 4);
    this->addKey(KEY_AT_MULTICLICK_TIME, DEFAULT_AT_MULTICLICK_TIME, 4);

    this->addKey(KEY_NUMBER_BUTTON_ADDITIONAL, 36);
#else
    this->addKey(KEY_MAX_BUTTON, 2, false);
    this->addKey(KEY_ANALOG_BUTTON, 2 * MAX_ANALOG_BUTTON, false);
    this->addKey(KEY_ANALOG_INPUT_EXPECTED, 5 * MAX_ANALOG_BUTTON, false);
    this->addKey(KEY_NUMBER_BUTTON, MAX_GPIO * 2, false);
    this->addKey(KEY_AT_HOLD_TIME, 4, false);
    this->addKey(KEY_AT_MULTICLICK_TIME, 4, false);
    this->addKey(KEY_NUMBER_BUTTON_ADDITIONAL, 36, false);
#endif

#ifdef SUPLA_LIMIT_SWITCH
    this->addKey(KEY_MAX_LIMIT_SWITCH, "1", 2);
#else
    this->addKey(KEY_MAX_LIMIT_SWITCH, 2, false);
#endif

#ifdef SUPLA_DHT22
    this->addKey(KEY_MAX_DHT22, "1", 2);
#else
    this->addKey(KEY_MAX_DHT22, 2, false);
#endif

#ifdef SUPLA_DHT11
    this->addKey(KEY_MAX_DHT11, "1", 2);
#else
    this->addKey(KEY_MAX_DHT11, 2, false);
#endif

#ifdef SUPLA_RGBW
    this->addKey(KEY_MAX_RGBW, "0", 2);
#else
    this->addKey(KEY_MAX_RGBW, 2, false);
#endif

#ifdef SUPLA_DS18B20
    this->addKey(KEY_MULTI_MAX_DS18B20, "1", 2);
    this->addKey(KEY_ADDR_DS18B20, MAX_DS18B20_ADDRESS_HEX * MAX_DS18B20);
#else
    this->addKey(KEY_MULTI_MAX_DS18B20, 2, false);
    this->addKey(KEY_ADDR_DS18B20, MAX_DS18B20_ADDRESS_HEX * MAX_DS18B20, false);
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_OLED) || defined(SUPLA_LCD_HD44780)
    this->addKey(KEY_NAME_SENSOR, MAX_DS18B20_NAME * MAX_DS18B20);
#else
    this->addKey(KEY_NAME_SENSOR, MAX_DS18B20_NAME * MAX_DS18B20, false);
#endif

#ifdef SUPLA_ROLLERSHUTTER
    this->addKey(KEY_MAX_ROLLERSHUTTER, "0", 2);
#else
    this->addKey(KEY_MAX_ROLLERSHUTTER, 2, false);
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_BMP280)
    this->addKey(KEY_ALTITUDE_BMX280, "0", 4);
#else
    this->addKey(KEY_ALTITUDE_BMX280, 4, false);
#endif

#ifdef SUPLA_IMPULSE_COUNTER
    this->addKey(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, "10", 4);
    this->addKey(KEY_MAX_IMPULSE_COUNTER, "0", 2);
#else
    this->addKey(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT, 4, false);
    this->addKey(KEY_MAX_IMPULSE_COUNTER, 2, false);
#endif

#if defined(SUPLA_OLED) || defined(SUPLA_LCD_HD44780)
    this->addKey(KEY_OLED_ANIMATION, "5", 2);
    this->addKey(KEY_OLED_BACK_LIGHT_TIME, "5", 2);
    this->addKey(KEY_OLED_BACK_LIGHT, "20", 2);
#else
    this->addKey(KEY_OLED_ANIMATION, 2, false);
    this->addKey(KEY_OLED_BACK_LIGHT_TIME, 2, false);
    this->addKey(KEY_OLED_BACK_LIGHT, 2, false);
#endif

#ifdef SUPLA_PUSHOVER
    this->addKey(KEY_PUSHOVER_TOKEN, MAX_TOKEN_SIZE);
    this->addKey(KEY_PUSHOVER_USER, MAX_USER_SIZE);
    this->addKey(KEY_PUSHOVER_MASSAGE, MAX_MESSAGE_SIZE * MAX_PUSHOVER_MESSAGE);
    this->addKey(KEY_PUSHOVER_SOUND, 3 * MAX_PUSHOVER_MESSAGE);
#else
    this->addKey(KEY_PUSHOVER_TOKEN, MAX_TOKEN_SIZE, false);
    this->addKey(KEY_PUSHOVER_USER, MAX_USER_SIZE, false);
    this->addKey(KEY_PUSHOVER_MASSAGE, MAX_MESSAGE_SIZE * MAX_PUSHOVER_MESSAGE, false);
    this->addKey(KEY_PUSHOVER_SOUND, 3 * MAX_PUSHOVER_MESSAGE, false);
#endif

#ifdef SUPLA_HC_SR04
    this->addKey(KEY_HC_SR04_MAX_SENSOR_READ, 3);
#else
    this->addKey(KEY_HC_SR04_MAX_SENSOR_READ, 3, false);
#endif

#ifdef SUPLA_DIRECT_LINKS
    this->addKey(KEY_DIRECT_LINKS_ON, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);
    this->addKey(KEY_DIRECT_LINKS_OFF, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);
#else
    this->addKey(KEY_DIRECT_LINKS_ON, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
    this->addKey(KEY_DIRECT_LINKS_OFF, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
#endif

    // #if defined(GUI_SENSOR_SPI) || defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_1WIRE) || defined(GUI_SENSOR_OTHER)
    //     this->addKey(KEY_CORRECTION_TEMP, 6 * MAX_DS18B20);
    //     this->addKey(KEY_CORRECTION_HUMIDITY, 6 * MAX_DS18B20);
    // #else
    this->addKey(KEY_CORRECTION_TEMP, 6 * MAX_DS18B20, false);
    this->addKey(KEY_CORRECTION_HUMIDITY, 6 * MAX_DS18B20, false);
    // #endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_SPI)
    this->addKey(KEY_ACTIVE_SENSOR, 16);
#else
    this->addKey(KEY_ACTIVE_SENSOR, 16, false);
#endif

#if defined(GUI_SENSOR_I2C_2) || defined(GUI_SENSOR_SPI)
    this->addKey(KEY_ACTIVE_SENSOR_2, 96);
#else
    this->addKey(KEY_ACTIVE_SENSOR_2, 96, false);
#endif

#if defined(SUPLA_MS5611)
    this->addKey(KEY_ALTITUDE_MS5611, "0", 4);
#else
    this->addKey(KEY_ALTITUDE_MS5611, 4, false);
#endif

#ifdef SUPLA_DEEP_SLEEP
    this->addKey(KEY_DEEP_SLEEP_TIME, "0", 3);
#else
    this->addKey(KEY_DEEP_SLEEP_TIME, 3, false);
#endif

#ifdef SUPLA_LCD_HD44780
    this->addKey(KEY_HD44780_TYPE, "2", 1);
#else
    this->addKey(KEY_HD44780_TYPE, 1, false);
#endif

#ifdef SUPLA_RF_BRIDGE
    this->addKey(KEY_RF_BRIDGE_CODE_ON, MAX_BRIDGE_RF * 10);
    this->addKey(KEY_RF_BRIDGE_CODE_OFF, MAX_BRIDGE_RF * 10);
    this->addKey(KEY_RF_BRIDGE_LENGTH, MAX_BRIDGE_RF * 3);
    this->addKey(KEY_RF_BRIDGE_TYPE, MAX_BRIDGE_RF * 2);
    this->addKey(KEY_RF_BRIDGE_PROTOCOL, MAX_BRIDGE_RF * 3);
    this->addKey(KEY_RF_BRIDGE_PULSE_LENGTHINT, MAX_BRIDGE_RF * 4);
    this->addKey(KEY_RF_BRIDGE_REPEAT, MAX_BRIDGE_RF * 2);
#else
    this->addKey(KEY_RF_BRIDGE_CODE_ON, MAX_BRIDGE_RF * 10, false);
    this->addKey(KEY_RF_BRIDGE_CODE_OFF, MAX_BRIDGE_RF * 10, false);
    this->addKey(KEY_RF_BRIDGE_LENGTH, MAX_BRIDGE_RF * 3, false);
    this->addKey(KEY_RF_BRIDGE_TYPE, MAX_BRIDGE_RF * 2, false);
    this->addKey(KEY_RF_BRIDGE_PROTOCOL, MAX_BRIDGE_RF * 3, false);
    this->addKey(KEY_RF_BRIDGE_PULSE_LENGTHINT, MAX_BRIDGE_RF * 4, false);
    this->addKey(KEY_RF_BRIDGE_REPEAT, MAX_BRIDGE_RF * 2, false);
#endif

#if defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
    this->addKey(KEY_MAX_ANALOG_READING, "1", 2);
#else
    this->addKey(KEY_MAX_ANALOG_READING, 2, false);
#endif

    this->addKey(KEY_FORCE_RESTART_ESP, "0", 1);

#ifdef GUI_SENSOR_I2C_EXPENDER
    this->addKey(KEY_ACTIVE_EXPENDER, 20);
    this->addKey(KEY_EXPANDER_NUMBER_BUTTON, 96);
#else
    this->addKey(KEY_ACTIVE_EXPENDER, 20, false);
    this->addKey(KEY_EXPANDER_NUMBER_BUTTON, 96, false);
#endif

#if defined(SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR) || defined(SUPLA_DIRECT_LINKS_MULTI_SENSOR)
    this->addKey(KEY_DIRECT_LINKS_TYPE, "0", 3 * MAX_DIRECT_LINK);
    this->addKey(KEY_MAX_DIRECT_LINKS_SENSOR, "0", 2);
    this->addKey(KEY_DIRECT_LINKS_SENSOR, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE);

#else
    this->addKey(KEY_DIRECT_LINKS_TYPE, "0", 3 * MAX_DIRECT_LINK, false);
    this->addKey(KEY_MAX_DIRECT_LINKS_SENSOR, 2, false);
    this->addKey(KEY_DIRECT_LINKS_SENSOR, MAX_DIRECT_LINK * MAX_DIRECT_LINKS_SIZE, false);
#endif

#ifdef SUPLA_WAKE_ON_LAN
    this->addKey(KEY_WAKE_ON_LAN_MAX, 2);
    this->addKey(KEY_WAKE_ON_LAN_MAC, MAX_WAKE_ON_LAN * 18);
#else
    this->addKey(KEY_WAKE_ON_LAN_MAX, 2, false);
    this->addKey(KEY_WAKE_ON_LAN_MAC, MAX_WAKE_ON_LAN * 18, false);
#endif
    //  this->addKey(KEY_VERSION_CONFIG, String(CURENT_VERSION).c_str(), 2);

#ifdef SUPLA_THERMOSTAT
    this->addKey(KEY_THERMOSTAT_TYPE, 2 * MAX_THERMOSTAT);
    this->addKey(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, 3 * MAX_THERMOSTAT);
    this->addKey(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL, 3 * MAX_THERMOSTAT);
    this->addKey(KEY_THERMOSTAT_HISTERESIS, 4 * MAX_THERMOSTAT);
    this->addKey(KEY_THERMOSTAT_TEMPERATURE_MIN, 4 * MAX_THERMOSTAT);
    this->addKey(KEY_THERMOSTAT_TEMPERATURE_MAX, 4 * MAX_THERMOSTAT);

#else
    this->addKey(KEY_THERMOSTAT_TYPE, 2 * MAX_THERMOSTAT, false);
    this->addKey(KEY_THERMOSTAT_MAIN_THERMOMETER_CHANNEL, 3 * MAX_THERMOSTAT, false);
    this->addKey(KEY_THERMOSTAT_AUX_THERMOMETER_CHANNEL, 3 * MAX_THERMOSTAT, false);
    this->addKey(KEY_THERMOSTAT_HISTERESIS, 4 * MAX_THERMOSTAT, false);
    this->addKey(KEY_THERMOSTAT_TEMPERATURE_MIN, 4 * MAX_THERMOSTAT, false);
    this->addKey(KEY_THERMOSTAT_TEMPERATURE_MAX, 4 * MAX_THERMOSTAT, false);
#endif

#ifdef SUPLA_CC1101
    this->addKey(KEY_WMBUS_SENSOR, 30);
    this->addKey(KEY_WMBUS_SENSOR_ID, 100);
    this->addKey(KEY_WMBUS_SENSOR_KEY, 200);
#else
    this->addKey(KEY_WMBUS_SENSOR, 30, false);
    this->addKey(KEY_WMBUS_SENSOR_ID, 100, false);
    this->addKey(KEY_WMBUS_SENSOR_KEY, 200, false);
#endif

    SPIFFS.end();
    switch (this->load()) {
      case E_CONFIG_OK:
        Serial.println(F("Config read"));
        this->showAllValue();
        return;
      case E_CONFIG_PARSE_ERROR:
        Serial.println(F("E_CONFIG_PARSE_ERROR: File was not parsable"));
        return;
      case E_CONFIG_FILE_NOT_FOUND:
        Serial.println(F("File not found"));
        return;
      default:
        Serial.println(F("Config read error"));
        delay(2000);
        ESP.restart();
        return;
    }
  }
  else {
    Serial.println(F("Failed to mount SPIFFS"));
    Serial.println(F("Formatting SPIFFS"));
    SPIFFS.format();
    delay(500);
    SuplaConfigManager();
    // ESP.restart();
  }
  //  switch (this->load()) {
  //    case E_CONFIG_OK:
  //      Serial.println(F("Config read"));
  //      return;
  //    case E_CONFIG_FS_ACCESS:
  //      Serial.println(F("E_CONFIG_FS_ACCESS: Couldn't access file system"));
  //      return;
  //    case E_CONFIG_FILE_NOT_FOUND:
  //      Serial.println(F("E_CONFIG_FILE_NOT_FOUND: File not found"));
  //      return;
  //    case E_CONFIG_FILE_OPEN:
  //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
  //      return;
  //    case E_CONFIG_PARSE_ERROR:
  //      Serial.println(F("E_CONFIG_PARSE_ERROR: File was not parsable"));
  //      return;
  //  }
}

bool SuplaConfigManager::SPIFFSbegin() {
  for (uint8_t t = 4; t > 0; t--) {
    if (
#ifdef ARDUINO_ARCH_ESP8266
        !SPIFFS.begin()
#elif ARDUINO_ARCH_ESP32
        !SPIFFS.begin(true)
#endif
    ) {
      Serial.printf("[SPIFFS] WAIT %d...\n", t);
      Serial.flush();
      delay(250);
    }
    else {
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::migrationConfig() {
  bool migration = false;
  // Serial.print(F("migration Config ver:"));
  // Serial.println(this->get(KEY_VERSION_CONFIG)->getValueInt());

  // if (this->get(KEY_VERSION_CONFIG)->getValueInt() == 0) {
  //   Serial.println(F("0 -> 1"));

  //   if (this->load(false) == E_CONFIG_OK) {
  //     this->get(KEY_VERSION_CONFIG)->setValue("1");
  //     migration = true;
  //   }
  // }

  // if (this->get(KEY_VERSION_CONFIG)->getValueInt() == 1) {
  //   Serial.println(F("1 -> 2"));
  //   // ustawienie starej długości zmiennej przed wczytaniem starego konfiga
  //   this->get(KEY_SUPLA_GUID)->setLength(SUPLA_GUID_SIZE);
  //   this->get(KEY_SUPLA_AUTHKEY)->setLength(SUPLA_AUTHKEY_SIZE);

  //   if (this->load(false) == E_CONFIG_OK) {
  //     // po poprawnym wczytaniu konfiga ustawienie poprawnej wartośći zmiennej
  //     this->get(KEY_SUPLA_GUID)->setLength(SUPLA_GUID_HEXSIZE);
  //     this->get(KEY_SUPLA_AUTHKEY)->setLength(SUPLA_AUTHKEY_HEXSIZE);

  //     this->get(KEY_VERSION_CONFIG)->setValue("2");
  //     migration = true;
  //   }
  // }

  // if (migration) {
  //   this->get(KEY_VERSION_CONFIG)->setValue(String(CURENT_VERSION).c_str());
  //   this->save();
  //   Serial.println(F("successful Config migration"));
  // }

  return migration;
}

uint8_t SuplaConfigManager::addKey(uint8_t key, int maxLength, bool loadKey) {
  return addKey(key, "", maxLength, loadKey);
}

uint8_t SuplaConfigManager::addKey(uint8_t key, const char *value, int maxLength, bool loadKey) {
  if (_optionCount == CONFIG_MAX_OPTIONS) {
    return E_CONFIG_MAX;
  }
  _options[key] = new ConfigOption(key, value, maxLength, loadKey);
  //_optionCount += 1; OPTION_COUNT

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::deleteKey(uint8_t key) {
  for (int i = 0; i < _optionCount; i++) {
    if (_options[i]->getKey() == key) {
      delete _options[_optionCount];
      _optionCount -= 1;
    }
  }

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::load(bool configParse) {
  if (SPIFFSbegin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
      configFile.setTimeout(5000);

      if (configFile) {
        int i = 0;
        int offset = 0;
        size_t length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        // #ifdef ARDUINO_ARCH_ESP8266
        //         FSInfo fs_info;
        //         SPIFFS.info(fs_info);

        //         float fileTotalKB = (float)fs_info.totalBytes / 1024.0;
        //         float fileUsedKB = (float)fs_info.usedBytes / 1024.0;

        //         Serial.println(F("File system (SPIFFS): "));
        //         Serial.print(F(" Total KB: "));
        //         Serial.print(fileTotalKB);
        //         Serial.println(F(" KB"));
        //         Serial.print(F(" Used KB: "));
        //         Serial.print(fileUsedKB);
        //         Serial.println(F(" KB"));
        //         Serial.print(F("Size file: "));
        //         Serial.println(configFile.size());
        //         Serial.print(F("Size conf: "));
        //         Serial.println(length);
        // #endif

        uint8_t *content = new uint8_t[length];
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          if (_options[i]->getLoadKey()) {
            if (strcmp((const char *)(content + offset), "") != 0) {
              // if (strcmp((const char *)(content + offset), "") != 0 || i == KEY_VERSION_CONFIG) {
              _options[i]->setValue((const char *)(content + offset));
            }
          }
          offset += _options[i]->getLength();
          delay(0);
        }

        // if (this->get(KEY_VERSION_CONFIG)->getValueInt() != CURENT_VERSION && configParse) {
        //   if (!this->migrationConfig())
        //     return E_CONFIG_PARSE_ERROR;
        // }

        configFile.close();
        SPIFFS.end();
        delete content;

        return E_CONFIG_OK;
      }
      else {
        configFile.close();
        SPIFFS.end();
        return E_CONFIG_FILE_OPEN;
      }
    }
    else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  }
  else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::save() {
  if (SPIFFSbegin()) {
    int i = 0;
    int offset = 0;
    int length = 0;

    for (i = 0; i < _optionCount; i++) {
      length += _options[i]->getLength();
    }

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
    configFile.setTimeout(5000);

    if (configFile) {
      uint8_t *content = new uint8_t[length];
      for (i = 0; i < _optionCount; i++) {
        if (_options[i]->getLoadKey() && strcmp(_options[i]->getValue(), "") != 0) {
          Serial.print(F("Save key: "));
          Serial.print(_options[i]->getKey());
          Serial.print(F(" Value: "));
          Serial.println(_options[i]->getValue());
          memcpy(content + offset, _options[i]->getValue(), _options[i]->getLength());
        }
        else {
          memset(content + offset, 0, _options[i]->getLength());
        }
        offset += _options[i]->getLength();
        delay(0);
      }

      configFile.write(content, length);

      configFile.flush();
      configFile.close();
      SPIFFS.end();

      delete content;

      return E_CONFIG_OK;
    }
    else {
      configFile.close();
      SPIFFS.end();
      return E_CONFIG_FILE_OPEN;
    }
  }

  return E_CONFIG_FS_ACCESS;
}

void SuplaConfigManager::showAllValue() {
  for (int i = 0; i < _optionCount; i++) {
    Serial.print(F("Key: "));
    Serial.print(_options[i]->getKey());
    Serial.print(F(" Value: "));
    Serial.println(_options[i]->getValue());
  }
}

void SuplaConfigManager::deleteAllValues() {
  deleteWifiSuplaAdminValues();
  deleteDeviceValues();
}

void SuplaConfigManager::deleteDeviceValues() {
  for (int i = KEY_MAX_RELAY; i < _optionCount; i++) {
    _options[i]->setValue("");
  }
}

void SuplaConfigManager::deleteWifiSuplaAdminValues() {
  for (int i = KEY_WIFI_SSID; i <= KEY_SUPLA_EMAIL; i++) {
    _options[i]->setValue("");
  }
}

void SuplaConfigManager::deleteGPIODeviceValues() {
  for (int i = KEY_GPIO; i <= KEY_GPIO + MAX_GPIO; i++) {
    _options[i]->setValue("");
  }
}

bool SuplaConfigManager::isDeviceConfigured() {
  return strcmp(this->get(KEY_SUPLA_GUID)->getValue(), "") == 0 || strcmp(this->get(KEY_SUPLA_AUTHKEY)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_LOGIN)->getValue(), "") == 0 || strcmp(this->get(KEY_ENABLE_SSL)->getValue(), "") == 0 ||
         strcmp(this->get(KEY_ENABLE_GUI)->getValue(), "") == 0 || strcmp(this->get(KEY_SUPLA_SERVER)->getValue(), DEFAULT_SERVER) == 0 ||
         strcmp(this->get(KEY_SUPLA_EMAIL)->getValue(), DEFAULT_EMAIL) == 0;
}

ConfigOption *SuplaConfigManager::get(uint8_t key) {
  for (int i = 0; i < _optionCount; i++) {
    if (_options[i]->getKey() == key) {
      return _options[i];
    }
  }
  return NULL;
}
bool SuplaConfigManager::set(uint8_t key, int value) {
  char buffer[10];
  itoa(value, buffer, 10);
  return set(key, buffer);
}

bool SuplaConfigManager::set(uint8_t key, const char *value) {
  for (int i = 0; i < _optionCount; i++) {
    if (key == _options[i]->getKey()) {
      _options[i]->setValue(value);
      return true;
    }
  }
  return false;
}

bool SuplaConfigManager::setElement(uint8_t key, int index, int newvalue) {
  return setElementInternal(key, index, String(newvalue));
}

bool SuplaConfigManager::setElement(uint8_t key, int index, double newvalue) {
  return setElementInternal(key, index, String(newvalue));
}

bool SuplaConfigManager::setElement(uint8_t key, int index, const String &newvalue) {
  return setElementInternal(key, index, newvalue);
}

bool SuplaConfigManager::setElementInternal(uint8_t key, int index, const String &newvalue) {
  for (int i = 0; i < _optionCount; i++) {
    if (key == _options[i]->getKey()) {
      String data = _options[i]->replaceElement(index, newvalue.c_str());
      _options[i]->setValue(data.c_str());
      return true;
    }
  }
  return false;
}

void SuplaConfigManager::setGUIDandAUTHKEY() {
  char GUID[SUPLA_GUID_SIZE];
  char AUTHKEY[SUPLA_AUTHKEY_SIZE];

  memset(GUID, 0, SUPLA_GUID_SIZE);
  memset(AUTHKEY, 0, SUPLA_AUTHKEY_SIZE);

#if defined(ARDUINO_ARCH_ESP32)
  esp_fill_random((unsigned char *)GUID, SUPLA_GUID_SIZE);
  esp_fill_random((unsigned char *)AUTHKEY, SUPLA_AUTHKEY_SIZE);
#else
  os_get_random((unsigned char *)GUID, SUPLA_GUID_SIZE);
  os_get_random((unsigned char *)AUTHKEY, SUPLA_AUTHKEY_SIZE);
#endif

  this->set(KEY_SUPLA_GUID, GUID);
  this->set(KEY_SUPLA_AUTHKEY, AUTHKEY);

  const size_t GUID_SIZE = SUPLA_GUID_SIZE;
  const size_t AUTHKEY_SIZE = SUPLA_AUTHKEY_SIZE;

  char guidBuffer[GUID_SIZE + 1];
  char authkeyBuffer[AUTHKEY_SIZE + 1];

  strcpy(guidBuffer, ConfigManager->get(KEY_SUPLA_GUID)->getValue());
  strcpy(authkeyBuffer, ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue());

  if (guidBuffer[strlen(guidBuffer) - 1] == '0' || authkeyBuffer[strlen(authkeyBuffer) - 1] == '0') {
    setGUIDandAUTHKEY();
  }
}

bool SuplaConfigManager::init() {
  if (SPIFFSbegin()) {
    if (SPIFFS.exists(Supla::ConfigFileName)) {
      File cfg = SPIFFS.open(Supla::ConfigFileName, "r");
      if (!cfg) {
        SUPLA_LOG_ERROR("SuplaConfigManager: failed to open config file");
        SPIFFS.end();
        return false;
      }

      int fileSize = cfg.size();

      SUPLA_LOG_DEBUG("SuplaConfigManager: config file size %d", fileSize);
      if (fileSize > CONFIG_MAX_SIZE) {
        SUPLA_LOG_ERROR("SuplaConfigManager: config file is too big");
        cfg.close();
        SPIFFS.end();
        return false;
      }

      uint8_t *buf = new uint8_t[CONFIG_MAX_SIZE];
      if (buf == nullptr) {
        SUPLA_LOG_ERROR("SuplaConfigManager: failed to allocate memory");
        cfg.close();
        SPIFFS.end();
        return false;
      }

      memset(buf, 0, CONFIG_MAX_SIZE);
      int bytesRead = cfg.read(buf, fileSize);

      cfg.close();
      SPIFFS.end();
      if (bytesRead != fileSize) {
        SUPLA_LOG_DEBUG("SuplaConfigManager: read bytes %d, while file is %d bytes", bytesRead, fileSize);
        delete[] buf;
        return false;
      }

      SUPLA_LOG_DEBUG("SuplaConfigManager: initializing storage from file...");
      auto result = initFromMemory(buf, fileSize);
      SUPLA_LOG_DEBUG("SuplaConfigManager: init result %d", result);
      delete[] buf;
      return result;
    }
    else {
      SUPLA_LOG_DEBUG("SuplaConfigManager: config file missing");
    }
    SPIFFS.end();
    return true;
  }
  return false;
}

void SuplaConfigManager::commit() {
  uint8_t *buf = new uint8_t[CONFIG_MAX_SIZE];
  if (buf == nullptr) {
    SUPLA_LOG_ERROR("SuplaConfigManager: failed to allocate memory");
    return;
  }

  memset(buf, 0, CONFIG_MAX_SIZE);

  size_t dataSize = serializeToMemory(buf, CONFIG_MAX_SIZE);

  if (!SPIFFSbegin()) {
    return;
  }

  File cfg = SPIFFS.open(Supla::ConfigFileName, "w");
  if (!cfg) {
    SUPLA_LOG_ERROR("SuplaConfigManager: failed to open config file for write");
    SPIFFS.end();
    return;
  }

  cfg.write(buf, dataSize);
  cfg.close();
  delete[] buf;
  SPIFFS.end();
}

void SuplaConfigManager::removeAll() {
  SUPLA_LOG_DEBUG("SuplaConfigManager remove all called");

  if (SPIFFSbegin()) {
    SPIFFS.remove(Supla::CustomCAFileName);

    File suplaDir = SPIFFS.open("/supla", "r");
    if (suplaDir && suplaDir.isDirectory()) {
      File file = suplaDir.openNextFile();
      while (file) {
        if (!file.isDirectory()) {
          SUPLA_LOG_DEBUG("SuplaConfigManager: removing file /supla/%s", file.name());
          char path[200] = {};
          snprintf(path, sizeof(path), "/supla/%s", file.name());
          file.close();
          if (!SPIFFS.remove(path)) {
            SUPLA_LOG_ERROR("SuplaConfigManager: failed to remove file");
          }
        }
        file = suplaDir.openNextFile();
      }
    }
    else {
      SUPLA_LOG_DEBUG("SuplaConfigManager: failed to open supla directory");
    }

    SPIFFS.end();
  }
}

bool SuplaConfigManager::setSuplaServer(const char *server) {
  if (strlen(server) > SUPLA_SERVER_NAME_MAXSIZE - 1) {
    return false;
  }
  return set(KEY_SUPLA_SERVER, server);
}

bool SuplaConfigManager::setEmail(const char *email) {
  if (strlen(email) > SUPLA_EMAIL_MAXSIZE - 1) {
    return false;
  }
  return set(KEY_SUPLA_EMAIL, email);
}

bool SuplaConfigManager::setAuthKey(const char *authkey) {
  return set(KEY_SUPLA_AUTHKEY, authkey);
}

bool SuplaConfigManager::setGUID(const char *guid) {
  return set(KEY_SUPLA_GUID, guid);
}

bool SuplaConfigManager::setDeviceName(const char *name) {
  if (strlen(name) > SUPLA_DEVICE_NAME_MAXSIZE - 1) {
    return false;
  }
  return set(KEY_HOST_NAME, name);
}

bool SuplaConfigManager::getSuplaServer(char *result) {
  String server = ConfigManager->get(KEY_SUPLA_SERVER)->getValue();
  int npos = server.indexOf(":");
  if (npos != -1) {
    server.remove(npos);  // Usuń wszystko od npos
  }

  strncpy(result, server.c_str(), SUPLA_SERVER_NAME_MAXSIZE);
  return true;
}

bool SuplaConfigManager::getEmail(char *result) {
  strncpy(result, ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(), SUPLA_EMAIL_MAXSIZE);
  return true;
}

bool SuplaConfigManager::getGUID(char *result) {
  strncpy(result, ConfigManager->get(KEY_SUPLA_GUID)->getValue(), SUPLA_GUID_SIZE);
  return true;
}

bool SuplaConfigManager::getAuthKey(char *result) {
  strncpy(result, ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue(), SUPLA_AUTHKEY_SIZE);
  return true;
}

bool SuplaConfigManager::getDeviceName(char *result) {
  strncpy(result, ConfigManager->get(KEY_HOST_NAME)->getValue(), SUPLA_DEVICE_NAME_MAXSIZE);
  return true;
}

bool SuplaConfigManager::setWiFiSSID(const char *ssid) {
  if (strlen(ssid) > MAX_SSID_SIZE - 1) {
    return false;
  }
  return set(KEY_WIFI_SSID, ssid);
}

bool SuplaConfigManager::setWiFiPassword(const char *password) {
  if (strlen(password) > MAX_WIFI_PASSWORD_SIZE - 1) {
    return false;
  }
  return set(KEY_WIFI_PASS, password);
}

bool SuplaConfigManager::getWiFiSSID(char *result) {
  strncpy(result, ConfigManager->get(KEY_WIFI_SSID)->getValue(), MAX_SSID_SIZE);
  return true;
}

bool SuplaConfigManager::getWiFiPassword(char *result) {
  strncpy(result, ConfigManager->get(KEY_WIFI_PASS)->getValue(), MAX_WIFI_PASSWORD_SIZE);
  return true;
}

bool SuplaConfigManager::getUInt8(const char *key, uint8_t *result) {
  if (strcmp(key, "security_level") == 0) {
    *result = 2;
    return true;
  }

  return KeyValue::getUInt8(key, result);
}