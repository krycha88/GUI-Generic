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

#ifndef GUI_GENERIC_COMMON_DEFINED_H
#define GUI_GENERIC_COMMON_DEFINED_H

#ifndef SUPLA_EXCLUDE_LITTLEFS_CONFIG
#define SUPLA_EXCLUDE_LITTLEFS_CONFIG
#endif

#include "GUI-Generic_Config.h"

#if defined(SUPLA_MCP23017) || defined(SUPLA_PCF8575) || defined(SUPLA_PCF8574)
#define GUI_SENSOR_I2C_EXPENDER
#endif

#if defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_ACTION_TRIGGER)
#if !defined(SUPLA_BUTTON)
#define SUPLA_BUTTON
#endif
#endif

#if defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_DIRECT_LINKS) || defined(SUPLA_LED)
#if !defined(SUPLA_RELAY)
#define SUPLA_RELAY
#endif
#endif

#ifndef TEMPLATE_BOARD_OLD
#ifndef TEMPLATE_BOARD_JSON
#define TEMPLATE_BOARD_JSON
#endif
#endif

#ifdef SUPLA_THERMOSTAT
#ifndef SUPLA_RELAY
#define SUPLA_RELAY
#endif

#ifndef SUPLA_LED
#define SUPLA_LED
#endif
#endif

#ifdef SUPLA_PUSHOVER
#ifndef SUPLA_CONDITIONS
#define SUPLA_CONDITIONS
#endif
#endif

#endif