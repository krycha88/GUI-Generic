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

#include "SuplaWebPageSensorSpi.h"

#ifdef GUI_SENSOR_SPI
void createWebPageSensorSpi() {
  WebServer->httpServer->on(getURL(PATH_SPI), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorSpi();
    else
      handleSensorSpiSave();
  });
}

void handleSensorSpi(int save) {
  uint8_t selected;

  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_SPI);

  addForm(F("post"), PATH_SPI);
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_SPI);
  addListGPIOBox(INPUT_CLK_GPIO, S_CLK, FUNCTION_CLK);
  addListGPIOBox(INPUT_CS_GPIO, S_CS, FUNCTION_CS);
  addListGPIOBox(INPUT_MISO_GPIO, S_MISO, FUNCTION_MISO);
  addListGPIOBox(INPUT_MOSI_GPIO, S_MOSI, FUNCTION_MOSI);
  addFormHeaderEnd();

#if defined(SUPLA_MAX6675) || defined(SUPLA_MAX31855)
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + "MAX31855/6675");
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_MISO) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_MOSI) != OFF_GPIO) {
#ifdef SUPLA_MAX6675
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX6675).toInt();
    addListBox(INPUT_MAX6675, S_MAX6675, STATE_P, 2, selected);
#endif
#ifdef SUPLA_MAX31855
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX31855).toInt();
    addListBox(INPUT_MAX31855, S_MAX31855, STATE_P, 2, selected);
#endif
  }
  addFormHeaderEnd();
#endif

#ifdef SUPLA_CC1101
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + "CC1101");

  addListGPIOBox(INPUT_GDO0_GPIO, S_GDO0, FUNCTION_GDO0);
  addListGPIOBox(INPUT_GDO2_GPIO, S_GDO2, FUNCTION_GDO2);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_MISO) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_MOSI) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_GDO0) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_GDO2) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_SPI_CC1101).toInt();
    addListBox(INPUT_CC1101, S_CC1101, STATE_P, 2, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_SPI_CC1101).toInt()) {
      addFormHeader(String(S_WMBUS_METER) + " 1");
      // first sensor
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_ENABLED1).toInt();
      addCheckBox(INPUT_WMBUS_SENSOR_ENABLED1, S_ON, selected);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_TYPE1).toInt();
      addListBox(INPUT_WMBUS_SENSOR_TYPE1, S_WMBUS_SENSOR_TYPE, sensors_types, 27, selected);
      String sensorId1 = ConfigManager->get(KEY_WMBUS_SENSOR_ID)->getElement(0);
      addTextBox(INPUT_WMBUS_SENSOR_ID1, S_WMBUS_SENSOR_ID, sensorId1.c_str(), 1, 9, false);
      String sensorKey1 = ConfigManager->get(KEY_WMBUS_SENSOR_KEY)->getElement(0);
      addTextBox(INPUT_WMBUS_SENSOR_KEY1, S_WMBUS_SENSOR_KEY, sensorKey1.c_str(), 1, 50, false);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_PROPERTY1).toInt();  
      addListBox(INPUT_WMBUS_SENSOR_PROP1, S_WMBUS_SENSOR_PROP, sensors_properties, 24, selected);
      addFormHeaderEnd();

      // second sensor
      addFormHeader(String(S_WMBUS_METER) + " 2");
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_ENABLED2).toInt();
      addCheckBox(INPUT_WMBUS_SENSOR_ENABLED2, S_ON, selected);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_TYPE2).toInt();
      addListBox(INPUT_WMBUS_SENSOR_TYPE2, S_WMBUS_SENSOR_TYPE, sensors_types, 27, selected);
      String sensorId2 = ConfigManager->get(KEY_WMBUS_SENSOR_ID)->getElement(1);
      addTextBox(INPUT_WMBUS_SENSOR_ID2, S_WMBUS_SENSOR_ID, sensorId2.c_str(), 1, 9, false);
      String sensorKey2 = ConfigManager->get(KEY_WMBUS_SENSOR_KEY)->getElement(1);
      addTextBox(INPUT_WMBUS_SENSOR_KEY2, S_WMBUS_SENSOR_KEY, sensorKey2.c_str(), 1, 50, false);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_PROPERTY2).toInt();  
      addListBox(INPUT_WMBUS_SENSOR_PROP2, S_WMBUS_SENSOR_PROP, sensors_properties, 24, selected);
      addFormHeaderEnd();

      // third sensor
      addFormHeader(String(S_WMBUS_METER) + " 3");
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_ENABLED3).toInt();
      addCheckBox(INPUT_WMBUS_SENSOR_ENABLED3, S_ON, selected);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_TYPE3).toInt();
      addListBox(INPUT_WMBUS_SENSOR_TYPE3, S_WMBUS_SENSOR_TYPE, sensors_types, 27, selected);
      String sensorId3 = ConfigManager->get(KEY_WMBUS_SENSOR_ID)->getElement(2);
      addTextBox(INPUT_WMBUS_SENSOR_ID3, S_WMBUS_SENSOR_ID, sensorId3.c_str(), 1, 9, false);
      String sensorKey3 = ConfigManager->get(KEY_WMBUS_SENSOR_KEY)->getElement(2);
      addTextBox(INPUT_WMBUS_SENSOR_KEY3, S_WMBUS_SENSOR_KEY, sensorKey3.c_str(), 1, 50, false);
      selected = ConfigManager->get(KEY_WMBUS_SENSOR)->getElement(WMBUS_CFG_SENSOR_PROPERTY3).toInt();  
      addListBox(INPUT_WMBUS_SENSOR_PROP3, S_WMBUS_SENSOR_PROP, sensors_properties, 24, selected);
      addFormHeaderEnd();
    }
  }
  addFormHeaderEnd();
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorSpiSave() {
  String input;

  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK) || !WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS) ||
      !WebServer->saveGPIO(INPUT_MISO_GPIO, FUNCTION_MISO) || !WebServer->saveGPIO(INPUT_MOSI_GPIO, FUNCTION_MOSI)
