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

#include "SuplaTemplateBoard.h"
#include "SuplaDeviceGUI.h"

namespace Supla {
namespace TanplateBoard {

void addTemplateBoard() {
#ifdef TEMPLATE_BOARD_JSON
#ifdef TEMPLATE_JSON
  chooseTemplateBoard(TEMPLATE_JSON);
  ConfigManager->set(KEY_BOARD, true);
#endif
#elif defined(TEMPLATE_BOARD_OLD)
  chooseTemplateBoard(ConfigESP->getDefaultTamplateBoard());
#endif
}
}  // namespace TanplateBoard
}  // namespace Supla

#ifdef TEMPLATE_BOARD_JSON
namespace Supla {
namespace TanplateBoard {

String templateBoardWarning;
bool oldVersion = false;

void chooseTemplateBoard(String board) {
  const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(4) + 200;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(board);
  JsonArray& GPIO = root["GPIO"];

  if (GPIO.size() == 0) {
    templateBoardWarning += "Błąd wczytania<br>";
    return;
  }

  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();
  templateBoardWarning = "";

  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_VIRTUAL_RELAY, "");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);
  ConfigManager->set(KEY_ANALOG_BUTTON, "");
  ConfigManager->set(KEY_ANALOG_INPUT_EXPECTED, "");

  ConfigManager->set(KEY_MAX_CONDITIONS, "0");
  ConfigManager->set(KEY_CONDITIONS_CLIENT_TYPE, "");
  ConfigManager->set(KEY_CONDITIONS_CLIENT_TYPE_NUMBER, "");
  ConfigManager->set(KEY_CONDITIONS_SENSOR_TYPE, "");
  ConfigManager->set(KEY_CONDITIONS_SENSOR_NUMBER, "");
  ConfigManager->set(KEY_CONDITIONS_TYPE, "");
  ConfigManager->set(KEY_CONDITIONS_MIN, "");
  ConfigManager->set(KEY_CONDITIONS_MAX, "");

  //"BTNACTION":[0,1,2]
  // 0 - Supla::Action::TURN_ON
  // 1 - Supla::Action::TURN_OFF
  // 2 - Supla::Action::TOGGLE
  JsonArray& buttonAction = root["BTNACTION"];

  //"BTNADC":[250, 500, 750]
  JsonArray& analogButtons = root["BTNADC"];
  for (size_t i = 0; i < analogButtons.size(); i++) {
    int expected = analogButtons[i];

    if (expected != 0) {
      ConfigManager->setElement(KEY_ANALOG_BUTTON, i, true);
      ConfigManager->setElement(KEY_ANALOG_INPUT_EXPECTED, i, expected);
    }

    addButtonAnalog(i, A0, buttonAction);
    // addButton(i, A0, Supla::Event::ON_CHANGE, buttonAction, true, true);
  }

  // {"NAME":"Shelly 2.5","GPIO":[320,0,32,0,224,193,0,0,640,192,608,225,3456,4736],"COND":[[0,0,10,0,0,"",90],[0,1,10,0,0,"",90]]}
  // "COND":[typeExecuitive, numberExecuitive,typeSensor,numberSensor,condition,valueON,valueOFF]
  //"typeExecuitive"
  // 0 - EXECUTIVE_RELAY
  // 1 - EXECUTIVE_RGBW

  //"type"
  // 0 - NO_SENSORS
  // 1 - SENSOR_DS18B20
  // 2 - SENSOR_DHT11
  // 3 - SENSOR_DHT22
  // 4 - SENSOR_SI7021_SONOFF
  // 5 - SENSOR_HC_SR04
  // 6 - SENSOR_BME280
  // 7 - SENSOR_SHT3x
  // 8 - SENSOR_SI7021
  // 9 - SENSOR_MAX6675
  // 10 - SENSOR_NTC_10K
  // 11 - SENSOR_BMP280
  // 12 - SENSOR_MPX_5XXX
  // 13 - SENSOR_MPX_5XXX_PERCENT
  // 14 - SENSOR_ANALOG_READING_MAP
  // 15 - SENSOR_VL53L0X
  // 16 - SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR
  // 17 - SENSOR_HDC1080
  // 18 - SENSOR_HLW8012
  // 19 - SENSOR_PZEM_V3
  // 20 - SENSOR_BINARY

  //"condition"
  //  0 - CONDITION_ON_LESS
  //  1 - CONDITION_ON_GREATER
  //  2 - CONDITION_ON_LESS_HUMIDITY
  //  3 - CONDITION_ON_GREATER_HUMIDITY
  //  4 - CONDITION_ON_LESS_VOLTAGE
  //  5 - CONDITION_ON_LESS_CURRENT
  //  6 - CONDITION_ON_LESS_POWER_ACTIVE
  //  7 - CONDITION_GPIO

