#ifndef SRC_SUPLA_SENSOR_INA_229_H_
#define SRC_SUPLA_SENSOR_INA_229_H_

#ifdef SUPLA_INA229

#include <SPI.h>
#include "INA229.h"
#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>

namespace Supla {
namespace Sensor {

class INA_229 : public OnePhaseElectricityMeter {
 public:
  INA_229(uint8_t csPin, SPIClass *spi = &SPI) : INA(csPin, spi) {
    if (!INA.begin()) {
      SUPLA_LOG_DEBUG("Unable to find INA229 (SPI)");
    }
    else {
      SUPLA_LOG_DEBUG("INA229 (SPI) is connected on CS pin: %d", csPin);
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
  INA229 INA;
};

};  // namespace Sensor
};  // namespace Supla
#endif
#endif  // SRC_SUPLA_SENSOR_INA_229_H_
