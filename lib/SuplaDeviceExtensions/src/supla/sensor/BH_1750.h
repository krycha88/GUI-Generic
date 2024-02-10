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
#ifndef _bh1750_h
#define _bh1750_h
#include <BH1750_WE.h>

#include <Arduino.h>
#include <Wire.h>
#include <supla/sensor/thermometer.h>

namespace Supla {
namespace Sensor {
class BH_1750 : public Thermometer {
 public:
  BH_1750(int8_t address = 0x23);
  double getValue();

 private:
  void onInit();

 protected:
  BH1750_WE *myBH1750;
};
}  // namespace Sensor
}  // namespace Supla

#endif