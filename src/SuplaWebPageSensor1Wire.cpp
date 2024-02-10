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

#include "SuplaWebPageSensor1Wire.h"

#ifdef GUI_SENSOR_1WIRE
void createWebPageSensor1Wire() {
  WebServer->httpServer->on(getURL(PATH_1WIRE), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensor1Wire();
    else
      handleSensor1WireSave();
  });

#ifdef SUPLA_DS18B20
  WebServer->httpServer->on(getURL(PATH_MULTI_DS), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorDs18b20();
    else
      handleSensorDs18b20Save();
  });
#endif
}

void handleSensor1Wire(int save) {
  uint8_t nr;

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_1WIRE);
  addForm(F("post"), PATH_1WIRE);
#ifdef SUPLA_DHT11
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_DHT11);
  addNumberBox(INPUT_MAX_DHT11, S_QUANTITY, KEY_MAX_DHT11, ConfigESP->countFreeGpio(FUNCTION_DHT11));
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    addListGPIOBox(INPUT_DHT11_GPIO, S_DHT11, FUNCTION_DHT11, nr);
  }
  addFormHeaderEnd();
#endif

#ifdef SUPLA_DHT22
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_DHT22);
  addNumberBox(INPUT_MAX_DHT22, S_QUANTITY, KEY_MAX_DHT22, ConfigESP->countFreeGpio(FUNCTION_DHT22));
  for (nr = 0; nr < ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    addListGPIOBox(INPUT_DHT22_GPIO, S_DHT22, FUNCTION_DHT22, nr);
  }
  addFormHeaderEnd();
#endif

#ifdef SUPLA_SI7021_SONOFF
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_SI7021_SONOFF);
  addListGPIOBox(INPUT_SI7021_SONOFF, S_SI7021_SONOFF, FUNCTION_SI7021_SONOFF);
  addFormHeaderEnd();
#endif

#ifdef SUPLA_DS18B20
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_DS18B20);
  addNumberBox(INPUT_MAX_DS18B20, S_QUANTITY, KEY_MULTI_MAX_DS18B20, MAX_DS18B20);
  addListGPIOBox(INPUT_MULTI_DS_GPIO, S_SENSORS_1WIRE, FUNCTION_DS18B20, 0, false, "", true);

  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    addLinkBox(String(S_CONFIGURATION) + S_SPACE + S_DS18B20, PATH_MULTI_DS);
  }

  addFormHeaderEnd();
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleSensor1WireSave() {
  uint8_t nr, last_value;

#ifdef SUPLA_DHT11
  last_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  for (nr = 0; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT11_GPIO, FUNCTION_DHT11, nr, INPUT_MAX_DHT11)) {
      handleSensor1Wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DHT11).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT11, WebServer->httpServer->arg(INPUT_MAX_DHT11).c_str());
  }
#endif

#ifdef SUPLA_DHT22
  last_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  for (nr = 0; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT22_GPIO, FUNCTION_DHT22, nr, INPUT_MAX_DHT22)) {
      handleSensor1Wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer->arg(INPUT_MAX_DHT22).c_str());
  }
#endif

#ifdef SUPLA_DS18B20
  int inputMaxDS18B20 = WebServer->httpServer->arg(INPUT_MAX_DS18B20).toInt();
  int oldMaxDS18B20 = ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt();

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DS18B20).c_str(), "") != 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer->arg(INPUT_MAX_DS18B20).c_str());
  }

  if (!WebServer->saveGPIO(INPUT_MULTI_DS_GPIO, FUNCTION_DS18B20)) {
    handleSensor1Wire(6);
    return;
  }
  else {
    if (strcmp(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(0).c_str(), "") == 0) {
      findAndSaveDS18B20Addresses();
    }
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (!WebServer->saveGPIO(INPUT_SI7021_SONOFF, FUNCTION_SI7021_SONOFF)) {
    handleSensor1Wire(6);
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:

#ifdef SUPLA_DS18B20
      if (inputMaxDS18B20 > oldMaxDS18B20 && ConfigESP->configModeESP != Supla::DEVICE_MODE_CONFIG) {
        handleSensor1Wire(SaveResult::DATA_SAVED_RESTART_MODULE);
        ConfigESP->rebootESP();
      }
#endif

      handleSensor1Wire(SaveResult::DATA_SAVE);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensor1Wire(2);
      break;
  }
}