  // "COND":[typeExecuitive, numberExecuitive,typeSensor,numberSensor,condition,valueON,valueOFF]

  JsonArray& conditions = root["COND"];

  for (size_t i = 0; i < conditions.size(); i++) {
    uint8_t maxConditions = ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt();

    ConfigManager->setElement(KEY_CONDITIONS_CLIENT_TYPE, maxConditions, (int)conditions[i][0]);         // typeExecuitive
    ConfigManager->setElement(KEY_CONDITIONS_CLIENT_TYPE_NUMBER, maxConditions, (int)conditions[i][1]);  // numberExecuitive
    ConfigManager->setElement(KEY_CONDITIONS_SENSOR_TYPE, maxConditions, (int)conditions[i][2]);         // typeSensor
    ConfigManager->setElement(KEY_CONDITIONS_SENSOR_NUMBER, maxConditions, (int)conditions[i][3]);       // numberSensor
    ConfigManager->setElement(KEY_CONDITIONS_TYPE, maxConditions, (int)conditions[i][4]);                // condition

    if (strcmp(conditions[i][5], "") != 0) {
      ConfigManager->setElement(KEY_CONDITIONS_MIN, maxConditions, (const char*)conditions[i][5]);  // "valueON"
    }

    if (strcmp(conditions[i][6], "") != 0) {
      ConfigManager->setElement(KEY_CONDITIONS_MAX, maxConditions, (const char*)conditions[i][6]);  // "valueOFF"
    }

    ConfigManager->set(KEY_MAX_CONDITIONS, maxConditions + 1);
  }

  String name = root["NAME"];
  ConfigManager->set(KEY_HOST_NAME, name.c_str());

#ifdef ARDUINO_ARCH_ESP8266
  if (GPIO.size() == 13) {
    oldVersion = true;

    int flag = root["FLAG"];
    switch (flag) {
      case FlagTemperatureAnalog:
        ConfigESP->setGpio(A0, FUNCTION_NTC_10K);
        break;
    }
  }

  templateBoardWarning += F("Wersja: ");
  if (oldVersion)
    templateBoardWarning += 1;
  else
    templateBoardWarning += 2;
  templateBoardWarning += F("<br>");
#elif ARDUINO_ARCH_ESP32

#endif

