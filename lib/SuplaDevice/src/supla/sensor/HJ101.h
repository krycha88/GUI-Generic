#ifndef _Hj101_h
#define _Hj101_h

#include <Arduino.h>
#include <EEPROM.h>

// https://github.com/xoseperez/hlw8012
#include <HLW8012.h>
#include <supla/element.h>
#include <supla/storage/storage.h>

#include "one_phase_electricity_meter.h"

namespace Supla {
namespace Sensor {

class HJ101 : public OnePhaseElectricityMeter, public Element {
 public:
  HJ101(int8_t pinCF,
        int8_t pinCF1,
        int8_t pinSEL,
        bool currentWhen = LOW,
        bool use_interrupts = true);

  void onInit();
  void readValuesFromDevice();
  void onSaveState();
  void onLoadState();

  double getCurrentMultiplier();
  double getVoltageMultiplier();
  double getPowerMultiplier();
  _supla_int64_t getCounter();

  void setCurrentMultiplier(double current_multiplier);
  void setVoltageMultiplier(double voltage_multiplier);
  void setPowerMultiplier(double power_multiplier);
  void setCounter(_supla_int64_t energy);

  static void ICACHE_RAM_ATTR hjl01_cf1_interrupt();
  static void ICACHE_RAM_ATTR hjl01_cf_interrupt();
  void calibrate(double calibPower, double calibVoltage);

 protected:
  static HLW8012 *hj101;
  int8_t pinCF;
  int8_t pinCF1;
  int8_t pinSEL;
  bool currentWhen;
  bool use_interrupts;

  unsigned _supla_int64_t energy;
  unsigned _supla_int64_t _energy;  // energy value read from memory at startup
};

};  // namespace Sensor
};  // namespace Supla

#endif