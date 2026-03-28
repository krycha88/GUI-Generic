#ifdef SUPLA_DS18B20

#ifndef SuplaSensorDS18B20_h
#define SuplaSensorDS18B20_h

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <supla/log_wrapper.h>
#include "supla/sensor/thermometer.h"

namespace Supla {
namespace Sensor {

class OneWireBus {
 public:
  explicit OneWireBus(uint8_t pinNumber);

  int8_t getIndex(uint8_t *deviceAddress);

  uint8_t pin;
  OneWireBus *nextBus;
  uint32_t lastReadTime;
  DallasTemperature sensors;

 protected:
  OneWire oneWire;
};

class DS18B20 : public Thermometer {
 public:
  explicit DS18B20(uint8_t pin, uint8_t *deviceAddress = nullptr);

  void iterateAlways();
  double getValue();
  DallasTemperature &getHwSensors();
  static void findAndSaveDS18B20Addresses();
  void setDeviceAddress(uint8_t* deviceAddress);

 protected:
  static OneWireBus *oneWireBus;
  OneWireBus *myBus;
  DeviceAddress address;
  int8_t retryCounter;
  double lastValidValue;
  uint32_t lastReadTime;
};

};  // namespace Sensor
};  // namespace Supla

#endif  // SuplaSensorDS18B20_h
#endif