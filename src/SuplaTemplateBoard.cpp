#include "SuplaTemplateBoard.h"
#include "SuplaWebPageRelay.h"
#include <supla/events.h>

void addButton(uint8_t gpio, uint8_t event) {
  uint8_t nr = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();
  nr++;
  ConfigESP->setGpio(gpio, nr, FUNCTION_BUTTON, -1, -1, -1, event);
  ConfigManager->set(KEY_MAX_BUTTON, nr++);
}

void addRelay(uint8_t gpio, uint8_t level) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RELAY)->getValueInt();
  nr++;
  ConfigESP->setGpio(gpio, nr, FUNCTION_RELAY, level, MEMORY_RELAY_RESTORE);
  ConfigManager->set(KEY_MAX_RELAY, nr++);
}

void addLimitSwitch(uint8_t gpio) {
  uint8_t nr = ConfigManager->get(KEY_MAX_LIMIT_SWITCH)->getValueInt();
  nr++;
  ConfigESP->setGpio(gpio, nr, FUNCTION_LIMIT_SWITCH);
  ConfigESP->setGpio(4, 1, FUNCTION_LIMIT_SWITCH);
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, nr++);
}

void addLedCFG(uint8_t gpio, uint8_t level) {
  ConfigESP->setGpio(gpio, FUNCTION_CFG_LED, level);
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
  Supla::GUI::addHLW8012(ConfigESP->getGpio(FUNCTION_CF), ConfigESP->getGpio(FUNCTION_CF1), ConfigESP->getGpio(FUNCTION_SEL));
}
#endif

void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin) {
  uint8_t nr = ConfigManager->get(KEY_MAX_RGBW)->getValueInt();
  nr++;
  ConfigESP->setGpio(redPin, nr, FUNCTION_RGBW_RED);
  ConfigESP->setGpio(greenPin, nr, FUNCTION_RGBW_GREEN);
  ConfigESP->setGpio(bluePin, nr, FUNCTION_RGBW_BLUE);
  ConfigESP->setGpio(brightnessPin, nr, FUNCTION_RGBW_BRIGHTNESS);
  ConfigManager->set(KEY_MAX_RGBW, nr++);
}

void addDimmer(int8_t brightnessPin) {
  addRGBW(OFF_GPIO, OFF_GPIO, OFF_GPIO, brightnessPin);
}

void chooseTemplateBoard(uint8_t board) {
  int8_t nr, key;
  for (nr = 0; nr <= OFF_GPIO; nr++) {
    key = KEY_GPIO + nr;
    ConfigManager->set(key, "");
  }

  ConfigManager->set(KEY_MAX_BUTTON, "0");
  ConfigManager->set(KEY_MAX_RELAY, "0");
  ConfigManager->set(KEY_MAX_LIMIT_SWITCH, "0");
  ConfigManager->set(KEY_MAX_RGBW, "0");

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
      addButton(14, Supla::ON_CHANGE);
      addButton(12, Supla::ON_CHANGE);
      addRelay(5);
      addRelay(13);
      addLimitSwitch(4);
      addLimitSwitch(16);
      break;
    case BOARD_INCAN4:
      addLedCFG(12);
      addButtonCFG(0);
      addButton(2, Supla::ON_CHANGE);
      addButton(10, Supla::ON_CHANGE);
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
      addButton(5);
      addRelay(4);
      break;
    case BOARD_SHELLY2:
      addLedCFG(16);
      addButtonCFG(12);
      addButton(12);
      addButton(14);
      addRelay(4);
      addRelay(5);
      break;
    case BOARD_SONOFF_BASIC:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
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
      ConfigESP->setGpio(14, FUNCTION_SI7021_SONOFF);
      break;
    case BOARD_SONOFF_SV:
      addLedCFG(13);
      addButtonCFG(0);
      addButton(0);
      addRelay(12);
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
      addButton(3, Supla::ON_CHANGE);
      addRelay(4);
      break;

    case BOARD_YUNTONG_SMART:
      addLedCFG(15);
      addButtonCFG(12);
      addButton(12, Supla::ON_CHANGE);
      addRelay(4);
      break;

    case BOARD_GOSUND_SP111:
      addLedCFG(2, LOW);
      addButtonCFG(13);
      addButton(13, Supla::ON_RELEASE);
      addRelay(15);
      addLed(0);
#ifdef SUPLA_HLW8012
      addHLW8012(5, 4, 12);
      Supla::GUI::counterHLW8012->setCurrentMultiplier(18388);
      Supla::GUI::counterHLW8012->setVoltageMultiplier(247704);
      Supla::GUI::counterHLW8012->setPowerMultiplier(2586583);
      Supla::Storage::ScheduleSave(2000);
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
      break;
    case BOARD_H801:
      addLedCFG(1);
      addButtonCFG(0);
      addRGBW(15, 13, 12, 4);
      break;
  }
}
