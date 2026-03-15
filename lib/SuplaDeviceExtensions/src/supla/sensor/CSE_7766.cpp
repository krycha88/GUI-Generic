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
#include "CSE_7766.h"

namespace Supla {
namespace Sensor {

CSE_7766::CSE_7766(HardwareSerial &serial)
    : sensor(serial),
      currentMultiplier(0.95),
      voltageMultiplier(2.37),
      powerMultiplier(2.52) {
}

void CSE_7766::onInit() {
  extChannel.setFlag(SUPLA_CHANNEL_FLAG_CALCFG_RESET_COUNTERS);

  sensor.setCurrentRatio(currentMultiplier);
  sensor.setVoltageRatio(voltageMultiplier);
  sensor.setPowerRatio(powerMultiplier);

  sensor.begin();

  readValuesFromDevice();
  updateChannelValues();
}

void CSE_7766::readValuesFromDevice() {
  bool currentChanelRelay = false;
  sensor.handle();

  for (auto element = Supla::Element::begin(); element != nullptr;
       element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
        if (channel->getValueBool()) currentChanelRelay = true;
      }
    }
  }

  if (currentChanelRelay) {
    energy = _energy + (sensor.getEnergy() /
                        36);  // current energy value = value at start
  }

  unsigned int _reactive = 0;
  double _pf = 0;
  double _current = sensor.getCurrent();
  unsigned int _voltage = sensor.getVoltage();
  unsigned int _active = sensor.getActivePower();
  unsigned int _apparent = _voltage * _current;
  if (_apparent > _active) {
    _reactive = sqrt(_apparent * _apparent - _active * _active);
  } else {
    _reactive = 0;
  }
  if (_active > _apparent) {
    _pf = 1;
  }
  if (_apparent == 0) {
    _pf = 0;
  } else {
    _pf = (double)_active / _apparent;
  }
  setVoltage(0, _voltage * 100);            // voltage in 0.01 V
  setCurrent(0, _current * 1000);           // current in 0.001 A
  setPowerActive(0, _active * 100000);      // power in 0.00001 kW
  setFwdActEnergy(0, energy);               // energy in 0.00001 kWh
  setPowerApparent(0, _apparent * 100000);  // power in 0.00001 kVA
  setPowerReactive(0, _reactive * 100000);  // power in 0.00001 kvar
  setPowerFactor(0, _pf * 1000);            // power in 0.001

  /* // voltage in 0.01 V
  setVoltage(0, sensor.getVoltage() * 100);
  // current in 0.001 A
  setCurrent(0, sensor.getCurrent() * 1000);
  // power in 0.00001 kW
  setPowerActive(0, sensor.getActivePower() * 100000);
  // energy in 0.00001 kWh
  setFwdActEnergy(0, energy);
  // power in 0.00001 kVA
  setPowerApparent(0, sensor.getApparentPower() * 100000);
  // power in 0.00001 kvar
  setPowerReactive(0, sensor.getReactivePower() * 100000);
  // power in 0.001
  setPowerFactor(0, sensor.getPowerFactor() * 1000); */
}

void CSE_7766::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&currentMultiplier,
                             sizeof(currentMultiplier));
  Supla::Storage::WriteState((unsigned char *)&voltageMultiplier,
                             sizeof(voltageMultiplier));
  Supla::Storage::WriteState((unsigned char *)&powerMultiplier,
                             sizeof(powerMultiplier));
}

void CSE_7766::onLoadState() {
  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }

  Supla::Storage::ReadState((unsigned char *)&currentMultiplier,
                            sizeof(currentMultiplier));

  Supla::Storage::ReadState((unsigned char *)&voltageMultiplier,
                            sizeof(voltageMultiplier));

  Supla::Storage::ReadState((unsigned char *)&powerMultiplier,
                            sizeof(powerMultiplier));
}

double CSE_7766::getCurrentMultiplier() {
  return currentMultiplier;
}

double CSE_7766::getVoltageMultiplier() {
  return voltageMultiplier;
}

double CSE_7766::getPowerMultiplier() {
  return powerMultiplier;
}

_supla_int64_t CSE_7766::getCounter() {
  return energy;
}

void CSE_7766::setCurrentMultiplier(double value) {
  currentMultiplier = value;
  sensor.setCurrentRatio(value);
}

void CSE_7766::setVoltageMultiplier(double value) {
  voltageMultiplier = value;
  sensor.setVoltageRatio(value);
}

void CSE_7766::setPowerMultiplier(double value) {
  powerMultiplier = value;
  sensor.setPowerRatio(value);
}

void CSE_7766::setCounter(_supla_int64_t value) {
  _energy = value;  // ------- energy value read from memory at startup
  energy = value;
  sensor.resetEnergy(value);
  setFwdActEnergy(0, value);
}

int CSE_7766::handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request) {
  if (request && request->Command == SUPLA_CALCFG_CMD_RESET_COUNTERS) {
    setCounter(0);
    Supla::Storage::ScheduleSave(1000);
    return SUPLA_CALCFG_RESULT_DONE;
  }
  return SUPLA_CALCFG_RESULT_NOT_SUPPORTED;
}

void CSE_7766::iterateAlways() {
  sensor.handle();
  Supla::Sensor::ElectricityMeter::iterateAlways();
}

void CSE_7766::unblockingDelay(unsigned long mseconds) {
  unsigned long timeout = millis();
  while ((millis() - timeout) < mseconds) {
    sensor.handle();
    delay(1);
  };
}

void CSE_7766::calibrate(double calibPower, double calibVoltage) {
  unblockingDelay(5000);

  Serial.print(F("Active Power (W)    : "));
  Serial.println(sensor.getActivePower());

  Serial.print(F("Current (A)         : "));
  Serial.println(sensor.getCurrent());

  Serial.print(F("Voltage (V)         : "));
  Serial.println(sensor.getVoltage());

  sensor.expectedPower(calibPower);
  sensor.expectedVoltage(calibVoltage);
  sensor.expectedCurrent(calibPower / calibVoltage);

  currentMultiplier = sensor.getCurrentRatio();
  voltageMultiplier = sensor.getVoltageRatio();
  powerMultiplier = sensor.getPowerRatio();

  Serial.print(F("New current multiplier : "));
  Serial.println(currentMultiplier);
  Serial.print(F("New voltage multiplier : "));
  Serial.println(voltageMultiplier);
  Serial.print(F("New power multiplier   : "));
  Serial.println(powerMultiplier);
  Supla::Storage::ScheduleSave(2000);
  delay(0);
}
};  // namespace Sensor
};  // namespace Supla
#endif
