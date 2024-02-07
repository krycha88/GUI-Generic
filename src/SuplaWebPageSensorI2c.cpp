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

#include "SuplaWebPageSensorI2c.h"

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_ENERGY_METER)
void createWebPageSensorI2c() {
  WebServer->httpServer->on(getURL(PATH_I2C), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorI2c();
    else
      handleSensorI2cSave();
  });
}

void webPageI2CScanner(TwoWire* wire) {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  // wire->begin(ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL));

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    wire->beginTransmission(address);
    error = wire->endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");

      addLabel(String("I2C device found at address 0x") + String(address, HEX));
      Serial.print(address, HEX);

      nDevices++;
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
    addLabel("No I2C devices found\n");
  }
  else
    Serial.println("done\n");
}

void handleSensorI2c(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_I2C);

  addForm(F("post"), PATH_I2C);
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_I2C);
  addListGPIOBox(INPUT_SDA, S_SDA, FUNCTION_SDA);
  addListGPIOBox(INPUT_SCL, S_SCL, FUNCTION_SCL);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    webPageI2CScanner(&Wire);
  }
  addFormHeaderEnd();

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt();
    addFormHeader();
    addListBox(INPUT_BME280, String(S_ADDRESS) + S_SPACE + S_BME280, BMx280_P, 4, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt())
      addNumberBox(INPUT_ALTITUDE_BMx280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BMX280, 1500);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_BMP280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt();
    addFormHeader();
    addListBox(INPUT_BMP280, String(S_ADDRESS) + S_SPACE + S_BMP280, BMx280_P, 4, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BMP280).toInt())
      addNumberBox(INPUT_ALTITUDE_BMx280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BMX280, 1500);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt();
    addFormHeader();
    addListBox(INPUT_SHT3x, S_SHT3X, SHT3x_P, 4, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_SHT_AUTODETECT
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt();
    addFormHeader();
    addListBox(INPUT_SUPLA_SHT_AUTODETECT, F("SHT"), STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt();
    addFormHeader();
    addListBox(INPUT_SI7021, S_SI702, STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_VL53L0X
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_VL53L0X).toInt();
    addFormHeader();
    addListBox(INPUT_VL53L0X, F("VL53L0X"), STATE_VL53L0X_P, 5, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_HDC1080
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HDC1080).toInt();
    addFormHeader();
    addListBox(INPUT_HDC1080, F("HDC1080"), STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_BH1750
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BH1750).toInt();
    addFormHeader();
    addListBox(INPUT_BH1750, F("BH1750"), STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_MS5611
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_I2C_MS5611).toInt();
    addFormHeader();
    addListBox(INPUT_MS5611, F("MS5611"), STATE_P, 2, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_I2C_MS5611).toInt())
      addNumberBox(INPUT_ALTITUDE_MS5611, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_MS5611, 9000);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_MAX44009
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MAX44009).toInt();
    addFormHeader();
    addListBox(INPUT_MAX44009, F("MAX44009"), STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_AHTX0
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_I2C_AHTX0).toInt();
    addFormHeader();
    addListBox(INPUT_AHTX0, F("AHTX0"), STATE_P, 2, selected);
    addFormHeaderEnd();
#endif

#ifdef SUPLA_OLED
    addFormHeader();

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt();
    addListBox(INPUT_OLED, S_OLED, OLED_P, 4, selected);

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      String name, sensorName, input;

#ifdef SUPLA_BUTTON
      selected = ConfigESP->getNumberButtonAdditional(BUTTON_OLED);
      addListNumbersBox(INPUT_BUTTON_OLED, S_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(), selected);
#endif

      addNumberBox(INPUT_OLED_ANIMATION, S_SCREEN_TIME, KEY_OLED_ANIMATION, 99);
      addNumberBox(INPUT_OLED_BRIGHTNESS_TIME, S_BACKLIGHT_S, KEY_OLED_BACK_LIGHT_TIME, 99);

      int value = ConfigESP->getBrightnessLevelOLED();
      addNumberBox(INPUT_OLED_BRIGHTNESS_LVL, S_BACKLIGHT_PERCENT, String(value), 100);

      for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_OLED_NAME;
        input += i;
        name = S_SCREEN;
        name += i + 1;
        addTextBox(input, name, sensorName, 0, 12, false);
      }
    }
    addFormHeaderEnd();
#endif

#ifdef SUPLA_LCD_HD44780
    addFormHeader();

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt();
    addListBox(INPUT_LCD, "HD44780", HD44780_P, 11, selected);

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_HD44780).toInt()) {
      String name, sensorName, input;

      selected = ConfigManager->get(KEY_HD44780_TYPE)->getValueInt();
      addListBox(INPUT_HD44780_TYPE, S_TYPE, HD44780_TYPE_P, 4, selected);

#ifdef SUPLA_BUTTON
      selected = ConfigESP->getNumberButtonAdditional(BUTTON_LCD);
      addListNumbersBox(INPUT_BUTTON_LCD, S_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(), selected);
#endif

      addNumberBox(INPUT_OLED_ANIMATION, S_SCREEN_TIME, KEY_OLED_ANIMATION, 99);
      addNumberBox(INPUT_OLED_BRIGHTNESS_TIME, S_BACKLIGHT_S, KEY_OLED_BACK_LIGHT_TIME, 99);

      for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_OLED_NAME;
        input += i;
        name = S_SCREEN;
        name += i + 1;
        addTextBox(input, name, sensorName, 0, 12, false);
      }
    }
    addFormHeaderEnd();
#endif

