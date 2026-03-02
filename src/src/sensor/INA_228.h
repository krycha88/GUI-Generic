#ifndef SRC_SUPLA_SENSOR_INA_228_H_
#define SRC_SUPLA_SENSOR_INA_228_H_

#ifdef SUPLA_INA228

#include <Wire.h>
#include "INA238.h"
#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>

namespace Supla {
namespace Sensor {

/*
Depending on the connections of the A0 and A1 pins, the INA228 can have any one of 16 different I2C addresses:
A1 Pin 	A0 Pin	I2C Address (Hex)	I2C Address (Decimal)
GND	GND	0x40	64
VCC	GND	0x41	65
SDA	GND	0x42	66
SCL	GND	0x43	67
GND	VCC	0x44	68
VCC	VCC	0x45	69
SDA	VCC	0x46	70
SCL	VCC	0x47	71
GND	SDA	0x48	72
VCC	SDA	0x49	73
SDA	SDA	0x4A	74
SCL	SDA	0x4B	75
GND	SCL	0x4C	76
VCC	SCL	0x4D	77
SDA	SCL	0x4E	78
SCL	SCL	0x4F	79

*/
class INA_228 : public OnePhaseElectricityMeter {
 public:
  INA_228(uint8_t address = 0x40, TwoWire *wire = &Wire) : INA(address, wire) {
    if (!INA.begin()) {
      SUPLA_LOG_DEBUG("Unable to find INA228");
    }
    else {
      SUPLA_LOG_DEBUG("INA228 is connected at address: 0x%x", address);
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
#endif  // SRC_SUPLA_SENSOR_INA_228_H_
