#include "SuplaWebPageSensor.h"

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaCommonPROGMEM.h"
#include "GUIGenericCommon.h"
#include "Markup.h"

SuplaWebPageSensor *WebPageSensor = new SuplaWebPageSensor();

SuplaWebPageSensor::SuplaWebPageSensor() {
}

void SuplaWebPageSensor::createWebPageSensor() {
  String path;

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  path = PATH_START;
  path += PATH_1WIRE;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handle1Wire, this));
  path = PATH_START;
  path += PATH_SAVE_1WIRE;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handle1WireSave, this));
#ifdef SUPLA_DS18B20
  path = PATH_START;
  path += PATH_MULTI_DS;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handleSearchDS, this));
  path = PATH_START;
  path += PATH_SAVE_MULTI_DS;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handleDSSave, this));
#endif
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_MCP23017)
  path = PATH_START;
  path += PATH_I2C;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handlei2c, this));
  path = PATH_START;
  path += PATH_SAVE_I2C;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handlei2cSave, this));
#endif

#if defined(SUPLA_MAX6675)
  path = PATH_START;
  path += PATH_SPI;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handleSpi, this));
  path = PATH_START;
  path += PATH_SAVE_SPI;
  WebServer->httpServer->on(path, std::bind(&SuplaWebPageSensor::handleSpiSave, this));
#endif
}

#ifdef SUPLA_DS18B20
void SuplaWebPageSensor::handleSearchDS() {
if (!WebServer->isLoggedIn()) { return; }
  supla_webpage_search(0);
}

void SuplaWebPageSensor::handleDSSave() {
if (!WebServer->isLoggedIn()) { return; }
  for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
    String dsAddr = INPUT_DS18B20_ADDR;
    String dsName = INPUT_DS18B20_NAME;
    dsAddr += i;
    dsName += i;

    ConfigManager->setElement(KEY_ADDR_DS18B20, i, WebServer->httpServer->arg(dsAddr).c_str());

    if (strcmp(WebServer->httpServer->arg(dsName).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(dsName).c_str());
    }

    Supla::GUI::sensorDS[i]->setDeviceAddress(HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i)));
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      // Serial.println(F("E_CONFIG_OK: Config save"));
      supla_webpage_search(1);
      // WebServer->rebootESP();
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_search(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_search(int save) {
  uint8_t count = 0;
  uint8_t pin = ConfigESP->getGpio(FUNCTION_DS18B20);

  OneWire ow(pin);
  DallasTemperature sensors;
  DeviceAddress address;
  char strAddr[64];
  uint8_t i;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_MULTI_DS);
  webContentBuffer += F("<center>");
  if (ConfigESP->getGpio(FUNCTION_DS18B20) < OFF_GPIO) {
    webContentBuffer += F("<form method='post' action='");
    webContentBuffer += PATH_SAVE_MULTI_DS;
    webContentBuffer += F("'>");
    this->showDS18B20();
    webContentBuffer += F("<button type='submit'>");
    webContentBuffer += S_SAVE;
    webContentBuffer += F("</button></form>");
    webContentBuffer += F("<br>");
  }
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_MULTI_DS;
  webContentBuffer += F("'>");
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += S_FOUND;
  webContentBuffer += F(" DS18b20</h3>");
  sensors.setOneWire(&ow);
  sensors.begin();
  if (sensors.isParasitePowerMode()) {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is ON", pin);
  }
  else {
    supla_log(LOG_DEBUG, "OneWire(pin %d) Parasite power is OFF", pin);
  }
  // report parasite power requirements
  for (i = 0; i < sensors.getDeviceCount(); i++) {
    if (!sensors.getAddress(address, i)) {
      supla_log(LOG_DEBUG, "Unable to find address for Device %d", i);
    }
    else {
      sprintf(strAddr, "%02X%02X%02X%02X%02X%02X%02X%02X", address[0], address[1], address[2], address[3], address[4], address[5], address[6],
              address[7]);
      supla_log(LOG_DEBUG, "Index %d - address %s", i, strAddr);

      webContentBuffer += F("<i><input name='");
      webContentBuffer += INPUT_DS18B20_ADDR;
      webContentBuffer += count;

      webContentBuffer += F("' value='");
      webContentBuffer += String(strAddr);
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_ADDRESS_HEX;
      webContentBuffer += F(" readonly><label></i>");

      count++;
    }
    delay(0);
  }

  if (count == 0) {
    webContentBuffer += F("<i><label>");
    webContentBuffer += S_NO_SENSORS_CONNECTED;
    webContentBuffer += F("</label></i>");
  }
  webContentBuffer += F("</div>");
  webContentBuffer += F("</center>");
  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE_FOUND;
  webContentBuffer += F("DS18b20</button></form>");
  webContentBuffer += F("<br><br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_1WIRE;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}

void SuplaWebPageSensor::showDS18B20(bool readonly) {
  if (ConfigESP->getGpio(FUNCTION_DS18B20) != OFF_GPIO) {
    webContentBuffer += F("<div class='w'>");
    webContentBuffer += F("<h3>");
    webContentBuffer += S_TEMPERATURE;
    webContentBuffer += F("</h3>");
    for (uint8_t i = 0; i < ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt(); i++) {
      double temp = Supla::GUI::sensorDS[i]->getValue();
      webContentBuffer += F("<i style='border-bottom:none !important;'><input name='");
      webContentBuffer += INPUT_DS18B20_NAME;
      webContentBuffer += i;
      webContentBuffer += F("' value='");
      webContentBuffer += String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_NAME;
      if (readonly) {
        webContentBuffer += F(" readonly");
      }
      webContentBuffer += F("><label>");
      webContentBuffer += S_NAME;
      webContentBuffer += i + 1;
      webContentBuffer += F("</label></i>");
      webContentBuffer += F("<i><input name='");
      webContentBuffer += INPUT_DS18B20_ADDR;
      webContentBuffer += i;
      webContentBuffer += F("' value='");
      webContentBuffer += String(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i));
      webContentBuffer += F("' maxlength=");
      webContentBuffer += MAX_DS18B20_ADDRESS_HEX;
      if (readonly) {
        webContentBuffer += F(" readonly");
      }
      webContentBuffer += F("><label>");
      if (temp != -275) {
        webContentBuffer += temp;
      }
      else {
        webContentBuffer += F("--.--");
      }
      webContentBuffer += F(" <b>&deg;C</b> ");
      webContentBuffer += F("</label></i>");
      delay(0);
    }
    webContentBuffer += F("</div>");
  }
}
#endif

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
void SuplaWebPageSensor::handle1Wire() {
if (!WebServer->isLoggedIn()) { return; }
  supla_webpage_1wire(0);
}

void SuplaWebPageSensor::handle1WireSave() {
if (!WebServer->isLoggedIn()) { return; }

  uint8_t nr, last_value;

#ifdef SUPLA_DHT11
  last_value = ConfigManager->get(KEY_MAX_DHT11)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT11_GPIO, FUNCTION_DHT11, nr, INPUT_MAX_DHT11)) {
      supla_webpage_1wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DHT11).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT11, WebServer->httpServer->arg(INPUT_MAX_DHT11).c_str());
  }
