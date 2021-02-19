#include "SuplaOled.h"
#include "SuplaDeviceGUI.h"

#ifdef SUPLA_OLED

uint8_t* chanelSensor;

String getTempString(double temperature) {
  if (temperature == -275) {
    return F("error");
  }
  else {
    return String(temperature, 1);
  }
}

String getHumidityString(double humidity) {
  if (humidity == -1) {
    return F("error");
  }
  else {
    return String(humidity, 1);
  }
}

String getPressureString(double pressure) {
  if (pressure == -1) {
    return F("error");
  }
  else {
    return String(pressure, 0);
  }
}

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void displayUiSignal(OLEDDisplay* display) {
  int x = display->getWidth() - 17;
  int y = 0;
  int value = getQuality();

  display->setColor(BLACK);
  display->fillRect(x, y, x + 46, 16);
  display->setColor(WHITE);
  if (value == -1) {
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + 1, y, "x");
  }
  else {
    if (value > 0)
      display->fillRect(x, y + 6, 3, 4);

    if (value >= 25)
      display->fillRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display->fillRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display->fillRect(x + 12, y, 3, 10);
  }
}

void displayUiRelayState(OLEDDisplay* display) {
  int y = 0;
  int x = 0;

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 0; i < Supla::GUI::relay.size(); i++) {
    if (Supla::GUI::relay[i]->isOn()) {
      display->setColor(WHITE);
      display->fillRect(x, y + 1, 10, 10);
      display->setColor(BLACK);
      display->drawString(x + 2, y, String(i + 1));
    }
    else {
      display->setColor(WHITE);
      display->drawString(x + 2, y, String(i + 1));
    }
    x += 15;
  }
  display->setColor(WHITE);
  display->drawHorizontalLine(0, 14, display->getWidth());
}

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  displayUiSignal(display);
  if (Supla::GUI::relay.size()) {
    displayUiRelayState(display);
  }
}

void displayUiSuplaStatus(OLEDDisplay* display) {
  int x = 0;
  int y = display->getHeight() / 3;
  display->clear();

  displayUiSignal(display);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawStringMaxWidth(x, y, display->getWidth(), ConfigESP->supla_status.msg);
  display->display();
}

void displayConfigMode(OLEDDisplay* display) {
  display->clear();
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, F("Tryb konfiguracyjny"));
  display->drawString(0, 28, F("AP name:"));
  display->drawString(0, 41, ConfigESP->getConfigNameAP());
  display->drawString(0, 54, F("IP: 192.168.4.1"));
  display->display();
}

void displayUiBlank(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(10, display->getHeight() / 2, F("SUPLA"));
}

void displayUiTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double temp, const String& name) {
  uint8_t temp_width, temp_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    temp_width = 0;
    temp_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, TEMP_WIDTH, TEMP_HEIGHT, temp_bits);

    temp_width = TEMP_WIDTH + 10;
    temp_height = TEMP_HEIGHT;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + temp_width, y + drawStringIcon, getTempString(temp));
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + temp_width + (getTempString(temp).length() * 12), y + drawStringIcon, "ºC");
}

void displaUiHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double humidity, const String& name) {
  uint8_t humidity_width, humidity_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    humidity_width = 0;
    humidity_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, HUMIDITY_WIDTH, HUMIDITY_HEIGHT, humidity_bits);
    humidity_width = HUMIDITY_WIDTH + 20;
    humidity_height = HUMIDITY_HEIGHT;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + humidity_width, y + drawStringIcon, getHumidityString(humidity));
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + humidity_width + (getHumidityString(humidity).length() * 12), y + drawStringIcon, "%");
}

void displayUiPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y, double pressure, const String& name) {
  uint8_t pressure_width, pressure_height;

  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 6;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  if (display->getWidth() <= 64 || display->getHeight() <= 48) {
    pressure_width = 0;
    pressure_height = 0;
  }
  else {
    display->drawXbm(x + 0, y + drawHeightIcon, PRESSURE_WIDTH, PRESSURE_HEIGHT, pressure_bits);
    pressure_width = PRESSURE_WIDTH + 10;
    pressure_height = PRESSURE_HEIGHT;
  }

  if (name != NULL) {
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + TEMP_WIDTH + 20, y + display->getHeight() / 2 - 15, name);
  }

  display->setFont(ArialMT_Plain_24);
  display->drawString(x + pressure_width, y + drawStringIcon, getPressureString(pressure));
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + pressure_width + (getPressureString(pressure).length() * 14), y + drawStringIcon, "hPa");
}

void displayTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastTemperature = channel->getValueDouble();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiTemperature(display, state, x, y, lastTemperature, name);
      }
    }
  }
}

