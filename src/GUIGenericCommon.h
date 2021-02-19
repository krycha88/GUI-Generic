#ifndef GUI_GENERIC_COMMON_H
#define GUI_GENERIC_COMMON_H

#ifndef UI_LANGUAGE
#include "language/pl.h"
#else
#define QUOTE(x)        QUOTE_1(x)
#define QUOTE_1(x)      #x
#define INCLUDE_FILE(x) QUOTE(language / x.h)
#include INCLUDE_FILE(UI_LANGUAGE)
#endif

#include "SuplaDeviceGUI.h"

uint8_t *HexToBytes(String _value);
int getCountSensorChannels();

#endif  // GUI_GENERIC_COMMON_H