#ifdef SUPLA_ADE7953
    addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + F("ADE7953"));
    addListGPIOBox(INPUT_ADE7953_IRQ, F("IRQ"), FUNCTION_ADE7953_IRQ);
    if (ConfigESP->getGpio(FUNCTION_ADE7953_IRQ) != OFF_GPIO) {
      float count = Supla::GUI::couterADE7953->getCounter();
      addNumberBox(INPUT_ADE7953_COUNTER_VALUE, String(S_IMPULSE_COUNTER_CHANGE_VALUE) + S_SPACE + F("[kWh]"), F("kWh"), false,
                   String(count / 100 / 1000));
      // addLinkBox(S_CALIBRATION, getParameterRequest(PATH_CALIBRATE, ARG_PARM_URL) + PATH_CSE7766);
    }
    addFormHeaderEnd();
#endif
  }

#ifdef ARDUINO_ARCH_ESP32
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_I2C + "2");
  addListGPIOBox(INPUT_SDA_2, String(S_SDA) + "2", FUNCTION_SDA_2);
  addListGPIOBox(INPUT_SCL_2, String(S_SCL) + "2", FUNCTION_SCL_2);

  if (ConfigESP->getGpio(FUNCTION_SDA_2) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL_2) != OFF_GPIO) {
    webPageI2CScanner(&Wire1);
  }
  addFormHeaderEnd();
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();

  addButton(S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorI2cSave() {
  String input;
  uint8_t key;

  if (!WebServer->saveGPIO(INPUT_SDA, FUNCTION_SDA) || !WebServer->saveGPIO(INPUT_SCL, FUNCTION_SCL)) {
    handleSensorI2c(6);
    return;
  }

#ifdef ARDUINO_ARCH_ESP32
  if (!WebServer->saveGPIO(INPUT_SDA_2, FUNCTION_SDA_2) || !WebServer->saveGPIO(INPUT_SCL_2, FUNCTION_SCL_2)) {
    handleSensorI2c(6);
    return;
  }
#endif

#ifdef SUPLA_BME280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BME280;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BME280,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  key = KEY_ALTITUDE_BMX280;
  input = INPUT_ALTITUDE_BMx280;
  if (strcmp(WebServer->httpServer->arg(INPUT_ALTITUDE_BMx280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_BMP280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BMP280;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BMP280,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  key = KEY_ALTITUDE_BMX280;
  input = INPUT_ALTITUDE_BMx280;
  if (strcmp(WebServer->httpServer->arg(INPUT_ALTITUDE_BMx280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_SHT3x
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SHT3x;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SHT3x,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_SHT_AUTODETECT
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SUPLA_SHT_AUTODETECT;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SHT3x,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_SI7021
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SI7021;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SI7021,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_VL53L0X
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_VL53L0X;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_VL53L0X,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_HDC1080
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_HDC1080;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_HDC1080,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_BH1750
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BH1750;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BH1750,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_MS5611
  key = KEY_ACTIVE_SENSOR_2;
  input = INPUT_MS5611;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR_2, SENSOR_I2C_MS5611,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  key = KEY_ALTITUDE_MS5611;
  input = INPUT_ALTITUDE_MS5611;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_MAX44009
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MAX44009;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_MAX44009,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_OLED
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_OLED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_OLED,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

#ifdef SUPLA_BUTTON
  input = INPUT_BUTTON_OLED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_NUMBER_BUTTON_ADDITIONAL, BUTTON_OLED,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

  input = INPUT_OLED_ANIMATION;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_ANIMATION, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_TIME;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_BACK_LIGHT_TIME, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_LVL;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    int value = WebServer->httpServer->arg(input).toInt();
ConfigESP->setBrightnessLevelOLED(value);
  }

  for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
    input = INPUT_OLED_NAME;
    input += i;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), ConfigManager->get(KEY_NAME_SENSOR)->getElement(i).c_str()) != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_LCD_HD44780
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_LCD;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_HD44780,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

#ifdef SUPLA_BUTTON
  input = INPUT_BUTTON_LCD;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_NUMBER_BUTTON_ADDITIONAL, BUTTON_LCD,  static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

  input = INPUT_HD44780_TYPE;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_HD44780_TYPE, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_ANIMATION;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_ANIMATION, WebServer->httpServer->arg(input).c_str());

  input = INPUT_OLED_BRIGHTNESS_TIME;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0)
    ConfigManager->set(KEY_OLED_BACK_LIGHT_TIME, WebServer->httpServer->arg(input).c_str());

  for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
    input = INPUT_OLED_NAME;
    input += i;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), ConfigManager->get(KEY_NAME_SENSOR)->getElement(i).c_str()) != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_ADE7953
  if (!WebServer->saveGPIO(INPUT_ADE7953_IRQ, FUNCTION_ADE7953_IRQ)) {
    handleSensorI2c(6);
    return;
  }
  else {
    Supla::GUI::addADE7953(ConfigESP->getGpio(FUNCTION_ADE7953_IRQ));
    if (strcmp(WebServer->httpServer->arg(INPUT_ADE7953_COUNTER_VALUE).c_str(), "") != 0) {
      Supla::GUI::couterADE7953->setCounter(WebServer->httpServer->arg(INPUT_ADE7953_COUNTER_VALUE).toFloat() * 100 * 1000);
      Supla::Storage::ScheduleSave(1000);
    }
  }
#endif

#ifdef SUPLA_AHTX0
  key = KEY_ACTIVE_SENSOR_2;
  input = INPUT_AHTX0;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR_2, SENSOR_I2C_AHTX0, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorI2c(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorI2c(2);
      break;
  }
}

#endif
