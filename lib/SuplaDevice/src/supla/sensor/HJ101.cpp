#include "hj101.h"

namespace Supla {
namespace Sensor {

HJ101::HJ101(int8_t pinCF,
             int8_t pinCF1,
             int8_t pinSEL,
             bool currentWhen,
             bool use_interrupts)
    : pinCF(pinCF),
      pinCF1(pinCF1),
      pinSEL(pinSEL),
      currentWhen(currentWhen),
      use_interrupts(use_interrupts) {
  hj101 = new HLW8012();
  hj101->begin(pinCF, pinCF1, pinSEL, currentWhen, use_interrupts);

  attachInterrupt(pinCF, hjl01_cf_interrupt, FALLING);
  attachInterrupt(pinCF1, hjl01_cf1_interrupt, FALLING);
}

void HJ101::onInit() {
  readValuesFromDevice();
  updateChannelValues();
}

void HJ101::readValuesFromDevice() {
  energy = _energy +
           (hj101->getEnergy() / 36);  // current energy value = value at start

  unsigned int _reactive = 0;
  double _pf = 0;
  double _current = hj101->getCurrent();
  unsigned int _voltage = hj101->getVoltage();
  unsigned int _active = hj101->getActivePower();
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
}

void HJ101::onSaveState() {
  double current_multiplier = getCurrentMultiplier();
  double voltage_multiplier = getVoltageMultiplier();
  double power_multiplier = getPowerMultiplier();

  Supla::Storage::WriteState((unsigned char *)&energy, sizeof(energy));
  Supla::Storage::WriteState((unsigned char *)&current_multiplier,
                             sizeof(current_multiplier));
  Supla::Storage::WriteState((unsigned char *)&voltage_multiplier,
                             sizeof(voltage_multiplier));
  Supla::Storage::WriteState((unsigned char *)&power_multiplier,
                             sizeof(power_multiplier));
}

void HJ101::onLoadState() {
  double current_multiplier;
  double voltage_multiplier;
  double power_multiplier;

  if (Supla::Storage::ReadState((unsigned char *)&energy, sizeof(energy))) {
    setCounter(energy);
  }

  if (Supla::Storage::ReadState((unsigned char *)&current_multiplier,
                                sizeof(current_multiplier))) {
    setCurrentMultiplier(current_multiplier);
  }

  if (Supla::Storage::ReadState((unsigned char *)&voltage_multiplier,
                                sizeof(voltage_multiplier))) {
    setVoltageMultiplier(voltage_multiplier);
  }

  if (Supla::Storage::ReadState((unsigned char *)&power_multiplier,
                                sizeof(power_multiplier))) {
    setPowerMultiplier(power_multiplier);
  }
}

double HJ101::getCurrentMultiplier() {
  return hj101->getCurrentMultiplier();
};

double HJ101::getVoltageMultiplier() {
  return hj101->getVoltageMultiplier();
};

double HJ101::getPowerMultiplier() {
  return hj101->getPowerMultiplier();
};

_supla_int64_t HJ101::getCounter() {
  return energy;
}

void HJ101::setCurrentMultiplier(double current_multiplier) {
  hj101->setCurrentMultiplier(current_multiplier);
};
void HJ101::setVoltageMultiplier(double voltage_multiplier) {
  hj101->setVoltageMultiplier(voltage_multiplier);
};
void HJ101::setPowerMultiplier(double power_multiplier) {
  hj101->setPowerMultiplier(power_multiplier);
};

void HJ101::setCounter(_supla_int64_t energy) {
  _energy = energy;  // ------- energy value read from memory at startup
  setFwdActEnergy(0, energy);
}

// When using interrupts we have to call the library entry point
// whenever an interrupt is triggered
void ICACHE_RAM_ATTR HJ101::hjl01_cf1_interrupt() {
  hj101->cf1_interrupt();
}

void ICACHE_RAM_ATTR HJ101::hjl01_cf_interrupt() {
  hj101->cf_interrupt();
}

void HJ101::calibrate(double calibPower, double calibVoltage) {
  unsigned long timeout1 = millis();
  while ((millis() - timeout1) < 10000) {
    delay(10);
  }

  Serial.print(F("[HLW] Active Power (W)    : "));
  Serial.println(hj101->getActivePower());
  Serial.print(F("[HLW] Voltage (V)         : "));
  Serial.println(hj101->getVoltage());
  Serial.print(F("[HLW] Current (A)         : "));
  Serial.println(hj101->getCurrent());

  hj101->expectedActivePower(calibPower);
  hj101->expectedVoltage(calibVoltage);
  hj101->expectedCurrent(calibPower / calibVoltage);

  unsigned long timeout2 = millis();
  while ((millis() - timeout2) < 2000) {
    delay(10);
  }

  double current_multi = getCurrentMultiplier();
  double voltage_multi = getVoltageMultiplier();
  double power_multi = getPowerMultiplier();

  Serial.print(F("[HLW] New current multiplier : "));
  Serial.println(current_multi);
  Serial.print(F("[HLW] New voltage multiplier : "));
  Serial.println(voltage_multi);
  Serial.print(F("[HLW] New power multiplier   : "));
  Serial.println(power_multi);
  Supla::Storage::ScheduleSave(2000);
  yield();
}

HLW8012 *HJ101::hj101 = nullptr;
};  // namespace Sensor
};  // namespace Supla