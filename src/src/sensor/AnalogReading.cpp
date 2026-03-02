
// /*
//   Copyright (C) krycha88

//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// */
#ifdef SUPLA_ANALOG_READING_KPOP
#include "AnalogReading.h"
#include <esp_adc/adc_cali_scheme.h>

#ifdef ARDUINO_ARCH_ESP32
namespace {
adc_oneshot_unit_handle_t adc_handle = nullptr;
adc_cali_handle_t cali_handle = nullptr;
}  // namespace

void Supla::Sensor::initADC() {
  if (!adc_handle) {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    if (adc_oneshot_new_unit(&init_config, &adc_handle) != ESP_OK) {
      Serial.println("ADC initialization failed or ADC1 is already in use");
      return;
    }
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc/adc_calibration.html

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);
  }
#endif

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle);
  }
#endif
}

void Supla::Sensor::cleanupADC() {
  if (adc_handle) {
    adc_oneshot_del_unit(adc_handle);
    adc_handle = nullptr;
  }
  if (cali_handle) {

    #if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_delete_scheme_line_fitting(cali_handle);
    #endif
    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_delete_scheme_curve_fitting(cali_handle);
    #endif
    cali_handle = nullptr;
  }
}

adc_channel_t Supla::Sensor::AnalogReading::get_ADC_channel(uint8_t pin) {
  switch (pin) {
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 0:
      return ADC_CHANNEL_0;

#endif
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 1:
      return ADC_CHANNEL_1;
#endif
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 2:
      return ADC_CHANNEL_2;
#endif
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 3:
      return ADC_CHANNEL_3;
#endif
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 4:
      return ADC_CHANNEL_4;
#endif
// here for C3 it is from ADC2 , needs some refactoring in higher function
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C3)
    case 5:
      return ADC_CHANNEL_5;
#endif
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5)
    case 6:
      return ADC_CHANNEL_6;

#endif
    case 32:
      return ADC_CHANNEL_4;
#ifndef CONFIG_IDF_TARGET_ESP32C3
    case 33:
      return ADC_CHANNEL_5;
    case 34:
      return ADC_CHANNEL_6;
    case 35:
      return ADC_CHANNEL_7;
#endif
    case 36:
      return ADC_CHANNEL_0;
    case 37:
      return ADC_CHANNEL_1;
    case 38:
      return ADC_CHANNEL_2;
    case 39:
      return ADC_CHANNEL_3;
    default:
      return static_cast<adc_channel_t>(-1);
  }
}
#endif

Supla::Sensor::AnalogReading::AnalogReading(uint8_t pin)
    : GeneralPurposeMeasurement(nullptr, false), pin(pin), min(0), max(0), minDesired(0), maxDesired(0) {
}

void Supla::Sensor::AnalogReading::onInit() {
  pinMode(pin, INPUT);
  channel.setNewValue(getValue());
  this->setRefreshIntervalMs(1000);

#ifdef ARDUINO_ARCH_ESP32
  initADC();

  adc_oneshot_chan_cfg_t chan_config = {.atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_DEFAULT};
  adc_oneshot_config_channel(adc_handle, get_ADC_channel(pin), &chan_config);
#endif
}

Supla::Sensor::AnalogReading::~AnalogReading() {
#ifdef ARDUINO_ARCH_ESP32
  cleanupADC();
#endif
}

uint16_t Supla::Sensor::AnalogReading::readValuesFromDevice() {
  uint16_t average = 0;
  int raw_value = 0;

#ifdef ARDUINO_ARCH_ESP32
  int calibrated_value = 0;
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
    if (adc_handle) {
      adc_oneshot_read(adc_handle, get_ADC_channel(pin), &raw_value);
      adc_cali_raw_to_voltage(cali_handle, raw_value, &calibrated_value);
      average += calibrated_value;
    }
  }
#else
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
    raw_value = analogRead(pin);
    average += raw_value;
  }
#endif

  average /= NO_OF_SAMPLES;
  return average;
}

double Supla::Sensor::AnalogReading::getValue() {
  double value;

  if (min == max || minDesired == maxDesired)
    return NAN;

  value = mapDouble(readValuesFromDevice(), min, max, minDesired, maxDesired);
  value = constrain(value, minDesired, maxDesired);

  return value;
}

void Supla::Sensor::AnalogReading::onSaveState() {
  Supla::Storage::WriteState((unsigned char *)&min, sizeof(min));
  Supla::Storage::WriteState((unsigned char *)&max, sizeof(max));
  Supla::Storage::WriteState((unsigned char *)&minDesired, sizeof(minDesired));
  Supla::Storage::WriteState((unsigned char *)&maxDesired, sizeof(maxDesired));
}

void Supla::Sensor::AnalogReading::onLoadState() {
  Supla::Storage::ReadState((unsigned char *)&min, sizeof(min));
  Supla::Storage::ReadState((unsigned char *)&max, sizeof(max));
  Supla::Storage::ReadState((unsigned char *)&minDesired, sizeof(minDesired));
  Supla::Storage::ReadState((unsigned char *)&maxDesired, sizeof(maxDesired));
}

double Supla::Sensor::AnalogReading::mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Supla::Sensor::AnalogReading::calibrateMinValue() {
  setMinValue(readValuesFromDevice());
  Serial.print(F("Calibrate - write MIN value: "));
  Serial.println(min);
  Supla::Storage::ScheduleSave(1000);
}

void Supla::Sensor::AnalogReading::calibrateMaxValue() {
  setMaxValue(readValuesFromDevice());
  Serial.print(F("Calibrate - write MAX value: "));
  Serial.println(max);
  Supla::Storage::ScheduleSave(1000);
}

void Supla::Sensor::AnalogReading::setMinValue(float value) {
  min = value;
  Supla::Storage::ScheduleSave(1000);
}

float Supla::Sensor::AnalogReading::getMinValue() {
  return min;
}

void Supla::Sensor::AnalogReading::setMaxValue(float value) {
  max = value;
  Supla::Storage::ScheduleSave(1000);
}

float Supla::Sensor::AnalogReading::getMaxValue() {
  return max;
}

void Supla::Sensor::AnalogReading::setMinDesiredValue(float value) {
  minDesired = value;
  Supla::Storage::ScheduleSave(1000);
}

float Supla::Sensor::AnalogReading::getMinDesiredValue() {
  return minDesired;
}

void Supla::Sensor::AnalogReading::setMaxDesiredValue(float value) {
  maxDesired = value;
  Supla::Storage::ScheduleSave(1000);
}

float Supla::Sensor::AnalogReading::getMaxDesiredValue() {
  return maxDesired;
}
#endif