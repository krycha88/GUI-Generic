#ifndef SuplaTemplateBoard_h
#define SuplaTemplateBoard_h

#include <pgmspace.h>
#include "SuplaWebPageRelay.h"
#include "GUIGenericCommon.h"

void addButton(uint8_t gpio, uint8_t event = 0);
void addRelay(uint8_t gpio, uint8_t level = HIGH);
void addLimitSwitch(uint8_t gpio);
void addLedCFG(uint8_t gpio, uint8_t level = HIGH);
void addLed(uint8_t gpio);
void addButtonCFG(uint8_t gpio);
void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL);
void addRGBW(int8_t redPin, int8_t greenPin, int8_t bluePin, int8_t brightnessPin);
void addDimmer(int8_t brightnessPin);

enum _board
{
  BOARD_ELECTRODRAGON = 1,
  BOARD_INCAN3,
  BOARD_INCAN4,
  BOARD_MELINK,
  BOARD_NEO_COOLCAM,
  BOARD_SHELLY1,
  BOARD_SHELLY2,
  BOARD_SONOFF_BASIC,
  BOARD_SONOFF_DUAL_R2,
  BOARD_SONOFF_S2X,
  BOARD_SONOFF_SV,
  BOARD_SONOFF_TH,
  BOARD_SONOFF_TOUCH,
  BOARD_SONOFF_TOUCH_2CH,
  BOARD_SONOFF_TOUCH_3CH,
  BOARD_SONOFF_4CH,
  BOARD_YUNSHAN,
  BOARD_YUNTONG_SMART,
  BOARD_GOSUND_SP111,
  BOARD_DIMMER_LUKASZH,
  BOARD_H801,
  MAX_MODULE
};

const char BOARD_NULL[] PROGMEM = S_ABSENT;
const char ELECTRODRAGON[] PROGMEM = "ElectroDragon";
const char INCAN3[] PROGMEM = "inCan3";
const char INCAN4[] PROGMEM = "inCan4";
const char MELINK[] PROGMEM = "Melink";
const char NEO_COOLCAM[] PROGMEM = "Neo Coolcam";
const char SHELLY1[] PROGMEM = "Shelly 1";
const char SHELLY2[] PROGMEM = "Shelly 2";
const char SONOFF_BASIC[] PROGMEM = "SONOFF BASIC";
const char SONOFF_DUAL_R2[] PROGMEM = "SONOFF DUAL R2";
const char SONOFF_S2X[] PROGMEM = "SONOFF S2X";
const char SONOFF_SV[] PROGMEM = "SONOFF SV";
const char SONOFF_TH[] PROGMEM = "SONOFF TH";
const char SONOFF_TOUCH[] PROGMEM = "SONOFF TOUCH";
const char SONOFF_TOUCH_2CH[] PROGMEM = "SONOFF TOUCH DUAL";
const char SONOFF_TOUCH_3CH[] PROGMEM = "SONOFF TOUCH TRIPLE";
const char SONOFF_4CH[] PROGMEM = "SONOFF 4CH";
const char YUNSHAN[] PROGMEM = "Yunshan";
const char YUNTONG_SMART[] PROGMEM = "YUNTONG Smart";
const char GOSUNG_SP111[] PROGMEM = "Gosund SP111";
const char DRIMMER_LUKASZH[] PROGMEM = "Dimmer by @LukaszH";
const char H801[] PROGMEM = "H801 RGBW";
const char SONOFF_MINI[] PROGMEM = "SONOFF Mini";

const char* const BOARD_P[MAX_MODULE] PROGMEM = {
    BOARD_NULL, ELECTRODRAGON, INCAN3,    INCAN4,       MELINK,           NEO_COOLCAM,      SHELLY1,    SHELLY2, SONOFF_BASIC,  SONOFF_DUAL_R2,
    SONOFF_S2X, SONOFF_SV,     SONOFF_TH, SONOFF_TOUCH, SONOFF_TOUCH_2CH, SONOFF_TOUCH_3CH, SONOFF_4CH, YUNSHAN, YUNTONG_SMART, GOSUNG_SP111, DRIMMER_LUKASZH, H801};

void chooseTemplateBoard(uint8_t board);

#endif  // SuplaTemplateBoard_h