#endif

#ifdef SUPLA_DHT22
  last_value = ConfigManager->get(KEY_MAX_DHT22)->getValueInt();
  for (nr = 1; nr <= last_value; nr++) {
    if (!WebServer->saveGPIO(INPUT_DHT22_GPIO, FUNCTION_DHT22, nr, INPUT_MAX_DHT22)) {
      supla_webpage_1wire(6);
      return;
    }
  }

  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DHT22).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DHT22, WebServer->httpServer->arg(INPUT_MAX_DHT22).c_str());
  }
#endif

#ifdef SUPLA_DS18B20
  if (!WebServer->saveGPIO(INPUT_MULTI_DS_GPIO, FUNCTION_DS18B20)) {
    supla_webpage_1wire(6);
    return;
  }
  if (strcmp(WebServer->httpServer->arg(INPUT_MAX_DS18B20).c_str(), "") > 0) {
    ConfigManager->set(KEY_MULTI_MAX_DS18B20, WebServer->httpServer->arg(INPUT_MAX_DS18B20).c_str());
  }
#endif

#ifdef SUPLA_SI7021_SONOFF
  if (!WebServer->saveGPIO(INPUT_SI7021_SONOFF, FUNCTION_SI7021_SONOFF)) {
    supla_webpage_1wire(6);
    return;
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_1wire(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_1wire(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_1wire(int save) {
  uint8_t nr, max;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_1WIRE);
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_1WIRE;
  webContentBuffer += F("'>");
#ifdef SUPLA_DHT11
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" DHT11"));
  addNumberBox(webContentBuffer, INPUT_MAX_DHT11, S_QUANTITY, KEY_MAX_DHT11, ConfigESP->countFreeGpio(FUNCTION_DHT11));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT11)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_DHT11_GPIO, F("DHT11"), FUNCTION_DHT11, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_DHT22
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" DHT22"));
  addNumberBox(webContentBuffer, INPUT_MAX_DHT22, S_QUANTITY, KEY_MAX_DHT22, ConfigESP->countFreeGpio(FUNCTION_DHT22));
  for (nr = 1; nr <= ConfigManager->get(KEY_MAX_DHT22)->getValueInt(); nr++) {
    addListGPIOBox(webContentBuffer, INPUT_DHT22_GPIO, F("DHT22"), FUNCTION_DHT22, nr);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SI7021_SONOFF
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" Si7021 Sonoff"));
  addListGPIOBox(webContentBuffer, INPUT_SI7021_SONOFF, F("Si7021 Sonoff"), FUNCTION_SI7021_SONOFF);
  addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_DS18B20
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" Multi DS18B20"));
  addNumberBox(webContentBuffer, INPUT_MAX_DS18B20, S_QUANTITY, KEY_MULTI_MAX_DS18B20, MAX_DS18B20);
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() > 1) {
    addListGPIOLinkBox(webContentBuffer, INPUT_MULTI_DS_GPIO, F("MULTI DS18B20"), FUNCTION_DS18B20, PATH_MULTI_DS);
  }
  else {
    addListGPIOBox(webContentBuffer, INPUT_MULTI_DS_GPIO, F("MULTI DS18B20"), FUNCTION_DS18B20);
  }
  addFormHeaderEnd(webContentBuffer);
