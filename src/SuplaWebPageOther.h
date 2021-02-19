#ifndef SuplaWebPageOther_h
#define SuplaWebPageOther_h

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"

void createWebPageOther();

#if defined(SUPLA_HC_SR04) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_HLW8012)
void suplaWebPageOther(int save);
void handleOther();
void handleOtherSave();
#endif

#if defined(SUPLA_IMPULSE_COUNTER)
#define INPUT_IMPULSE_COUNTER_GPIO             "ic"
#define INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT "icdt"
#define INPUT_IMPULSE_COUNTER_PULL_UP          "icpu"
#define INPUT_IMPULSE_COUNTER_RAISING_EDGE     "icre"
#define INPUT_IMPULSE_COUNTER_CHANGE_VALUE     "iccv"
#define INPUT_MAX_IMPULSE_COUNTER              "imic"
void handleImpulseCounterSet();
void handleImpulseCounterSaveSet();
void supla_impulse_counter_set(int save);
#endif

#if defined(SUPLA_HLW8012)
#define INPUT_CF  "cf"
#define INPUT_CF1 "cf1"
#define INPUT_SEL "sel"

#define PATH_HLW8012_CALIBRATE      "calibrate"
#define PATH_SAVE_HLW8012_CALIBRATE "savecalibrate"

#define INPUT_CALIB_POWER   "power"
#define INPUT_CALIB_VOLTAGE "voltage"

void handleHLW8012Calibrate();
void handleHLW8012CalibrateSave();
void suplaWebpageHLW8012Calibrate(uint8_t save);
#endif

#ifdef SUPLA_RGBW
#define INPUT_RGBW_MAX              "rgbwm"
#define INPUT_RGBW_RED              "rgbwr"
#define INPUT_RGBW_GREEN            "rgbwg"
#define INPUT_RGBW_BLUE             "rgbwb"
#define INPUT_RGBW_BRIGHTNESS       "rgbwbr"
#define INPUT_RGBW_COLOR_BRIGHTNESS "rgbwcb"
#endif

#ifdef SUPLA_PUSHOVER
#define INPUT_PUSHOVER_TOKEN "pot"
#define INPUT_PUSHOVER_USER  "pou"
#endif

#ifdef SUPLA_HC_SR04
#define INPUT_TRIG_GPIO               "trig"
#define INPUT_ECHO_GPIO               "echo"
#define INPUT_HC_SR04_MAX_SENSOR_READ "hsm"
#endif

#endif  // SuplaWebPageOther_h
