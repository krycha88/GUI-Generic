/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SuplaWebPageSensorI2c_h
#define SuplaWebPageSensorI2c_h

#include "SuplaDeviceGUI.h"
#include <Wire.h>

#if defined(SUPLA_BME280) || defined(SUPLA_SHT3x) || defined(SUPLA_SI7021) || defined(SUPLA_OLED) || defined(GUI_SENSOR_I2C_EXPENDER) || \
    defined(SUPLA_BMP280) || defined(SUPLA_VL53L0X) || defined(SUPLA_HDC1080) || defined(SUPLA_LCD_HD44780) || defined(SUPLA_BH1750) ||  \
    defined(SUPLA_MAX44009) || defined(SUPLA_SHT_AUTODETECT)
#define GUI_SENSOR_I2C
#endif

/* Another one to support more sensors settings.*/
#if defined(SUPLA_MS5611) || defined(SUPLA_AHTX0)
#define GUI_SENSOR_I2C_2
#endif

#if defined(SUPLA_ADE7953)
#define GUI_SENSOR_I2C_ENERGY_METER
#endif

enum _sensor
{
  SENSOR_I2C_BME280,
  SENSOR_I2C_SHT3x,
  SENSOR_I2C_SI7021,
  SENSOR_SPI_MAX6675,
  SENSOR_I2C_OLED,
  SENSOR_I2C_FOR_USE_1,
  SENSOR_I2C_BMP280,
  SENSOR_I2C_VL53L0X,
  SENSOR_I2C_HDC1080,
  SENSOR_I2C_HD44780,
  SENSOR_SPI_MAX31855,
  SENSOR_I2C_BH1750,
  SENSOR_I2C_MAX44009,
  SENSOR_I2C_FOR_USE_2
};

/*Added to support configuration of more sensors.*/
enum _sensor2
{
  SENSOR_I2C_MS5611,
  SENSOR_SPI_CC1101,
  SENSOR_I2C_AHTX0
};

#if defined(GUI_SENSOR_I2C) || defined(GUI_SENSOR_I2C_ENERGY_METER) || defined(GUI_SENSOR_I2C_2)

#if defined(SUPLA_BME280) || defined(SUPLA_BMP280)
enum _bmeAdress
{
  BMx280_ADDRESS_0X76 = 1,
  BMx280_ADDRESS_0X77,
  BMx280_ADDRESS_0X76_AND_0X77
};

#define INPUT_BME280          "bme280"
#define INPUT_BMP280          "bmp280"
#define INPUT_ALTITUDE_BMx280 "abme280"
#endif

#ifdef SUPLA_SHT3x
enum _shtAdress
{
  SHT3x_ADDRESS_0X44 = 1,
  SHT3x_ADDRESS_0X45,
  SHT3x_ADDRESS_0X44_AND_0X45
};
#endif

#ifdef SUPLA_LCD_HD44780
enum _LCDAdress
{
  HD44780_ADDRESS_0X20 = 1,
  HD44780_ADDRESS_0X21,
  HD44780_ADDRESS_0X22,
  HD44780_ADDRESS_0X23,
  HD44780_ADDRESS_0X24,
  HD44780_ADDRESS_0X25,
  HD44780_ADDRESS_0X26,
  HD44780_ADDRESS_0X27,
  HD44780_ADDRESS_0X38,
  HD44780_ADDRESS_0X3F,
};
#endif

#define PATH_I2C "i2c"

#define INPUT_SDA "sda"
#define INPUT_SCL "scl"

#ifdef ARDUINO_ARCH_ESP32
#define INPUT_SDA_2 "sda2"
#define INPUT_SCL_2 "scl2"
#endif

#ifdef SUPLA_SHT3x
#define INPUT_SHT3x "sht30"
#endif

#ifdef SUPLA_SI7021
#define INPUT_SI7021 "si7021"
#endif

#if defined(SUPLA_OLED) || defined(SUPLA_LCD_HD44780)
#define INPUT_OLED                 "oled"
#define INPUT_LCD                  "ilcd"
#define INPUT_HD44780_TYPE         "iht"
#define INPUT_OLED_ANIMATION       "oleda"
#define INPUT_OLED_BRIGHTNESS_TIME "oledb"
#define INPUT_OLED_BRIGHTNESS_LVL  "oledc"
#define INPUT_OLED_NAME            "ion"
#define INPUT_BUTTON_OLED          "ibo"
#define INPUT_BUTTON_LCD           "ibl"
#endif

void createWebPageSensorI2c();
void handleSensorI2c(int save = 0);
void handleSensorI2cSave();
void webPageI2CScanner(TwoWire* wire);

#ifdef SUPLA_VL53L0X
#define INPUT_VL53L0X "ilox"
#endif

#ifdef SUPLA_HDC1080
#define INPUT_HDC1080 "ihdc"
#endif

#ifdef SUPLA_BH1750
#define INPUT_BH1750 "ibh"
#endif

#ifdef SUPLA_MS5611
#define INPUT_MS5611 "ms5611"
#define INPUT_ALTITUDE_MS5611 "ams5611"
#endif

#ifdef SUPLA_MAX44009
#define INPUT_MAX44009 "imax"
#endif

#ifdef SUPLA_ADE7953
#define INPUT_ADE7953_IRQ           "iai"
#define INPUT_ADE7953_COUNTER_VALUE "iacv"
#endif

#ifdef SUPLA_SHT_AUTODETECT
#define INPUT_SUPLA_SHT_AUTODETECT "issa"
#endif

#endif

#if defined(GUI_SENSOR_I2C_2)
#ifdef SUPLA_AHTX0
#define INPUT_AHTX0 "aht"
enum _ahtAdress
{
  AHT_ADDRESS_0X38 = 1,
  AHT_ADDRESS_0X39,
  AHT_ADDRESS_0X38_AND_0X39
};
#endif

#endif // defined(GUI_SENSOR_I2C)

#endif  // ifndef SuplaWebPageSensorI2c_h
