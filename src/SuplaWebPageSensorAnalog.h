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

#ifndef SuplaWebPageSensorAnalog_h
#define SuplaWebPageSensorAnalog_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_NTC_10K) || defined(SUPLA_MPX_5XXX) || defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
#define GUI_SENSOR_ANALOG
#endif

#ifdef GUI_SENSOR_ANALOG

#define PATH_ANALOG "analog"

void createWebPageSensorAnalog();
void handleSensorAnalog(int save = 0);
void handleSensorAnalogSave();

#ifdef SUPLA_NTC_10K
#define INPUT_NTC_10K "in10k"
#endif

#ifdef SUPLA_MPX_5XXX
#define PATH_MPX_5XX_EMPTY "mpxe"
#define PATH_MPX_5XX_FULL  "mpxf"

#define INPUT_MPX_5XXX     "impx"
#define INPUT_THANK_HEIGHT "ith"
#define INPUT_THANK_EMPTY  "ite"
#define INPUT_THANK_FULL   "itf"
#endif

#endif

#if defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
#define INPUT_MAX_ANALOG_READING             "imar"
#define PATH_ANALOG_READING_MAP_MIN          "armin"
#define PATH_ANALOG_READING_MAP_MAX          "armax"
#define INPUT_ANALOG_READING_MAP             "ziarm"
#define INPUT_ANALOG_READING_MAP_MIN         "ziarmin"
#define INPUT_ANALOG_READING_MAP_MAX         "ziarmnax"
#define INPUT_ANALOG_READING_MAP_MIN_DESIRED "ziarmind"
#define INPUT_ANALOG_READING_MAP_MAX_DESIRED "ziarmnaxd"

#define URL_ARG_NR "nr"
#endif

#endif  // ifndef SuplaWebPageSensorAnalog_h