  for (size_t i = 0; i < GPIO.size(); i++) {
    int gpioJSON = (int)GPIO[i];
    int gpio = getGPIO(i);

#ifdef ARDUINO_ARCH_ESP8266
    if (oldVersion)
      gpioJSON = convert(gpioJSON);
#endif

    switch (gpioJSON) {
      case NewNone:
        break;
      case NewUsers:
        break;

      case NewI2CSCL:
        ConfigESP->setGpio(gpio, FUNCTION_SCL);
        break;
      case NewI2CSDA:
        ConfigESP->setGpio(gpio, FUNCTION_SDA);
        break;

      case NewI2CSCL2:
        ConfigESP->setGpio(gpio, FUNCTION_SCL_2);
        break;
      case NewI2CSDA2:
        ConfigESP->setGpio(gpio, FUNCTION_SDA_2);
        break;

      case NewRelay1:
        addRelay(0, gpio);
        break;
      case NewRelay2:
        addRelay(1, gpio);
        break;
      case NewRelay3:
        addRelay(2, gpio);
        break;
      case NewRelay4:
        addRelay(3, gpio);
        break;
      case NewRelay5:
        addRelay(4, gpio);
        break;
      case NewRelay6:
        addRelay(5, gpio);
        break;
      case NewRelay7:
        addRelay(6, gpio);
        break;
      case NewRelay8:
        addRelay(7, gpio);
        break;
      case NewRelay1i:
        addRelay(0, gpio, LOW);
        break;
      case NewRelay2i:
        addRelay(1, gpio, LOW);
        break;
      case NewRelay3i:
        addRelay(2, gpio, LOW);
        break;
      case NewRelay4i:
        addRelay(3, gpio, LOW);
        break;
      case NewRelay5i:
        addRelay(4, gpio, LOW);
        break;
      case NewRelay6i:
        addRelay(5, gpio, LOW);
        break;
      case NewRelay7i:
        addRelay(6, gpio, LOW);
        break;
      case NewRelay8i:
        addRelay(7, gpio, LOW);
        break;
      case NewSwitch1:
        if (ConfigESP->getGpio(0, FUNCTION_BUTTON) != OFF_GPIO) {
          ConfigESP->clearGpio(ConfigESP->getGpio(0, FUNCTION_BUTTON), FUNCTION_BUTTON);
          ConfigManager->set(KEY_MAX_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - 1);
        }
        else {
          addButtonCFG(gpio);
        }

        addButton(0, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);

        break;
      case NewSwitch2:
        addButton(1, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;
      case NewSwitch3:
        addButton(2, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;
      case NewSwitch4:
        addButton(3, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;
      case NewSwitch5:
        addButton(4, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;
      case NewSwitch6:
        addButton(5, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;
      case NewSwitch7:
        addButton(6, gpio, Supla::Event::ON_CHANGE, buttonAction, true, true);
        break;

      case NewSwitch1n:
        if (ConfigESP->getGpio(0, FUNCTION_BUTTON) != OFF_GPIO) {
          ConfigESP->clearGpio(ConfigESP->getGpio(0, FUNCTION_BUTTON), FUNCTION_BUTTON);
          ConfigManager->set(KEY_MAX_BUTTON, ConfigManager->get(KEY_MAX_BUTTON)->getValueInt() - 1);
        }
        else {
          addButtonCFG(gpio);
        }

        addButton(0, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);

        break;
      case NewSwitch2n:
        addButton(1, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;
      case NewSwitch3n:
        addButton(2, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;
      case NewSwitch4n:
        addButton(3, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;
      case NewSwitch5n:
        addButton(4, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;
      case NewSwitch6n:
        addButton(5, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;
      case NewSwitch7n:
        addButton(6, gpio, Supla::Event::ON_CHANGE, buttonAction, false, false);
        break;

      case NewButton1:
        addButton(0, gpio, Supla::Event::ON_PRESS, buttonAction, true, true);
        addButtonCFG(gpio);
        break;
      case NewButton2:
        addButton(1, gpio, Supla::Event::ON_PRESS, buttonAction, true, true);
        break;
      case NewButton3:
        addButton(2, gpio, Supla::Event::ON_PRESS, buttonAction, true, true);
        break;
      case NewButton4:
        addButton(3, gpio, Supla::Event::ON_PRESS, buttonAction, true, true);
        break;

      case NewButton1n:
        addButton(0, gpio, Supla::Event::ON_PRESS, buttonAction, false, false);
        addButtonCFG(gpio);
        break;
      case NewButton2n:
        addButton(1, gpio, Supla::Event::ON_PRESS, buttonAction, false, false);
        break;
      case NewButton3n:
        addButton(2, gpio, Supla::Event::ON_PRESS, buttonAction, false, false);
        break;
      case NewButton4n:
        addButton(3, gpio, Supla::Event::ON_PRESS, buttonAction, false, false);
        break;

      case NewLed1:
        addLedCFG(gpio, LOW);
        // addLed(0, gpio);
        break;
      case NewLed2:
        addLed(0, gpio, LOW);
        break;
      case NewLed3:
        addLed(1, gpio, LOW);
        break;
      case NewLed4:
        addLed(2, gpio, LOW);
        break;
      case NewLedLink:
        addLedCFG(gpio, LOW);
        break;

      case NewLed1i:
        addLedCFG(gpio);
        // addLed(0, gpio, LOW);
        break;
      case NewLed2i:
        addLed(0, gpio);
        break;
      case NewLed3i:
        addLed(1, gpio);
        break;
      case NewLed4i:
        addLed(2, gpio);
        break;
      case NewLedLinki:
        addLedCFG(gpio);
        break;

      case NewPWM1:
        if (isActiveRGBW(GPIO)) {
          ConfigESP->setGpio(gpio, FUNCTION_RGBW_RED);
        }
        else {
          ConfigESP->setGpio(gpio, ConfigManager->get(KEY_MAX_RGBW)->getValueInt(), FUNCTION_RGBW_BRIGHTNESS);
        }

        ConfigManager->set(KEY_MAX_RGBW, ConfigManager->get(KEY_MAX_RGBW)->getValueInt() + 1);
        break;
      case NewPWM2:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_GREEN);
        break;
      case NewPWM3:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BLUE);
        break;
      case NewPWM4:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BRIGHTNESS);
        break;
      case NewPWM5:
        // Wsparcie dla RGBW+W dodatkowym kanałem ściemniacza
        ConfigESP->setGpio(gpio, 1, FUNCTION_RGBW_BRIGHTNESS);
        ConfigManager->set(KEY_MAX_RGBW, ConfigManager->get(KEY_MAX_RGBW)->getValueInt() + 1);
        break;

      case NewPWM1i:
        if (isActiveRGBW(GPIO)) {
          ConfigESP->setGpio(gpio, FUNCTION_RGBW_RED);
        }
        else {
          ConfigESP->setGpio(gpio, ConfigManager->get(KEY_MAX_RGBW)->getValueInt(), FUNCTION_RGBW_BRIGHTNESS);
        }
        ConfigManager->set(KEY_MAX_RGBW, ConfigManager->get(KEY_MAX_RGBW)->getValueInt() + 1);
        break;
      case NewPWM2i:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_GREEN);
        break;
      case NewPWM3i:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BLUE);
        break;
      case NewPWM4i:
        ConfigESP->setGpio(gpio, FUNCTION_RGBW_BRIGHTNESS);
        break;
      case NewPWM5i:
        // Wsparcie dla RGBW+W dodatkowym kanałem ściemniacza
        ConfigESP->setGpio(gpio, 1, FUNCTION_RGBW_BRIGHTNESS);
        ConfigManager->set(KEY_MAX_RGBW, ConfigManager->get(KEY_MAX_RGBW)->getValueInt() + 1);
        break;

      case NewHLW8012CF:
        ConfigESP->setGpio(gpio, FUNCTION_CF);
        break;
      case NewBL0937CF:
        ConfigESP->setGpio(gpio, FUNCTION_CF);
        break;
      case NewHLWBLCF1:
        ConfigESP->setGpio(gpio, FUNCTION_CF1);
        break;
      case NewHLWBLSELi:
        ConfigESP->setGpio(gpio, FUNCTION_SEL);
        break;

      case NewTemperatureAnalog:
        ConfigESP->setGpio(gpio, FUNCTION_NTC_10K);
        break;

      case NewEthPOWER:
        break;

      case NewEthMDC:
        break;

      case NewEthMDIO:
        break;

      case NewSI7021:
        ConfigESP->setGpio(gpio, FUNCTION_SI7021_SONOFF);
        break;

      case NewDS18x20:
        ConfigESP->setGpio(gpio, FUNCTION_DS18B20);
        ConfigManager->set(KEY_MULTI_MAX_DS18B20, 1);
        break;

      case NewCSE7766Rx:
        ConfigESP->setGpio(gpio, FUNCTION_CSE7766_RX);
        break;

      case NewBinary1:
        addLimitSwitch(0, gpio);
        break;
      case NewBinary2:
        addLimitSwitch(1, gpio);
        break;
      case NewBinary3:
        addLimitSwitch(2, gpio);
        break;
      case NewBinary4:
        addLimitSwitch(3, gpio);
        break;

      case NewADE7953_IRQ:
        ConfigESP->setGpio(gpio, FUNCTION_ADE7953_IRQ);
        break;

      default:
        templateBoardWarning += "Brak funkcji: ";
        templateBoardWarning += gpioJSON;
        templateBoardWarning += "<br>";
    }
  }

#ifdef GUI_SENSOR_I2C_EXPENDER
  if (root["MCP23017"].success()) {
    addExpander(EXPENDER_MCP23017, root["MCP23017"]);
  }

  if (root["MCP23017_I2C2"].success()) {
    addExpander(EXPENDER_MCP23017_I2C2, root["MCP23017_I2C2"]);
  }

  if (root["PCF8575"].success()) {
    addExpander(EXPENDER_PCF8575, root["PCF8575"]);
  }

  if (root["PCF8575"].success()) {
    addExpander(EXPENDER_PCF8575_I2C2, root["PCF8575_I2C2"]);
  }

  if (root["PCF8574"].success()) {
    addExpander(EXPENDER_PCF8574, root["PCF8574"]);
  }
#endif

  JsonArray& numberButtons = root["BTN"];
  for (size_t i = 0; i < numberButtons.size(); i++) {
    int nr = numberButtons[i];
    nr--;

    if (nr >= 0) {
      ConfigESP->setNumberButton(nr);
    }
  }
}

int convert(int gpioJSON) {
  switch (gpioJSON) {
    case None:
      return NewNone;
    case Users:
      return NewUsers;

    case I2CSCL:
      return NewI2CSCL;
    case I2CSDA:
      return NewI2CSDA;

    case Relay1:
      return NewRelay1;
    case Relay2:
      return NewRelay2;
    case Relay3:
      return NewRelay3;
    case Relay4:
      return NewRelay4;

    case Relay1i:
      return NewRelay1i;
    case Relay2i:
      return NewRelay2i;
    case Relay3i:
      return NewRelay3i;
    case Relay4i:
      return NewRelay4i;

    case Switch1:
      return NewSwitch1;
    case Switch2:
      return NewSwitch2;
    case Switch3:
      return NewSwitch3;
    case Switch4:
      return NewSwitch4;
    case Switch5:
      return NewSwitch5;
    case Switch6:
      return NewSwitch6;
    case Switch7:
      return NewSwitch7;

    case Switch1n:
      return NewSwitch1n;
    case Switch2n:
      return NewSwitch2n;
    case Switch3n:
      return NewSwitch3n;
    case Switch4n:
      return NewSwitch4n;
    case Switch5n:
      return NewSwitch5n;
    case Switch6n:
      return NewSwitch6n;
    case Switch7n:
      return NewSwitch7n;

    case Button1:
      return NewButton1;
    case Button2:
      return NewButton2;
    case Button3:
      return NewButton3;
    case Button4:
      return NewButton4;

    case Button1n:
      return NewButton1n;
    case Button2n:
      return NewButton2n;
    case Button3n:
      return NewButton3n;
    case Button4n:
      return NewButton4n;

    case Led1:
      return NewLed1;
    case Led2:
      return NewLed2;
    case Led3:
      return NewLed3;
    case Led4:
      return NewLed4;
    case LedLink:
      return NewLedLink;

    case Led1i:
      return NewLed1i;
    case Led2i:
      return NewLed2i;
    case Led3i:
      return NewLed3i;
    case Led4i:
      return NewLed4i;
    case LedLinki:
      return NewLedLinki;

    case PWM1:
      return NewPWM1;
    case PWM2:
      return NewPWM2;
    case PWM3:
      return NewPWM3;
    case PWM4:
      return NewPWM4;
    case PWM5:
      return NewPWM5;
    case PWM1i:
      return NewPWM1;
    case PWM2i:
      return NewPWM2i;
    case PWM3i:
      return NewPWM3i;
    case PWM4i:
      return NewPWM4i;
    case PWM5i:
      return NewPWM5i;

    case HLW8012CF:
      return NewHLW8012CF;
    case BL0937CF:
      return NewBL0937CF;
    case HLWBLCF1:
      return NewHLWBLCF1;
    case HLWBLSELi:
      return NewHLWBLSELi;

    case SI7021:
      return NewSI7021;

    case DS18x20:
      return NewDS18x20;

    case CSE7766Tx:
      return NewCSE7766Tx;
    case CSE7766Rx:
      return NewCSE7766Rx;

    case Binary1:
      return NewBinary1;
    case Binary2:
      return NewBinary2;
    case Binary3:
      return NewBinary3;
    case Binary4:
      return NewBinary4;

    case ADE7953_IRQ:
      return NewADE7953_IRQ;
  }
  return NewNone;
}

uint8_t getGPIO(uint8_t gpio) {
#ifdef ARDUINO_ARCH_ESP8266
  if (gpio == 6 || gpio == 7)
    gpio = gpio + 3;
  else if (gpio >= 8)
    gpio = gpio + 4;
#elif ARDUINO_ARCH_ESP32
  if (gpio == 6 || gpio == 7)
    gpio = gpio + 3;
  else if (gpio >= 8 && gpio <= 23)
    gpio = gpio + 4;
  else if (gpio == 24)
    gpio = 6;
  else if (gpio == 25)
    gpio = 7;
  else if (gpio == 26)
    gpio = 8;
  else if (gpio == 27)
    gpio = 11;
  else if (gpio >= 28)
    gpio = gpio + 4;
#endif

  return gpio;
}

void addButton(uint8_t nr, uint8_t gpio, uint8_t event, JsonArray& buttonAction, bool pullUp, bool invertLogic) {
  uint8_t maxButton = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  if (buttonAction[nr].success())
    ConfigESP->setAction(gpio, (int)buttonAction[nr]);
  else
    ConfigESP->setAction(gpio, Supla::GUI::Action::TOGGLE);

  ConfigESP->setEvent(gpio, event);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  if (ConfigESP->getGpio(FUNCTION_CFG_BUTTON) == OFF_GPIO)
    addButtonCFG(gpio);
  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);

  ConfigESP->setNumberButton(nr);
  ConfigManager->set(KEY_MAX_BUTTON, maxButton + 1);
}

void addButtonAnalog(uint8_t nr, uint8_t gpio, JsonArray& buttonAction) {
  uint8_t maxButton = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  if (buttonAction[nr].success())
    ConfigESP->setAction(gpio, (int)buttonAction[nr]);
  else
    ConfigESP->setAction(gpio, Supla::GUI::Action::TOGGLE);

  ConfigESP->setEvent(gpio, Supla::GUI::Event::ON_PRESS);
  ConfigESP->setNumberButton(nr);
  ConfigManager->set(KEY_MAX_BUTTON, maxButton + 1);
}

void addRelay(uint8_t nr, uint8_t gpio, uint8_t level) {
  uint8_t maxRelay = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RESTORE);
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);

  ConfigManager->set(KEY_MAX_RELAY, maxRelay + 1);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  uint8_t ledPin = ConfigESP->getGpio(FUNCTION_CFG_LED);

  if (ledPin != OFF_GPIO) {
    ConfigESP->clearGpio(ledPin, FUNCTION_CFG_LED);
    addLed(0, ledPin, ConfigESP->getInversed(ledPin));
  }

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED);
}

void addLed(uint8_t nr, uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, nr, FUNCTION_LED);
}