void displayDoubleTemperature(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastTemperature = channel->getValueDoubleFirst();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiTemperature(display, state, x, y, lastTemperature, name);
      }
    }
  }
}

void displayDoubleHumidity(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastHumidit = channel->getValueDoubleSecond();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displaUiHumidity(display, state, x, y, lastHumidit, name);
      }
    }
  }
}

void displayPressure(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelNumber() == chanelSensor[state->currentFrame]) {
        double lastPressure = channel->getValueDouble();
        String name = ConfigManager->get(KEY_NAME_SENSOR)->getElement(state->currentFrame);
        displayUiPressure(display, state, x, y, lastPressure, name);
      }
    }
  }
}

SuplaOled::SuplaOled() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    switch (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
      case OLED_SSD1306_0_96:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64, I2C_ONE, -1);
        break;
      case OLED_SH1106_1_3:
        display = new SH1106Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_128_64, I2C_ONE, -1);
        break;
      case OLED_SSD1306_0_66:
        display = new SSD1306Wire(0x3c, ConfigESP->getGpio(FUNCTION_SDA), ConfigESP->getGpio(FUNCTION_SCL), GEOMETRY_64_48, I2C_ONE, -1);
        break;
    }

    ui = new OLEDDisplayUi(display);

    overlays[0] = {msOverlay};
    int maxFrame = getCountSensorChannels();

    if (maxFrame == 0)
      maxFrame = 1;

    frames = new FrameCallback[maxFrame];
    chanelSensor = new uint8_t[maxFrame];

    for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
      if (element->getChannel()) {
        auto channel = element->getChannel();

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          frames[frameCount] = {displayTemperature};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          frames[frameCount] = {displayDoubleTemperature};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
          frames[frameCount] = {displayDoubleHumidity};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }
      }
      if (element->getSecondaryChannel()) {
        auto channel = element->getSecondaryChannel();
        if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
          frames[frameCount] = {displayPressure};
          chanelSensor[frameCount] = channel->getChannelNumber();
          frameCount += 1;
        }
      }
    }

    if (frameCount == 0) {
      frames[frameCount] = {displayUiBlank};
      frameCount += 1;
    }

    if (frameCount == 1) {
      ui->disableAllIndicators();
      ui->disableAutoTransition();
    }
    else {
      switch (ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt()) {
        case OLED_CONTROLL_NORMAL:
          ui->setTimePerFrame(5000);
          break;
        case OLED_CONTROLL_SLOW:
          ui->setTimePerFrame(10000);
          break;
        case OLED_CONTROLL_MANUAL:
          ui->disableAutoTransition();
          ui->setTimePerTransition(250);
          break;
      }
      ui->setTargetFPS(30);
      ui->setIndicatorPosition(BOTTOM);
      ui->setIndicatorDirection(LEFT_RIGHT);
      ui->setFrameAnimation(SLIDE_LEFT);
    }

    ui->setFrames(frames, frameCount);
    ui->setOverlays(overlays, overlaysCount);
    ui->init();

    display->setBrightness(255);
    display->flipScreenVertically();
  }
}

void SuplaOled::iterateAlways() {
  if (ConfigESP->getGpio(FUNCTION_SDA) != OFF_GPIO && ConfigESP->getGpio(FUNCTION_SCL) != OFF_GPIO) {
    if (ConfigESP->supla_status.status != STATUS_REGISTERED_AND_READY) {
      displayUiSuplaStatus(display);
      return;
    }

    if (millis() - timeLastChangeOled > (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() * 1000) && oledON &&
        ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
      display->setBrightness(50);
      oledON = false;
      // display.displayOff();
    }

    if (ConfigESP->configModeESP == NORMAL_MODE) {
      int remainingTimeBudget = ui->update();

      if (remainingTimeBudget > 0)
        delay(remainingTimeBudget);
    }
    else {
      displayConfigMode(display);
    }
  }
}

void SuplaOled::addButtonOled(int pin) {
  if (pin != OFF_GPIO) {
    Supla::Control::Button* button = new Supla::Control::Button(pin, true, true);

    if (ConfigManager->get(KEY_OLED_BACK_LIGHT_TIME)->getValueInt() != 0) {
      button->addAction(TURN_ON_OLED, this, Supla::ON_PRESS);
    }

    if (ConfigManager->get(KEY_OLED_ANIMATION)->getValueInt() == OLED_CONTROLL_MANUAL && frameCount > 1) {
      button->addAction(NEXT_FRAME, this, Supla::ON_PRESS);
    }
  }
}

void SuplaOled::handleAction(int event, int action) {
  if (action == NEXT_FRAME) {
    ui->nextFrame();
  }

  if (action == TURN_ON_OLED) {
    display->setBrightness(255);
    timeLastChangeOled = millis();
    oledON = true;
  }
}
#endif