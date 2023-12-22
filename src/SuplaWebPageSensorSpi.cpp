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
  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SPI);

  addForm(webContentBuffer, F("post"), PATH_SPI);
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_SPI);
  addListGPIOBox(webContentBuffer, INPUT_CLK_GPIO, S_CLK, FUNCTION_CLK);
  addListGPIOBox(webContentBuffer, INPUT_CS_GPIO, S_CS, FUNCTION_CS);
  addListGPIOBox(webContentBuffer, INPUT_MISO_GPIO, S_MISO, FUNCTION_MISO);  // MISO
  addListGPIOBox(webContentBuffer, INPUT_MOSI_GPIO, S_MOSI, FUNCTION_MOSI);
  addFormHeaderEnd(webContentBuffer);

#if defined(SUPLA_MAX6675) || defined(SUPLA_MAX31855)
  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_MISO) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_MOSI) != OFF_GPIO) {
#ifdef SUPLA_MAX6675
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX6675).toInt();
    addListBox(webContentBuffer, INPUT_MAX6675, S_MAX6675, STATE_P, 2, selected);
#endif

#ifdef SUPLA_MAX31855
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_SPI_MAX31855).toInt();
    addListBox(webContentBuffer, INPUT_MAX31855, S_MAX31855, STATE_P, 2, selected);
#endif
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_CC1101
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + S_SPACE + "CC1101");

  addListGPIOBox(webContentBuffer, INPUT_GDO0_GPIO, S_GDO0, FUNCTION_GDO0);
  addListGPIOBox(webContentBuffer, INPUT_GDO2_GPIO, S_GDO2, FUNCTION_GDO2);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_MISO) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_MOSI) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_GDO0) != OFF_GPIO &&
      ConfigESP->getGpio(FUNCTION_GDO2) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_SPI_CC1101).toInt();
    addListBox(webContentBuffer, INPUT_CC1101, S_CC1101, STATE_P, 2, selected);
    if (ConfigManager->get(KEY_ACTIVE_SENSOR_2)->getElement(SENSOR_SPI_CC1101).toInt()) {
      selected = ConfigManager->get(KEY_WMBUS_SENSOR_TYPE)->getElement(WMBUS_CFG_SENSOR_TYPE).toInt();
      addListBox(webContentBuffer, INPUT_WMBUS_SENSOR_TYPE, S_WMBUS_SENSOR_TYPE, sensors_types, 27, selected);
      std::string sensorId = ConfigManager->get(KEY_WMBUS_SENSOR_ID)->getValue();
      addTextBox(webContentBuffer, INPUT_WMBUS_SENSOR_ID, S_WMBUS_SENSOR_ID, sensorId.c_str(), 1, 100, true);
      std::string sensorKey = ConfigManager->get(KEY_WMBUS_SENSOR_KEY)->getValue();
      addTextBox(webContentBuffer, INPUT_WMBUS_SENSOR_KEY, S_WMBUS_SENSOR_KEY, sensorKey.c_str(), 1, 200, false);
    }
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorSpiSave() {
  String input;

  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK) || !WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS) ||
      !WebServer->saveGPIO(INPUT_MISO_GPIO, FUNCTION_MISO) || !WebServer->saveGPIO(INPUT_MOSI_GPIO, FUNCTION_MOSI)
#ifdef SUPLA_CC1101

      || !WebServer->saveGPIO(INPUT_GDO0_GPIO, FUNCTION_GDO0) || !WebServer->saveGPIO(INPUT_GDO2_GPIO, FUNCTION_GDO2)
#endif
  ) {
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
  input = INPUT_WMBUS_SENSOR_TYPE;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_WMBUS_SENSOR_TYPE, WMBUS_CFG_SENSOR_TYPE, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
  }

  input = INPUT_WMBUS_SENSOR_ID;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->set(KEY_WMBUS_SENSOR_ID, WebServer->httpServer->arg(input).c_str());
  }

  input = INPUT_WMBUS_SENSOR_KEY;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->set(KEY_WMBUS_SENSOR_KEY, WebServer->httpServer->arg(input).c_str());
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