/*
  Based on: https://github.com/wmbusmeters/wmbusmeters/blob/master/src/driver_mkradio4.cc
  Copyright (C) 2019-2023 Fredrik Öhrström (gpl-3.0-or-later)
*/

#pragma once

#include "driver.h"

#include <vector>
#include <string>

struct Mkradio4: Driver
{
  Mkradio4() : Driver(std::string("mkradio4")) {
    // Techem Service 'T','C','H'
    addDetection(MANFCODE('T', 'C', 'H'), 0x62, 0x95);
    addDetection(MANFCODE('T', 'C', 'H'), 0x62, 0x70);
    addDetection(MANFCODE('T', 'C', 'H'), 0x72, 0x95);
    addDetection(MANFCODE('T', 'C', 'H'), 0x72, 0x70);
  };
  virtual std::map<std::string, float> get_values(std::vector<unsigned char> &telegram) override {
    std::map<std::string, float> ret_val{};
    if (detect(this->get_manufacturer(telegram), this->get_type(telegram), this->get_version(telegram))) {
      add_to_map(ret_val, "total_water_m3", this->get_total_water_m3(telegram));

      if (ret_val.size() > 0) {
        return ret_val;
      }
      else {
        return {};
      }
    }
    else {
      return {};
    }
  };

private:
  float get_total_water_m3(std::vector<unsigned char> &telegram) {
    float ret_val{};
    size_t i = 11;

    ret_val = ((((uint32_t)telegram[i+4] << 8) + (uint32_t)telegram[i+3]) / 10.0) + 
              ((((uint32_t)telegram[i+8] << 8) + (uint32_t)telegram[i+7]) / 10.0);

    return ret_val;
  };
};