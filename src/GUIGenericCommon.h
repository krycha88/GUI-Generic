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

#ifndef GUI_GENERIC_COMMON_H
#define GUI_GENERIC_COMMON_H

#include "GUI-Generic_Config.h"
#include "GUIGenericCommonDefined.h"
#include "src/language/common.h"
#include "Arduino.h"

#ifndef UI_LANGUAGE
#define UI_LANGUAGE pl
#endif
#define QUOTE(x)        QUOTE_1(x)
#define QUOTE_1(x)      #x
#define INCLUDE_FILE(x) QUOTE(src/language/x.h)
#include INCLUDE_FILE(UI_LANGUAGE)

uint8_t *HexToBytes(String _value);
uint8_t getCountSensorChannels();
uint8_t getCountActiveThermostat();
int getCountChannels();
const String getAPName();
void printFreeMemory(const char* location = "");

#endif  // GUI_GENERIC_COMMON_H