void addButtonCFG(uint8_t gpio) {
  for (uint8_t nr = 0; nr <= OFF_GPIO; nr++) {
    ConfigESP->clearGpio(nr, FUNCTION_CFG_BUTTON);
  }

  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

void addLimitSwitch(uint8_t nr, uint8_t gpio) {
  uint8_t max = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();

  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH);
  ConfigESP->setPullUp(gpio, HIGH);
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, max + 1);
}

bool isActiveRGBW(JsonArray& GPIO) {
  bool isActivRGBW = false;

  for (size_t i = 0; i < GPIO.size(); i++) {
    int gpioJSON = (int)GPIO[i];

#ifdef ARDUINO_ARCH_ESP8266
    if (oldVersion)
      gpioJSON = convert(gpioJSON);
#endif

    if (gpioJSON == NewPWM2 || gpioJSON == NewPWM3 || gpioJSON == NewPWM4 || gpioJSON == NewPWM5 || gpioJSON == NewPWM2i || gpioJSON == NewPWM3i ||
        gpioJSON == NewPWM4i || gpioJSON == NewPWM5i) {
      isActivRGBW = true;
    }
  }

  return isActivRGBW;
}

#ifdef GUI_SENSOR_I2C_EXPENDER
void addExpander(uint8_t typeExpander, JsonArray& expander) {
  // {"NAME":"MCP23017","GPIO":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,608,604,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"MCP23017":[[[address,function],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]]}
  // {"NAME":"MCP23017","GPIO":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,608,604,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"MCP23017":[[[0,1],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]]}
  // {"NAME":"MCP23017","GPIO":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,608,640,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"MCP23017":[[[0,1],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[[1,1],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[[2,2],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[[3,2],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]]}
  //
  // Doda jeden MCP23017 pod adres 0x20 z funkcjami 1-8 przekaźniki od 9-16 przyciski
  // {"NAME":"MCP23017x1_ESP8266","GPIO":[0,0,544,0,640,608,0,0,0,0,0,0,0,0],"MCP23017":[[[0,1],1,2,3,4,5,6,7,8,0,0,0,0,0,0,0,0],[[0,2],9,10,11,12,13,14,15,16,0,0,0,0,0,0,0,0]]}
  // Doda dwa PCF8575 pod adres 0x20 oraz 0x21 z funkcjami 1-16 przekaźniki na adres 0x20 oraz od 1-16 przyciski na adres 0x21
  // {"NAME":"PCF8575x2_ESP8266","GPIO":[0,0,544,0,640,608,0,0,0,0,0,0,0,0],"PCF8575":[[[0,1],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[[1,2],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]]}
  // Doda dwa PCF8574 pod adres 0x20 oraz 0x21 z funkcjami 1-8 przekaźniki na adres 0x20 oraz od 1-8 przyciski na adres 0x21
  // {"NAME":"PCF8574x2_ESP8266","GPIO":[0,0,544,0,640,608,0,0,0,0,0,0,0,0],"PCF8574":[[[0,1],1,2,3,4,5,6,7,8],[[1,2],1,2,3,4,5,6,7,8]]}
  // Doda PCF8575 pod adres 0x20 dla przekaźników oraz MCP23017 pod adres 0x021 dla przycisków
  // {"NAME":"PCF_MCP_ESP8266","GPIO":[0,0,544,0,640,608,0,0,0,0,0,0,0,0],"PCF8575":[[[0,1],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]],"MCP23017":[[[1,2],1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]]}
  //
  // "address":
  // 0 - 0x20
  // 1 - 0x21
  // 2 - 0x22
  // 3 - 0x23
  //
  // "function":
  // 1 - FUNCTION_RELAY
  // 2 - FUNCTION_BUTTON
  // 3 - FUNCTION_LIMIT_SWITCH

  // type:
  // 1 - MCP23017 x16
  // 2 - PCF8575 x16
  // 3 - PCF8574 x8

  size_t key = FUNCTION_OFF;
  size_t sizeExpander = 0;

  for (size_t i = 0; i < expander.size(); i++) {
    int address = (int)expander[i][0][0];
    int function = (int)expander[i][0][1];

    switch (function) {
      case FUNCTION_RELAY:
        key = KEY_MAX_RELAY;
        break;
      case FUNCTION_BUTTON:
        key = KEY_MAX_BUTTON;
        break;
      case FUNCTION_LIMIT_SWITCH:
        key = KEY_MAX_LIMIT_SWITCH;
        break;
    }

    if (typeExpander == EXPENDER_MCP23017) {
      sizeExpander = 16;
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_MCP23017);
    }
    else if (typeExpander == EXPENDER_MCP23017_I2C2) {
      sizeExpander = 16;
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_MCP23017_I2C2);
    }
    else if (typeExpander == EXPENDER_PCF8575) {
      sizeExpander = 16;
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_PCF8575);
    }
    else if (typeExpander == EXPENDER_PCF8575_I2C2) {
      sizeExpander = 16;
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_PCF8575_I2C2);
    }
    else if (typeExpander == EXPENDER_PCF8574) {
      sizeExpander = 8;
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_PCF8574);
    }
    else if (typeExpander == EXPENDER_PCF8574_I2C2) {
      ConfigManager->setElement(KEY_ACTIVE_EXPENDER, function, EXPENDER_PCF8574_I2C2);
    }

    for (size_t ii = 1; ii <= sizeExpander; ii++) {
      uint8_t gpio = (int)expander[i][ii];

      if (gpio != 0) {
        if (gpio != 17)
          gpio = gpio - 1;

        uint8_t nr = ConfigManager->get(key)->getValueInt();
        Expander->setGpioExpander(gpio, address, nr, function);

        switch (function) {
          case FUNCTION_RELAY:
            ConfigESP->setLevel(gpio, true);
            ConfigESP->setMemory(gpio, MEMORY_RESTORE);
            break;
          case FUNCTION_BUTTON:
            ConfigESP->setAction(gpio, Supla::GUI::Action::TOGGLE);
            ConfigESP->setEvent(gpio, Supla::GUI::Event::ON_CHANGE);
            ConfigESP->setPullUp(gpio, true);
            ConfigESP->setInversed(gpio, true);
            ConfigESP->setNumberButton(nr);
            break;
        }
        ConfigManager->set(key, ConfigManager->get(key)->getValueInt() + 1);
      }
    }
  }
}
#endif

}  // namespace TanplateBoard
}  // namespace Supla
#elif defined(TEMPLATE_BOARD_OLD)
void addButton(uint8_t gpio, uint8_t event, uint8_t action, bool pullUp, bool invertLogic) {
  uint8_t nr = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

  ConfigESP->setEvent(gpio, event);
  ConfigESP->setAction(gpio, action);
  ConfigESP->setPullUp(gpio, pullUp);
  ConfigESP->setInversed(gpio, invertLogic);

  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON);
  ConfigManager->set(KEY_MAX_BUTTON, nr + 1);
}

