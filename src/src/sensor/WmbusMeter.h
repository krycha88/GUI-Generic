#ifndef _WmbusMeter_h
#define _WmbusMeter_h
#include <Arduino.h>

#include <rf_mbus.hpp>
#include <Drivers/driver.h>
#include <wmbus_utils.hpp>
#include <utils.hpp>
#include <SensorBase.h>

#include <stdint.h>
#include <string>
#include <vector>

#include <supla/element.h>

namespace Supla {
namespace Sensor {
class WmbusMeter {
 public:
  WmbusMeter(uint8_t mosi = 23, uint8_t miso = 19, uint8_t clk = 18, uint8_t cs = 5, uint8_t gdo0 = 4, uint8_t gdo2 = 2);

  void IterateAlways();
  std::map<std::string, Driver *> drivers_{};
  std::map<std::string, SensorBase *> sensors_{};
  void add_driver(Driver *driver);
  void add_sensor(SensorBase *sensor);
  bool decrypt_telegram(std::vector<unsigned char> &telegram, std::vector<unsigned char> key);
  float parse_frame(std::vector<unsigned char> &frame);

 private:
  float readValue = 0.0;
  int packetLength = 192;
  rf_mbus receiver;
};

ICACHE_RAM_ATTR void WmbusMeterIterateAlways();
};  // namespace Sensor
};  // namespace Supla

extern Supla::Sensor::WmbusMeter *meter;
#endif