#ifdef SUPLA_CC1101

      || !WebServer->saveGPIO(INPUT_GDO0_GPIO, FUNCTION_GDO0) || !WebServer->saveGPIO(INPUT_GDO2_GPIO, FUNCTION_GDO2)
#endif
  ) 
  {
    handleSensorSpi(6);
    return;
  }

#ifdef SUPLA_MAX6675
  input = INPUT_MAX6675;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_SPI_MAX6675, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_MAX31855
  input = INPUT_MAX31855;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_SPI_MAX31855, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
#endif

#ifdef SUPLA_CC1101

  input = INPUT_CC1101;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR_2, SENSOR_SPI_CC1101, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  // first sensor is always enabled
  ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_ENABLED1, 1);
  input = INPUT_WMBUS_SENSOR_TYPE1;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_TYPE1, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }
 
  input = INPUT_WMBUS_SENSOR_ID1;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR_ID, 0, WebServer->httpServer->arg(input).c_str());
  }

  input = INPUT_WMBUS_SENSOR_KEY1;
  ConfigManager->setElement(KEY_WMBUS_SENSOR_KEY, 0, WebServer->httpServer->arg(input).c_str());

  input = INPUT_WMBUS_SENSOR_PROP1;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_PROPERTY1, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  // second sensor is optional
  input = INPUT_WMBUS_SENSOR_ENABLED2;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_ENABLED2, 1);
    input = INPUT_WMBUS_SENSOR_TYPE2;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_TYPE2, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    }
  
    input = INPUT_WMBUS_SENSOR_ID2;
    ConfigManager->setElement(KEY_WMBUS_SENSOR_ID, 1, WebServer->httpServer->arg(input).c_str());

    input = INPUT_WMBUS_SENSOR_KEY2;
    ConfigManager->setElement(KEY_WMBUS_SENSOR_KEY, 1, WebServer->httpServer->arg(input).c_str());

    input = INPUT_WMBUS_SENSOR_PROP2;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_PROPERTY2, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    }
  }
  else
  {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_ENABLED2, 0);
  }

  // third sensor is optional
  input = INPUT_WMBUS_SENSOR_ENABLED3;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_ENABLED3, 1);
    input = INPUT_WMBUS_SENSOR_TYPE3;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_TYPE3, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    }
  
    input = INPUT_WMBUS_SENSOR_ID3;
    ConfigManager->setElement(KEY_WMBUS_SENSOR_ID, 2, WebServer->httpServer->arg(input).c_str());

    input = INPUT_WMBUS_SENSOR_KEY3;
    ConfigManager->setElement(KEY_WMBUS_SENSOR_KEY, 2, WebServer->httpServer->arg(input).c_str());

    input = INPUT_WMBUS_SENSOR_PROP3;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_PROPERTY3, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    }
  }
  else
  {
    ConfigManager->setElement(KEY_WMBUS_SENSOR, WMBUS_CFG_SENSOR_ENABLED3, 0);
  }

#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorSpi(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorSpi(2);
      break;
  }
}
#endif