void addRelay(uint8_t gpio, uint8_t level) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();

  ConfigESP->setLevel(gpio, level);
  ConfigESP->setMemory(gpio, MEMORY_RESTORE);
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY);
  ConfigManager->set(KEY_MAX_RELAY, nr + 1);
}

void addLimitSwitch(uint8_t gpio) {
  uint8_t nr = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();

  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH);
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, nr + 1);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setLevel(gpio, level);
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED);
}

void addLed(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_LED);
}

void addButtonCFG(uint8_t gpio) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_BUTTON);
}

#ifdef SUPLA_HLW8012
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL) {
  ConfigESP->setGpio(pinCF, FUNCTION_CF);
  ConfigESP->setGpio(pinCF1, FUNCTION_CF1);
  ConfigESP->setGpio(pinSEL, FUNCTION_SEL);
  // Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
}
#endif

void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RGBW)->getValueInt();

  ConfigESP->setGpio(redPin, nr, FUNCTION_RGBW_RED);
  ConfigESP->setGpio(greenPin, nr, FUNCTION_RGBW_GREEN);
  ConfigESP->setGpio(bluePin, nr, FUNCTION_RGBW_BLUE);
  ConfigESP->setGpio(brightnessPin, nr, FUNCTION_RGBW_BRIGHTNESS);
  ConfigManager->set(KEY_MAX_RGBW, nr + 1);
}

