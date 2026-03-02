#ifndef SRC_SUPLA_SENSOR_INA_226_H_
#define SRC_SUPLA_SENSOR_INA_226_H_

#ifdef SUPLA_INA226

#include <Wire.h>
#include "INA226.h"
#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>

namespace Supla {
namespace Sensor {

class INA_226 : public OnePhaseElectricityMeter {
 public:
  INA_226(uint8_t address = 0x40, TwoWire *wire = &Wire) : INA(address, wire) {
    if (!INA.begin()) {
      SUPLA_LOG_DEBUG("Unable to find INA226");
    }
    else {
      SUPLA_LOG_DEBUG("INA226 is connected at address: 0x%x", address);
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
  INA226 INA;
};

};  // namespace Sensor
};  // namespace Supla
#endif
#endif  // SRC_SUPLA_SENSOR_INA_226_H_
