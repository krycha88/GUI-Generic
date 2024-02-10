
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

// #ifndef _analog_reding_map_kpop_h
// #define _analog_reding_map_kpop_h

// #include <Arduino.h>
// #include <supla/sensor/general_purpose_measurement.h>
// #include <supla/storage/storage.h>

// #ifdef ARDUINO_ARCH_ESP32
// #include <driver/adc.h>
// #include <esp_adc_cal.h>
// #endif

// namespace Supla {
// namespace Sensor {

// #define NO_OF_SAMPLES 10

// class AnalogReding : public GeneralPurposeMeasurement {
//  public:
//   AnalogReding(uint8_t pin);

//   void onInit();

// #ifdef ARDUINO_ARCH_ESP32
//   adc1_channel_t get_adc1_chanel(uint8_t pin);
// #endif

//   uint16_t readValuesFromDevice();
//   virtual double getValue();

//   void iterateAlways();

//   void onSaveState();
//   void onLoadState();

//   double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

//   void calibrateMinValue();
//   void calibrateMaxValue();

//   void setMinValue(float value);
//   float getMinValue();

//   void setMaxValue(float value);
//   float getMaxValue();

//   void setMinDesiredValue(float value);
//   float getMinDesiredValue();

//   void setMaxDesiredValue(float value);
//   float getMaxDesiredValue();

//  protected:
//   uint8_t pin;

//   float min;
//   float max;
//   float minDesired;
//   float maxDesired;
// };

// };  // namespace Sensor
// };  // namespace Supla

// #endif