void addDimmer(int8_t brightnessPin) {
  addRGBW(OFF_GPIO, OFF_GPIO, OFF_GPIO, brightnessPin);
}

void chooseTemplateBoard(uint8_t board) {
  ConfigESP->clearEEPROM();
  ConfigManager->deleteGPIODeviceValues();

  ConfigManager->set(KEY_BOARD, board);
  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");
  ConfigManager->set(KEY_CFG_MODE, CONFIG_MODE_10_ON_PRESSES);

  switch (board) {
    case BOARD_ELECTRODRAGON:
      addLedCFG(16);
      addButtonCFG(0);
      addButton(0);
      addButton(2);
      addRelay(12);
      addRelay(13);
      break;
    case BOARD_INCAN3:
      addLedCFG(2, LOW);
      addButtonCFG(0);
      addButton(14, Supla::Event::ON_CHANGE);
      addButton(12, Supla::Event::ON_CHANGE);
      addRelay(5);
      addRelay(13);
      addLimitSwitch(4);
      addLimitSwitch(16);
      break;
    case BOARD_INCAN4:
      addLedCFG(12);
      addButtonCFG(0);
      addButton(2, Supla::Event::ON_CHANGE);
      addButton(10, Supla::Event::ON_CHANGE);
      addRelay(4);
      addRelay(14);
      addLimitSwitch(4);
      addLimitSwitch(16);
      break;
    case BOARD_MELINK:
      addLedCFG(12);
      addButtonCFG(5);
      addButton(5);
      addRelay(4);
      break;
    case BOARD_NEO_COOLCAM:
      addLedCFG(4);
      addButtonCFG(13);
      addButton(13);
      addRelay(12);
      break;
    case BOARD_SHELLY1:
      addButtonCFG(5);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addRelay(4);
      break;
    case BOARD_SHELLY2:
      addLedCFG(16);
      addButtonCFG(12);
      addButton(12, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addButton(14, Supla::Event::ON_PRESS, Supla::Action::TOGGLE, false, true);
      addRelay(4);
      addRelay(5);
      break;
    case BOARD_SONOFF_BASIC:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_MINI:
      addLedCFG(13);
      addButtonCFG(4);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(12);
      break;
    case BOARD_SONOFF_DUAL_R2:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addRelay(12);
      addRelay(5);
      break;
    case BOARD_SONOFF_S2X:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_SV:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      addLimitSwitch(4);
      break;
    case BOARD_SONOFF_TH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      ConfigESP->setGpio(14, FUNCTION_SI7021_SONOFF);
      break;
    case BOARD_SONOFF_TOUCH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
      break;
    case BOARD_SONOFF_TOUCH_2CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addRelay(12);
      addRelay(5);
      break;
    case BOARD_SONOFF_TOUCH_3CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addButton(10);
      addRelay(12);
      addRelay(5);
      addRelay(4);
      break;
    case BOARD_SONOFF_4CH:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addButton(9);
      addButton(10);
      addButton(14);
      addRelay(12);
      addRelay(5);
      addRelay(4);
      addRelay(15);
      break;
    case BOARD_YUNSHAN:
      addLedCFG(2, LOW);
      addButtonCFG(0);
      addButton(3);
      addRelay(4);
      break;

    case BOARD_YUNTONG_SMART:
      addLedCFG(15);
      addButtonCFG(12);
      addButton(12);
      addRelay(4);
      break;

    case BOARD_GOSUND_SP111:
      addLedCFG(2, LOW);
      addButtonCFG(13);
      addButton(13);
      addRelay(15);
      addLed(0);
#ifdef SUPLA_HLW8012
      addHLW8012(5, 4, 12);
      // Supla::GUI::counterHLW8012->setCurrentMultiplier(18388);
      // Supla::GUI::counterHLW8012->setVoltageMultiplier(247704);
      // Supla::GUI::counterHLW8012->setPowerMultiplier(2586583);
      // Supla::GUI::counterHLW8012->setMode(LOW);
#endif
      break;
    case BOARD_DIMMER_LUKASZH:
      addLedCFG(15);
      addButtonCFG(0);
      addDimmer(14);
      addDimmer(12);
      addDimmer(13);
      addButton(5);
      addButton(4);
      addButton(16);
      // ConfigESP->setGpio(GPIO_ANALOG_A0_ESP8266, FUNCTION_NTC_10K);
      break;
    case BOARD_H801:
      addLedCFG(1);
      addButtonCFG(0);
      addRGBW(15, 13, 12, 4);
      break;
    case BOARD_SHELLY_PLUG_S:
      addLedCFG(2, LOW);
      addButtonCFG(13);
      addButton(13);
      addRelay(15);
      addLed(0);
#ifdef SUPLA_HLW8012
      addHLW8012(5, 14, 12);
      // Supla::GUI::counterHLW8012->setCurrentMultiplier(18388);
      // Supla::GUI::counterHLW8012->setVoltageMultiplier(247704);
      // Supla::GUI::counterHLW8012->setPowerMultiplier(2586583);
#endif
      break;
    case BOARD_MINITIGER_1CH:
      addLedCFG(1);
      addButtonCFG(5);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(12);
      break;
    case BOARD_MINITIGER_2CH:
      addLedCFG(1);
      addButtonCFG(3);
      addButton(3, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(13);
      addRelay(14);
      break;
    case BOARD_MINITIGER_3CH:
      addLedCFG(1);
      addButtonCFG(3);
      addButton(3, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(5, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addButton(4, Supla::Event::ON_PRESS, Supla::Action::TOGGLE);
      addRelay(13);
      addRelay(12);
      addRelay(14);
      break;
  }
}
#endif