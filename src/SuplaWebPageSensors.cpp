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

#include "SuplaWebPageSensors.h"

void createWebPageSensors() {
#ifdef GUI_SENSOR_SPI
  createWebPageSensorSpi();
#endif

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_2) || defined(GUI_SENSOR_I2C_ENERGY_METER)
  createWebPageSensorI2c();
#endif

#ifdef GUI_SENSOR_1WIRE
  createWebPageSensor1Wire();
#endif

#if defined(GUI_SENSOR_ANALOG)
  createWebPageSensorAnalog();
#endif
}
