#ifdef SUPLA_DS18B20

#ifndef SuplaSensorDS18B20_h
#define SuplaSensorDS18B20_h

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWireNg.h>

#include <supla-common/log.h>
#include <supla/sensor/thermometer.h>

class DS18B20 : public Supla::Sensor::Thermometer {
 public:
  DS18B20(uint8_t* deviceAddress);
  void iterateAlways();
  double getValue();

  void onInit();
  static void initSharedResources(uint8_t pin);
  void setDeviceAddress(uint8_t* deviceAddress);
  static void waitForAndRequestTemperatures();
  void restartOneWire();

  static void findAndSaveDS18B20Addresses();

 private:
  static OneWire sharedOneWire;
  static DallasTemperature sharedSensors;

  static unsigned long lastConversionTime;
  const unsigned long conversionInterval = 10000;

  uint8_t address[8];
  double lastValidValue;
  uint8_t retryCounter;

  unsigned long lastUpdateTime;
};

#endif  // SuplaSensorDS18B20_h
#endif