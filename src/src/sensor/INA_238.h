#ifndef SRC_SUPLA_SENSOR_INA_238_H_
#define SRC_SUPLA_SENSOR_INA_238_H_

#ifdef SUPLA_INA238

#include <Wire.h>
#include "../../lib/INA238/INA238.h"
#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>

namespace Supla {
namespace Sensor {

class INA_238 : public OnePhaseElectricityMeter {
 public:
  INA_238(uint8_t address = 0x40, TwoWire *wire = &Wire) : INA(address, wire) {
    if (!INA.begin()) {
      SUPLA_LOG_DEBUG("Unable to find INA238");
    }
    else {
      SUPLA_LOG_DEBUG("INA238 is connected at address: 0x%x", address);
    }
  }

  void onInit() override {
    readValuesFromDevice();
    updateChannelValues();
  }

  virtual void readValuesFromDevice() {
    setVoltage(0, INA.getBusVoltage() * 1000);    // Convert V to mV
    setCurrent(0, INA.getCurrent() * 1000);       // Convert A to mA
    setPowerActive(0, INA.getPower() * 100000);   // Convert W to 0.00001 W
  }

 protected:
  INA238 INA;
};

};  // namespace Sensor
};  // namespace Supla
#endif
#endif  // SRC_SUPLA_SENSOR_INA_238_H_
