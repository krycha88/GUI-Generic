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
#ifdef SUPLA_CSE7766
#ifndef _cse7766_h
#define _cse7766_h

#include <Arduino.h>

// https://github.com/ingeniuske/CSE7766
#include <CSE7766.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <supla/storage/storage.h>

namespace Supla {
namespace Sensor {

class CSE_7766 : public OnePhaseElectricityMeter {
 public:
  CSE_7766(HardwareSerial& serial);

  void onInit();
  void readValuesFromDevice();
  void onSaveState();
  void onLoadState();

  double getCurrentMultiplier();
  double getVoltageMultiplier();
  double getPowerMultiplier();
  _supla_int64_t getCounter();

  void setCurrentMultiplier(double value);
  void setVoltageMultiplier(double value);
  void setPowerMultiplier(double value);
  void setCounter(_supla_int64_t value);
  int handleCalcfgFromServer(TSD_DeviceCalCfgRequest* request);

  void iterateAlways();
  void unblockingDelay(unsigned long mseconds);
  void calibrate(double calibPower, double calibVoltage);

 protected:
  CSE7766 sensor;

  double currentMultiplier;
  double voltageMultiplier;
  double powerMultiplier;

  unsigned _supla_int64_t energy = 0;
  unsigned _supla_int64_t _energy =
      0;  // energy value read from memory at startup
};

};  // namespace Sensor
};  // namespace Supla

#endif
#endif