#endif

  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE;
  webContentBuffer += F("</button></form>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_DEVICE_SETTINGS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");

  WebServer->sendContent();
}
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017)
void SuplaWebPageSensor::handlei2c() {
if (!WebServer->isLoggedIn()) { return; }
  supla_webpage_i2c(0);
}

void SuplaWebPageSensor::handlei2cSave() {
if (!WebServer->isLoggedIn()) { return; }

  String input;
  uint8_t key;

  if (!WebServer->saveGPIO(INPUT_SDA_GPIO, FUNCTION_SDA)) {
    supla_webpage_i2c(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_SCL_GPIO, FUNCTION_SCL)) {
    supla_webpage_i2c(6);
    return;
  }

#ifdef SUPLA_BME280
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_BME280;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_BME280, WebServer->httpServer->arg(input).toInt());
  }

  key = KEY_ALTITUDE_BME280;
  input = INPUT_ALTITUDE_BME280;
  if (strcmp(WebServer->httpServer->arg(INPUT_ALTITUDE_BME280).c_str(), "") != 0) {
    ConfigManager->set(key, WebServer->httpServer->arg(input).c_str());
  }
#endif

#ifdef SUPLA_SHT3x
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SHT3x;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SHT3x, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_SI7021
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_SI7021;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_SI7021, WebServer->httpServer->arg(input).toInt());
  }
#endif

#ifdef SUPLA_OLED
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_OLED;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_OLED, WebServer->httpServer->arg(input).toInt());
  }

  if (!WebServer->saveGPIO(INPUT_BUTTON_GPIO, FUNCTION_CFG_BUTTON)) {
    supla_webpage_i2c(6);
    return;
  }

  input = INPUT_OLED_ANIMATION;
  ConfigManager->set(KEY_OLED_ANIMATION, WebServer->httpServer->arg(input).c_str());
  input = INPUT_OLED_BRIGHTNESS;
  ConfigManager->set(KEY_OLED_BACK_LIGHT_TIME, WebServer->httpServer->arg(input).c_str());

  for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
    input = INPUT_OLED_NAME;
    input += i;
    if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
      ConfigManager->setElement(KEY_NAME_SENSOR, i, WebServer->httpServer->arg(input).c_str());
    }
  }
#endif

#ifdef SUPLA_MCP23017
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MCP23017;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_MCP23017, WebServer->httpServer->arg(input).toInt());

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt()) {
      ConfigESP->clearFunctionGpio(FUNCTION_RELAY);
      ConfigESP->clearFunctionGpio(FUNCTION_BUTTON);
    }
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_i2c(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_i2c(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_i2c(int save) {
  uint8_t selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_I2C);

  addForm(webContentBuffer, F("post"), PATH_SAVE_I2C);
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" i2c"));
  addListGPIOBox(webContentBuffer, INPUT_SDA_GPIO, F("SDA"), FUNCTION_SDA);
  addListGPIOBox(webContentBuffer, INPUT_SCL_GPIO, F("SCL"), FUNCTION_SCL);
  addFormHeaderEnd(webContentBuffer);

  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