#ifdef SUPLA_DS18B20
void handleSensorDs18b20(int save) {
  uint8_t count = 0;
  uint8_t pin = ConfigESP->getGpio(FUNCTION_DS18B20);

  OneWire ow(pin);
  DallasTemperature sensors;
  DeviceAddress address;
  char strAddr[64];
  uint8_t i;

  double temp;
  String s_temp;
  s_temp.reserve(12);

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(getURL(PATH_MULTI_DS));

  if (ConfigESP->getGpio(FUNCTION_DS18B20) < OFF_GPIO) {
    addForm(F("post"), getURL(PATH_MULTI_DS));

    if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
      addFormHeader(S_TEMPERATURE);
      for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
        temp = Supla::GUI::sensorDS[i]->getValue();

        addTextBox(getInput(INPUT_DS18B20_NAME, i), String(S_NAME) + (i + 1), ConfigManager->get(KEY_NAME_SENSOR)->getElement(i), emptyString, 0,
                   MAX_DS18B20_NAME, false, false, false, false);

        s_temp = emptyString;
        if (temp != -275) {
          s_temp += temp;
        }
        else {
          s_temp += F("--.--");
        }
        s_temp += F(" <b>&deg;C</b> ");

        addTextBox(getInput(INPUT_DS18B20_ADDR, i), s_temp, ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i), emptyString, 0,
                   MAX_DS18B20_ADDRESS_HEX, false);
        delay(0);
      }
      addFormHeaderEnd();
    }

    addButtonSubmit(S_SAVE);
    addFormEnd();
    addBr();
  }

  addForm(F("post"), getURL(PATH_MULTI_DS));
  addFormHeader(String(S_FOUND) + S_SPACE + S_DS18B20);
  sensors.setOneWire(&ow);
  sensors.begin();
  if (sensors.isParasitePowerMode()) {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is ON", pin);
  }
  else {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is OFF", pin);
  }
  for (i = 0; i < sensors.getDeviceCount(); i++) {
    if (!sensors.getAddress(address, i)) {
      supla_log(LOG_DEBUG, "Unable to find address for Device %d", i);
    }
    else {
      sprintf(strAddr, "%02X%02X%02X%02X%02X%02X%02X%02X", address[0], address[1], address[2], address[3], address[4], address[5], address[6],
              address[7]);
      supla_log(LOG_DEBUG, "Index %d - address %s", i, strAddr);

      addTextBox(getInput(INPUT_DS18B20_ADDR, count), emptyString, String(strAddr), emptyString, 0, MAX_DS18B20_ADDRESS_HEX, false, true);
      count++;
    }
    delay(0);
  }

  if (count == 0) {
    addLabel(S_NO_SENSORS_CONNECTED);
  }
  addFormHeaderEnd();

  addButtonSubmit(String(S_SAVE_FOUND) + S_DS18B20);
  addFormEnd();
  addButton(S_RETURN, PATH_1WIRE);
  WebServer->sendHeaderEnd();
}

void handleSensorDs18b20Save() {
  for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
    String dsAddr = INPUT_DS18B20_ADDR;
    String dsName = INPUT_DS18B20_NAME;
    dsAddr += i;
    dsName += i;

    ConfigManager->setElement(KEY_ADDR_DS18B20, i, WebServer->httpServer->arg(dsAddr).c_str());
    ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(dsName).c_str());
    if (Supla::GUI::sensorDS[i] != nullptr) {
      Supla::GUI::sensorDS[i]->setDeviceAddress(HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i)));
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorDs18b20(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorDs18b20(2);
      break;
  }
}
#endif

#endif