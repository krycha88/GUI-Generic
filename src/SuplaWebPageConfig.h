#ifndef SuplaWebPageConfig_h
#define SuplaWebPageConfig_h

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"

#define  PATH_CONFIG            "config"
#define  PATH_SAVE_CONFIG       "saveconfig"
#define  INPUT_CFG_LED_GPIO     "cfgl"
#define  INPUT_CFG_BTN_GPIO     "cfgb"
#define  INPUT_CFG_LED_LEVEL    "icll"
#define  INPUT_CFG_MODE         "cfgm"

class SuplaWebPageConfig {
 public:
  SuplaWebPageConfig();
  void createWebPageConfig();
  void handleConfig();
  void handleConfigSave();

 private:
  void supla_webpage_config(int save);
};

extern SuplaWebPageConfig *WebPageConfig;
#endif  // SuplaWebPageConfig_h