#ifdef SUPLA_BME280
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_BME280).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_BME280, F("BME280 adres"), BME280_P, 4, selected);
    addNumberBox(webContentBuffer, INPUT_ALTITUDE_BME280, S_ALTITUDE_ABOVE_SEA_LEVEL, KEY_ALTITUDE_BME280, 1500);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SHT3x
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SHT3x).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SHT3x, F("SHT3x"), SHT3x_P, 4, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_SI7021
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_SI7021).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_SI7021, F("SI7021"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_OLED
    addFormHeader(webContentBuffer);

    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt();
    addListBox(webContentBuffer, INPUT_OLED, F("OLED"), OLED_P, 4, selected);

    if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      String name, sensorName, input;

      addListGPIOBox(webContentBuffer, INPUT_BUTTON_GPIO, F("PRZYCISK OLED"), FUNCTION_CFG_BUTTON);
      selected = ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt();
      addListBox(webContentBuffer, INPUT_OLED_ANIMATION, F("STEROWANIE"), OLED_CONTROLL_P, 3, selected);
      addNumberBox(webContentBuffer, INPUT_OLED_BRIGHTNESS, F("PODŚWIETLENIE[s]"), KEY_OLED_BACK_LIGHT_TIME, 99);

      for (uint8_t i = 0; i < getCountSensorChannels(); i++) {
        sensorName = String(ConfigManager->get(KEY_NAME_SENSOR)->getElement(i));
        input = INPUT_OLED_NAME;
        input += i;
        name = F("EKRAN ");
        name += i + 1;
        addTextBox(webContentBuffer, input, name, sensorName, 0, MAX_DS18B20_NAME, false);
      }
    }
    addFormHeaderEnd(webContentBuffer);
#endif

#ifdef SUPLA_MCP23017
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MCP23017).toInt();
    addFormHeader(webContentBuffer);
    addListBox(webContentBuffer, INPUT_MCP23017, F("MCP23017"), STATE_P, 2, selected);
    addFormHeaderEnd(webContentBuffer);
#endif
  }

  addButtonSubmit(webContentBuffer, S_SAVE);
  addFormEnd(webContentBuffer);

  addButton(webContentBuffer, S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendContent();
}
#endif

#if defined(SUPLA_MAX6675)
void SuplaWebPageSensor::handleSpi() {
if (!WebServer->isLoggedIn()) { return; }
  supla_webpage_spi(0);
}

void SuplaWebPageSensor::handleSpiSave() {
if (!WebServer->isLoggedIn()) { return; }

  String input;
  uint8_t key;

#if defined(SUPLA_MAX6675)
  if (!WebServer->saveGPIO(INPUT_CLK_GPIO, FUNCTION_CLK)) {
    supla_webpage_spi(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_CS_GPIO, FUNCTION_CS)) {
    supla_webpage_spi(6);
    return;
  }
  if (!WebServer->saveGPIO(INPUT_D0_GPIO, FUNCTION_D0)) {
    supla_webpage_spi(6);
    return;
  }
#endif

#ifdef SUPLA_MAX6675
  key = KEY_ACTIVE_SENSOR;
  input = INPUT_MAX6675;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->setElement(KEY_ACTIVE_SENSOR, SENSOR_I2C_MAX6675, WebServer->httpServer->arg(input).toInt());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      supla_webpage_spi(1);
      break;
    case E_CONFIG_FILE_OPEN:
      //      Serial.println(F("E_CONFIG_FILE_OPEN: Couldn't open file"));
      supla_webpage_spi(2);
      break;
  }
}

void SuplaWebPageSensor::supla_webpage_spi(int save) {
  uint8_t nr, suported, selected;

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_SPI);
  webContentBuffer += F("<form method='post' action='");
  webContentBuffer += PATH_SAVE_SPI;
  webContentBuffer += F("'>");

#if defined(SUPLA_MAX6675)
  addFormHeader(webContentBuffer, String(S_GPIO_SETTINGS_FOR) + F(" SPI"));
  addListGPIOBox(webContentBuffer, INPUT_CLK_GPIO, F("CLK"), FUNCTION_CLK);
  addListGPIOBox(webContentBuffer, INPUT_CS_GPIO, F("CS"), FUNCTION_CS);
  addListGPIOBox(webContentBuffer, INPUT_D0_GPIO, F("D0"), FUNCTION_D0);

  if (ConfigESP->getGpio(FUNCTION_CLK) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_CS) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_D0) != OFF_GPIO) {
    selected = ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_MAX6675).toInt();
    addListBox(webContentBuffer, INPUT_MAX6675, F("MAX6675/MAX31855"), STATE_P, 2, selected);
  }
  addFormHeaderEnd(webContentBuffer);
#endif
  webContentBuffer += F("<button type='submit'>");
  webContentBuffer += S_SAVE;
  webContentBuffer += F("</button></form>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_START;
  webContentBuffer += PATH_DEVICE_SETTINGS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}
#endif