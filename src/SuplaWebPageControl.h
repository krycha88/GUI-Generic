#ifndef SuplaWebPageControl_h
#define SuplaWebPageControl_h

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"

#define PATH_CONTROL            "control"
#define PATH_SWITCH             "switch"
#define PATH_SAVE_SWITCH        "saveswitch"
#define PATH_SAVE_CONTROL       "savecontrol"
#define PATH_BUTTON_SET         "setbutton"
#define PATH_SAVE_BUTTON_SET    "savesetbutton"
#define INPUT_TRIGGER           "trs"
#define INPUT_BUTTON_SET        "bts"
#define INPUT_BUTTON_GPIO       "btg"
#define INPUT_BUTTON_INVERSED   "ibi"
#define INPUT_BUTTON_EVENT      "icl"
#define INPUT_BUTTON_ACTION     "bta"
#define INPUT_LIMIT_SWITCH_GPIO "lsg"
#define INPUT_MAX_BUTTON        "mbt"
#define INPUT_MAX_LIMIT_SWITCH  "mls"

/*enum _trigger_button {
  TRIGGER_PRESS,
  TRIGGER_RELEASE,
  TRIGGER_CHANGE
};*/

class SuplaWebPageControl {
 public:
  void createWebPageControl();
#ifdef SUPLA_BUTTON
  void handleControl();
  void handleControlSave();
#endif

#ifdef SUPLA_LIMIT_SWITCH
  void handleLimitSwitch();
  void handleLimitSwitchSave();
  void suplaWebpageLimitSwitch(int save);
#endif

#if (defined(SUPLA_BUTTON) && defined(SUPLA_RELAY)) || (defined(SUPLA_RSUPLA_BUTTONELAY) || defined(SUPLA_ROLLERSHUTTER))
  void handleButtonSet();
  void handleButtonSaveSet();
#endif

#ifdef SUPLA_BUTTON
  void supla_webpage_button_set(int save, int nr = 0);
#endif

  void handleButtonSetMCP23017();
  void handleButtonSaveSetMCP23017();
  void supla_webpage_button_set_MCP23017(int save);

 private:
  void supla_webpage_control(int save);
};

extern SuplaWebPageControl* WebPageControl;
#endif  // SuplaWebPageControl_h
