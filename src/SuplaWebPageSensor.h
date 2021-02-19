#ifndef SuplaWebPageSensor_h
#define SuplaWebPageSensor_h

#include "SuplaDeviceGUI.h"
#include "SuplaWebServer.h"
#include "SuplaWebPageControl.h"

#define PATH_MULTI_DS                 "multids"
#define PATH_SAVE_MULTI_DS            "savemultids"
#define PATH_1WIRE                    "1wire"
#define PATH_SAVE_1WIRE               "save1wire"
#define PATH_I2C                      "i2c"
#define PATH_SAVE_I2C                 "savei2c"
#define PATH_SPI                      "spi"
#define PATH_SAVE_SPI                 "savespi"
#define PATH_OTHER                    "other"
#define PATH_SAVE_OTHER               "saveother"
#define PATH_IMPULSE_COUNTER_SET      "setimpulsecounter"
#define PATH_SAVE_IMPULSE_COUNTER_SET "savesetimpulsecounter"

#define INPUT_MULTI_DS_GPIO   "mdsg"
#define INPUT_DHT11_GPIO      "dht11"
#define INPUT_DHT22_GPIO      "dht22"
#define INPUT_SDA_GPIO        "sdag"
#define INPUT_SCL_GPIO        "sclg"
#define INPUT_BME280          "bme280"
#define INPUT_ALTITUDE_BME280 "abme280"
#define INPUT_SHT3x           "sht30"
#define INPUT_SI7021          "si7021"
#define INPUT_OLED            "oled"
#define INPUT_OLED_ANIMATION  "oleda"
#define INPUT_OLED_BRIGHTNESS "oledb"
#define INPUT_MCP23017        "mcp"
#define INPUT_SI7021_SONOFF   "si7021sonoff"
#define INPUT_MAX_DHT11       "mdht11"
#define INPUT_MAX_DHT22       "mdht22"
#define INPUT_MAX_DS18B20     "maxds"
#define INPUT_CLK_GPIO        "clk"
#define INPUT_CS_GPIO         "cs"
#define INPUT_D0_GPIO         "d0"
#define INPUT_MAX6675         "max6675"
#define INPUT_OLED_NAME       "ion"

enum _sensorI2C
{
  SENSOR_I2C_BME280,
  SENSOR_I2C_SHT3x,
  SENSOR_I2C_SI7021,
  SENSOR_I2C_MAX6675,
  SENSOR_I2C_OLED,
  SENSOR_I2C_MCP23017
};

#ifdef SUPLA_BME280
enum _bmeAdress
{
  BME280_ADDRESS_0X76 = 1,
  BME280_ADDRESS_0X77,
  BME280_ADDRESS_0X76_AND_0X77
};
#endif

#ifdef SUPLA_SHT3x
enum _shtAdress
{
  SHT3x_ADDRESS_0X44 = 1,
  SHT3x_ADDRESS_0X45,
  SHT3x_ADDRESS_0X44_AND_0X45
};
#endif

class SuplaWebPageSensor {
 public:
  SuplaWebPageSensor();
  void createWebPageSensor();

#ifdef SUPLA_DS18B20
#define INPUT_DS18B20_ADDR "dsaddr"
#define INPUT_DS18B20_NAME "dsname"

  void handleSearchDS();
  void handleDSSave();
  void showDS18B20(bool readonly = false);
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_SI7021) || defined(SUPLA_SHT3x) || defined(SUPLA_OLED) || defined(SUPLA_MCP23017)
  void handle1Wire();
  void handle1WireSave();
  void handlei2c();
  void handlei2cSave();
  void supla_webpage_i2c(int save);
#endif

#if defined(SUPLA_MAX6675)
  void handleSpi();
  void handleSpiSave();
#endif

 private:
#if defined(SUPLA_DS18B20) || defined(SUPLA_DHT11) || defined(SUPLA_DHT22) || defined(SUPLA_SI7021_SONOFF)
  void supla_webpage_1wire(int save);
#ifdef SUPLA_DS18B20
  void supla_webpage_search(int save);
#endif
#endif

#if defined(SUPLA_MAX6675)
  void supla_webpage_spi(int save);
#endif
};

extern SuplaWebPageSensor* WebPageSensor;
#endif  // SuplaWebPageSensor_h
