#ifndef GUI_Generic_Config_h
#define GUI_Generic_Config_h

// ##### User configuration #####
// #define USE_CUSTOM

#ifdef USE_CUSTOM
#define BUILD_VERSION "User GUI 1.0.1"

// #define TEMPLATE_BOARD_OLD
// #define TEMPLATE_BOARD_JSON

#ifdef TEMPLATE_BOARD_JSON
#define TEMPLATE_JSON "{\"NAME\":\"Shelly 2.5\",\"GPIO\":[320,0,32,0,224,193,0,0,640,192,608,225,3456,4736]}"
#elif defined(TEMPLATE_BOARD_OLD)
#define DEFAULT_TEMPLATE_BOARD BOARD_SHELLY2
#endif

// #define SUPLA_DEBUG_MODE
// #define SUPLA_OTA
// #define SUPLA_MDNS
// #define SUPLA_ENABLE_GUI
// #define SUPLA_ENABLE_SSL

// Language en - english, pl - polish (default if not defined UI_LANGUAGE), es- spanish, fr - french, de - german,
// #define UI_LANGUAGE de

// #define SUPLA_RELAY
// #define SUPLA_CONDITIONS
// #define SUPLA_BUTTON
// #define SUPLA_ACTION_TRIGGER
// #define SUPLA_LIMIT_SWITCH
// #define SUPLA_ROLLERSHUTTER
// #define SUPLA_CONFIG
// #define SUPLA_LED

// ##### 1Wire #####
// #define SUPLA_DS18B20
// #define SUPLA_DHT11
// #define SUPLA_DHT22
// #define SUPLA_SI7021_SONOFF

// ##### i2c #####
// #define SUPLA_BME280
// #define SUPLA_BMP280
// #define SUPLA_SHT3x
// #define SUPLA_SHT_AUTODETECT
// #define SUPLA_SI7021
// #define SUPLA_OLED
// #define SUPLA_MCP23017
// #define SUPLA_PCF8575
// #define SUPLA_PCF8574
// #define SUPLA_VL53L0X
// #define SUPLA_HDC1080
// #define SUPLA_LCD_HD44780
// #define SUPLA_MS5611
// #define SUPLA_AHTX0
// ##### SPI #####
// #define SUPLA_MAX6675
// #define SUPLA_MAX31855
// #define SUPLA_CC1101

// ##### Other #####
// #define SUPLA_HC_SR04
// #define SUPLA_IMPULSE_COUNTER
// #define SUPLA_HLW8012
// #define SUPLA_RGBW
// #define SUPLA_PUSHOVER
// #define SUPLA_DIRECT_LINKS
// #define SUPLA_PZEM_V_3
// #define SUPLA_CSE7766
// #define SUPLA_DEEP_SLEEP
// #define SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
// #define SUPLA_RF_BRIDGE
// #define SUPLA_ADE7953

// #define SUPLA_PMSX003 
// #define SUPLA_VINDRIKTNING_IKEA

// ##### Analog #####
// #define SUPLA_NTC_10K
// #define SUPLA_MPX_5XXX
// #define SUPLA_ANALOG_READING_MAP

#endif  // USE_CUSTOM
#endif  // GUI-Generic_Config_h
