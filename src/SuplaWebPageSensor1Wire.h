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

#ifndef SuplaWebPageSensor1Wire_h
#define SuplaWebPageSensor1Wire_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
#define GUI_SENSOR_1WIRE
#endif

#ifdef GUI_SENSOR_1WIRE
#define PATH_1WIRE "1wire"

#ifdef SUPLA_DHT11
#define INPUT_DHT11_GPIO "dht11"
#define INPUT_MAX_DHT11  "mdht11"
#endif

#ifdef SUPLA_DHT22
#define INPUT_DHT22_GPIO "dht22"
#define INPUT_MAX_DHT22  "mdht22"
#endif

#ifdef SUPLA_SI7021_SONOFF
#define INPUT_SI7021_SONOFF "si7021sonoff"
#endif

void createWebPageSensor1Wire();
void handleSensor1Wire(int save = 0);
void handleSensor1WireSave();

#ifdef SUPLA_DS18B20
#define PATH_MULTI_DS "multids"

#define INPUT_MAX_DS18B20   "maxds"
#define INPUT_MULTI_DS_GPIO "mdsg"
#define INPUT_DS18B20_ADDR  "dsaddr"
#define INPUT_DS18B20_NAME  "dsname"

void handleSensorDs18b20(int save = 0);
void handleSensorDs18b20Save();
#endif

#endif

#endif  // ifndef SuplaWebPageSensor1Wire_h
