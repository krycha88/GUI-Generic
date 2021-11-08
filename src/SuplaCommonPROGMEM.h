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

#ifndef SuplaCommonPROGMEM_h
#define SuplaCommonPROGMEM_h
#include <pgmspace.h>
#include "SuplaDeviceGUI.h"
#include "GUIGenericCommon.h"

#define PGMT(pgm_ptr) (reinterpret_cast<const __FlashStringHelper*>(pgm_ptr))

const char HTTP_META[] PROGMEM =
    "<!DOCTYPE HTML><html><head><meta http-equiv='content-type' content='text/html; charset=UTF-8'>"
	"<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>\n";
const char HTTP_STYLE[] PROGMEM =
	    "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue "
	    "Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:#00d151;color:#fff;line-"
	    "height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px "
	    "10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:#00d151;padding-right:5px}#l "
	    "path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;box-shadow:0 1px 3px rgba(0,0,0,.3)}h1,h3{margin:10px "
	    "8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica "
	    "Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-"
	    "bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px "
	    "#00d151;height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:#00d151;line-height:41px;}i "
	    "input,"
	    "select{width:calc(100% - "
	    "166px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:165px;-webkit-"
	    "appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px "
	    "2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;box-shadow:0 1px "
	    "3px rgba(0,0,0,.3);cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;box-shadow:0 1px 3px "
	    "rgba(0,0,0,.3);text-align:center;font-size:26px;z-index:100}"
	    ".dif{position:absolute;width:160px;z-index:0}.difl{margin-left:-10px}.difr{margin-left:310px}.dift{margin-top:-94px}"
	    ".iframe{width:160px;height:90px;border:0;display:none}"
		"@media all and (max-height:920px){.s{margin-top:80px}}"
		"@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 "
	    "8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto "
	    "20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 "
	    "12px;color:#00d151;font-size:13px;position:relative;line-height:18px}i input,select{width:calc(100% - "
	    "10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px "
	    "#00d151}select{width:100%;float:none;margin:0}}iframe{margin:auto;display:block;}.formcenter{color:#000;width:50%;margin: 25px auto 25px "
	    "auto;} input[type='checkbox' i]{appearance:auto;margin-top:10px;height:20px;width:calc(100% - 116px);}"
		"@media (max-width: 900px){input[type='checkbox' i]{width:auto;}.difr{margin-left:calc(100% - 190px);}}</style>";
const char HTTP_SCRIPT[] PROGMEM =
	"<script>"
	"function ifl(i){"
	"let r=new XMLHttpRequest;"
	"r.open('GET',i.src,true);"
	"r.onprogress=function(ev){"
	"let s=ev.target.status;"
	"let sfn=(s).toString()[0];"
	"switch(sfn) {"
	"case '2':"
	"r.abort();"
	"i.style.display='block';"
	"break;"
	"default:"
	"r.abort();"
	"i.style.display='none';"
	"};"
	"};"
	"r.send('');"
	"};"
	"</script>\n";
const char HTTP_DIV_START[] PROGMEM =
    "</head><body><div class='s'>";
const char HTTP_DIV_END[] PROGMEM =
    "</div></body></html>";
const char HTTP_LOGO[] PROGMEM =
    "<a href='/'><svg version='1.1' id='l' x='0' y='0' viewBox='0 0 200 200' xml:space='preserve'><path "
    "d='M59.3,2.5c18.1,0.6,31.8,8,40.2,23.5c3.1,5.7,4.3,11.9,4.1,18.3c-0.1,3.6-0.7,7.1-1.9,10.6c-0.2,0.7-0.1,1.1,0.6,1.5c12.8,7.7,25.5,15.4,38.3,"
    "23c2.9,1.7,5.8,3.4,8.7,5.3c1,0.6,1.6,0.6,2.5-0.1c4.5-3.6,9.8-5.3,15.7-5.4c12.5-0.1,22.9,7.9,25.2,19c1.9,9.2-2.9,19.2-11.8,23.9c-8.4,4.5-16.9,4."
    "5-25.5,0.2c-0.7-0.3-1-0.2-1.5,0.3c-4.8,4.9-9.7,9.8-14.5,14.6c-5.3,5.3-10.6,10.7-15.9,16c-1.8,1.8-3.6,3.7-5.4,5.4c-0.7,0.6-0.6,1,0,1.6c3.6,3.4,5."
    "8,7.5,6.2,12.2c0.7,7.7-2.2,14-8.8,18.5c-12.3,8.6-30.3,3.5-35-10.4c-2.8-8.4,0.6-17.7,8.6-22.8c0.9-0.6,1.1-1,0.8-2c-2-6.2-4.4-12.4-6.6-18.6c-6.3-"
    "17.6-12.7-35.1-19-52.7c-0.2-0.7-0.5-1-1.4-0.9c-12.5,0.7-23.6-2.6-33-10.4c-8-6.6-12.9-15-14.2-25c-1.5-11.5,1.7-21.9,9.6-30.7C32.5,8.9,42.2,4.2,"
    "53.7,2.7c0.7-0.1,1.5-0.2,2.2-0.2C57,2.4,58.2,2.5,59.3,2.5z "
    "M76.5,81c0,0.1,0.1,0.3,0.1,0.6c1.6,6.3,3.2,12.6,4.7,18.9c4.5,17.7,8.9,35.5,13.3,53.2c0.2,0.9,0.6,1.1,1.6,0.9c5.4-1.2,10.7-0.8,15.7,1.6c0.8,0.4,"
    "1.2,0.3,1.7-0.4c11.2-12.9,22.5-25.7,33.4-38.7c0.5-0.6,0.4-1,0-1.6c-5.6-7.9-6.1-16.1-1.3-24.5c0.5-0.8,0.3-1.1-0.5-1.6c-9.1-4.7-18.1-9.3-27.2-14c-"
    "6.8-3.5-13.5-7-20.3-10.5c-0.7-0.4-1.1-0.3-1.6,0.4c-1.3,1.8-2.7,3.5-4.3,5.1c-4.2,4.2-9.1,7.4-14.7,9.7C76.9,80.3,76.4,80.3,76.5,81z "
    "M89,42.6c0.1-2.5-0.4-5.4-1.5-8.1C83,23.1,74.2,16.9,61.7,15.8c-10-0.9-18.6,2.4-25.3,9.7c-8.4,9-9.3,22.4-2.2,32.4c6.8,9.6,19.1,14.2,31.4,11.9C79."
    "2,67.1,89,55.9,89,42.6z "
    "M102.1,188.6c0.6,0.1,1.5-0.1,2.4-0.2c9.5-1.4,15.3-10.9,11.6-19.2c-2.6-5.9-9.4-9.6-16.8-8.6c-8.3,1.2-14.1,8.9-12.4,16.6C88.2,183.9,94.4,188.6,"
    "102.1,188.6z "
    "M167.7,88.5c-1,0-2.1,0.1-3.1,0.3c-9,1.7-14.2,10.6-10.8,18.6c2.9,6.8,11.4,10.3,19,7.8c7.1-2.3,11.1-9.1,9.6-15.9C180.9,93,174.8,88.5,167.7,88.5z'/"
    "></svg></a>";
const char HTTP_SUMMARY[] PROGMEM =
    "<h1>{h}</h1><span>LAST STATE: {s}<br>Firmware: SuplaDevice {v}<br>GUID: {g}<br>MAC: {m}<br>Free Mem: {f}KB</span>\n";
const char HTTP_IFRAMES[] PROGMEM =
    "<div class='dif difl dift'><iframe class='iframe' src='https://gui-generic-builder.supla.io/f.php?tl' onload='ifl(this)'></iframe></div>"
    "<div class='dif difr dift'><iframe class='iframe' src='https://gui-generic-builder.supla.io/f.php?tr' onload='ifl(this)'></iframe></div>"
    "<div class='dif difl'><iframe class='iframe' src='https://gui-generic-builder.supla.io/f.php?l' onload='ifl(this)'></iframe></div>"
    "<div class='dif difr'><iframe class='iframe' src='https://gui-generic-builder.supla.io/f.php?r' onload='ifl(this)'></iframe></div>";

const char HTTP_FAVICON[] PROGMEM =
    "<link rel='icon' "
    "href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAAB3RJTUUH5AUUCCQbIwTFfgAAAAlwSFlzAAALEgAACxIB0t1+/"
    "AAAAARnQU1BAACxjwv8YQUAAAAwUExURf7nNZuNIOvWMci2KWRbFJGEHnRpFy8rCdrGLSAdBgwLAod6G7inJkI8DVJLEKeYIg6cTsoAAAGUSURBVHjaY2CAAFUGNLCF4QAyl4mhmP8BB4LPc"
    "WtdAe+BEBiX9QD77Kzl24GKHCAC/OVZH5hkVyUCFQlCRJhnKjAwLVlVb8lQDOY/ZFrG8FDVQbVqbU8BWODc3BX8dbMMGJhfrUyAaOla+/dAP8jyncsbgJTKgVP/"
    "b+pOAUudegAkGMsrGZhE1EFyDGwLwNaucmZyl1TgKTdg4JvAwMBzn3txeKWrMwP7wQcMWiAtf2c9YDjUfYBJapsDw66bm4AiUesOnJty0/O9iwLDPI5EhhCD6/"
    "q3Chk4dgCleJYpAEOmfCkDB+sbsK1886YBRfgWMTBwbi896wR04YZuAyAH6OmzDCbr3RgYsj6A1HEBPXCfgWHONgaG6eUBII0LFTiA7jn+iIF/"
    "MbMTyEu3lphtAJtpvl4BTLPNWgVSySA+y28aWIDdyGtVBgNH5psshVawwHGGO+arLr7MYFoJjZr/"
    "zBPYj85a1sC4ulwAIsIdcJzh2qt1WReYBWBR48gxgd1ziQIi6hTYEsxR45pZwRU9+oWgNAB1F3c/H6bYqgAAAABJRU5ErkJggg==' type='image/x-png' />\n";

const char HTTP_RBT[] PROGMEM = "<a href='/?reboot=1'><button>Restart</button></a>";

const char ICON_EDIT[] PROGMEM =
    "<img "
    "src='data:image/"
    "png;base64,"
    "iVBORw0KGgoAAAANSUhEUgAAAAwAAAAMCAQAAAD8fJRsAAAAB3RJTUUH5AYHChEfgNCVHgAAAAlwSFlzAAAuIwAALiMB"
    "eKU/dgAAAARnQU1BAACxjwv8YQUAAABBSURBVHjaY1BiwA4xhWqU/"
    "gMxAzZhEGRAF2ZQmoGpA6R6BlSaAV34P0QYIYEmDJPAEIZJQFxSg+"
    "kPDGFsHiQkAQDjTS5MMLyE4wAAAABJRU5ErkJggg=='>";

const char OFF[] PROGMEM = S_OFF;
const char ON[] PROGMEM = S_ON;
const char S_EMPTY[] PROGMEM = "";

#ifdef ARDUINO_ARCH_ESP8266
const char GPIO0[] PROGMEM = "0-D3";
const char GPIO1[] PROGMEM = "1-TX";
const char GPIO2[] PROGMEM = "2-D4";
const char GPIO3[] PROGMEM = "3-RX";
const char GPIO4[] PROGMEM = "4-D2";
const char GPIO5[] PROGMEM = "5-D1";
const char GPIO9[] PROGMEM = "9-FL (ESP8285)";
const char GPIO10[] PROGMEM = "10-FL (ESP8285)";
const char GPIO12[] PROGMEM = "12-D6";
const char GPIO13[] PROGMEM = "13-D7";
const char GPIO14[] PROGMEM = "14-D5";
const char GPIO15[] PROGMEM = "15-D8";
const char GPIO16[] PROGMEM = "16-D0";
const char GPIO17[] PROGMEM = "17-A0 ANALOG";

const char* const GPIO_P[] PROGMEM = {GPIO0,
                                      GPIO1,
                                      GPIO2,
                                      GPIO3,
                                      GPIO4,
                                      GPIO5,
                                      S_EMPTY,
                                      S_EMPTY,
                                      S_EMPTY
#ifdef ARDUINO_ESP8266_GENERIC
                                      ,
                                      S_EMPTY,
                                      S_EMPTY
#else
                                      ,
                                      GPIO9,
                                      GPIO10
#endif
                                      ,
                                      S_EMPTY,
                                      GPIO12,
                                      GPIO13,
                                      GPIO14,
                                      GPIO15,
                                      GPIO16,
                                      GPIO17,
                                      OFF};
#elif ARDUINO_ARCH_ESP32
const char GPIO0[] PROGMEM = "0-IO";
const char GPIO1[] PROGMEM = "1-TX";
const char GPIO2[] PROGMEM = "2-IO";
const char GPIO3[] PROGMEM = "3-RX";
const char GPIO4[] PROGMEM = "4-IO";
const char GPIO5[] PROGMEM = "5-IO";
const char GPIO12[] PROGMEM = "12-IO";
const char GPIO13[] PROGMEM = "13-IO";
const char GPIO14[] PROGMEM = "14-IO";
const char GPIO15[] PROGMEM = "15-IO";
const char GPIO16[] PROGMEM = "16-IO";
const char GPIO17[] PROGMEM = "17-IO";
const char GPIO18[] PROGMEM = "18-IO";
const char GPIO19[] PROGMEM = "19-IO";
const char GPIO21[] PROGMEM = "21-IO";
const char GPIO22[] PROGMEM = "22-IO";
const char GPIO23[] PROGMEM = "23-IO";
const char GPIO25[] PROGMEM = "25-IO";
const char GPIO26[] PROGMEM = "26-IO";
const char GPIO27[] PROGMEM = "27-IO";
const char GPIO32[] PROGMEM = "32-AO";
const char GPIO33[] PROGMEM = "33-AO";
const char GPIO34[] PROGMEM = "34-IA";
const char GPIO35[] PROGMEM = "35-IA";
const char GPIO36[] PROGMEM = "36-IA";
const char GPIO39[] PROGMEM = "39-IA";

const char* const GPIO_P[] PROGMEM = {GPIO0,   GPIO1,  GPIO2,   GPIO3,  GPIO4,   GPIO5,   S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY,
                                      S_EMPTY, GPIO12, GPIO13,  GPIO14, GPIO15,  GPIO16,  GPIO17,  GPIO18,  GPIO19,  S_EMPTY, GPIO21,
                                      GPIO22,  GPIO23, S_EMPTY, GPIO25, GPIO26,  GPIO27,  S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, GPIO32,
                                      GPIO33,  GPIO34, GPIO35,  GPIO36, S_EMPTY, S_EMPTY, GPIO39,  OFF};
#endif

#ifdef SUPLA_MCP23017
const char GPIO_A0[] PROGMEM = "A0";
const char GPIO_A1[] PROGMEM = "A1";
const char GPIO_A2[] PROGMEM = "A2";
const char GPIO_A3[] PROGMEM = "A3";
const char GPIO_A4[] PROGMEM = "A4";
const char GPIO_A5[] PROGMEM = "A5";
const char GPIO_A6[] PROGMEM = "A6";
const char GPIO_A7[] PROGMEM = "A7";
const char GPIO_B0[] PROGMEM = "B0";
const char GPIO_B1[] PROGMEM = "B1";
const char GPIO_B2[] PROGMEM = "B2";
const char GPIO_B3[] PROGMEM = "B3";
const char GPIO_B4[] PROGMEM = "B4";
const char GPIO_B5[] PROGMEM = "B5";
const char GPIO_B6[] PROGMEM = "B6";
const char GPIO_B7[] PROGMEM = "B7";

const char* const GPIO_MCP23017_P[] PROGMEM = {GPIO_A0, GPIO_A1, GPIO_A2, GPIO_A3, GPIO_A4, GPIO_A5, GPIO_A6, GPIO_A7, GPIO_B0,
                                               GPIO_B1, GPIO_B2, GPIO_B3, GPIO_B4, GPIO_B5, GPIO_B6, GPIO_B7, S_EMPTY, OFF};
#endif

#if defined(SUPLA_BME280) || defined(SUPLA_BMP280)
const char ADR76[] PROGMEM = "0x76";
const char ADR77[] PROGMEM = "0x77";
const char ADR76_ADR77[] PROGMEM = "0x76 & 0x77";
const char* const BMx280_P[] PROGMEM = {OFF, ADR76, ADR77, ADR76_ADR77};
#endif

#ifdef SUPLA_SHT3x
const char ADR44[] PROGMEM = "0x44";
const char ADR45[] PROGMEM = "0x45";
const char ADR44_ADR45[] PROGMEM = "0x44 & 0x45";
const char* const SHT3x_P[] PROGMEM = {OFF, ADR44, ADR45, ADR44_ADR45};
#endif

#ifdef SUPLA_MCP23017
const char ADR20[] PROGMEM = "0x20";
const char ADR21[] PROGMEM = "0x21";
const char ADR22[] PROGMEM = "0x22";
const char ADR23[] PROGMEM = "0x23";
const char* const MCP23017_P[] PROGMEM = {ADR20, ADR21, ADR22, ADR23, OFF};
#endif

const char* const STATE_P[] PROGMEM = {OFF, ON};

const char LOW_STATE_CONTROL[] PROGMEM = S_LOW;
const char HIGH_STATE_CONTROL[] PROGMEM = S_HIGH;
const char* const LEVEL_P[] PROGMEM = {LOW_STATE_CONTROL, HIGH_STATE_CONTROL};

const char POSITION_MEMORY[] PROGMEM = S_POSITION_MEMORY;
const char* const MEMORY_P[] PROGMEM = {OFF, ON, POSITION_MEMORY};

const char REACTION_ON_PRESS[] PROGMEM = S_REACTION_ON_PRESS;
const char REACTION_ON_RELEASE[] PROGMEM = S_REACTION_ON_RELEASE;
const char REACTION_ON_CHANGE[] PROGMEM = S_REACTION_ON_CHANGE;
const char* const TRIGGER_P[] PROGMEM = {REACTION_ON_PRESS, REACTION_ON_RELEASE, REACTION_ON_CHANGE};

const char ACTION_TOGGLE[] PROGMEM = S_TOGGLE;

const char* const ACTION_P[] PROGMEM = {ON, OFF, ACTION_TOGGLE};

#ifdef SUPLA_RF_BRIDGE
namespace Supla {
namespace GUI {

enum RFBridgeType
{
  TRANSMITTER,
  RECEIVER
};

}
}  // namespace Supla
const char TRANSMITTER[] PROGMEM = "Nadajnik";
const char RECEIVER[] PROGMEM = "Odbiornik";
const char* const RF_BRIDGE_TYPE_P[] PROGMEM = {TRANSMITTER, RECEIVER};
#endif

namespace Supla {
namespace GUI {
enum ActionRolleShutter
{
  STEP_BY_STEP,
  MOVE_UP_OR_MOVE_DOWN,
  OPEN_OR_CLOSE
};
}
}  // namespace Supla
#ifdef SUPLA_ROLLERSHUTTER
const char* const ACTION_ROLLER_SHUTTER_P[] PROGMEM = {"STEP BY STEP", "MOVE UP OR MOVE DOWN", "OPEN OR CLOSE"};
#endif

const char CFG_10_PRESSES[] PROGMEM = S_CFG_10_PRESSES;
const char CFG_5SEK_HOLD[] PROGMEM = S_5SEK_HOLD;
const char* const CFG_MODE_P[] PROGMEM = {CFG_10_PRESSES, CFG_5SEK_HOLD};

#ifdef SUPLA_OLED
const char SSD1306[] PROGMEM = "SSD1306 - 0,96''";
const char SH1106[] PROGMEM = "SH1106 - 1,3''";
const char SSD1306_WEMOS_SHIELD[] PROGMEM = "SSD1306 - 0,66'' WEMOS OLED shield";
const char* const OLED_P[] PROGMEM = {OFF, SSD1306, SH1106, SSD1306_WEMOS_SHIELD};

const char CONTROLL_NORMAL[] PROGMEM = S_NORMAL;
const char CONTROLL_SLOW[] PROGMEM = S_SLOW;
const char CONTROLL_MANUAL[] PROGMEM = S_MANUALLY;
const char* const OLED_CONTROLL_P[] PROGMEM = {CONTROLL_NORMAL, CONTROLL_SLOW, CONTROLL_MANUAL};
#endif

enum conditioningType
{
  HEATING,
  COOLING,
  MOISTURIZING,
  DRAINGE
};

/*const char CONDITIONING__HEATING[] PROGMEM = "Temperatura - ogrzewanie";
const char CONDITIONING__COOLING[] PROGMEM = "Temperatura - chłodzenie";
const char CONDITIONING__MOISTURIZING[] PROGMEM = "Wilgotność - nawilżanie";
const char CONDITIONING__DRAINGE[] PROGMEM = "Wilgotność - osuszanie";*/
const char CONDITIONING__HEATING[] PROGMEM = S_ON_CH_VAL_OFF_HEATING;
const char CONDITIONING__COOLING[] PROGMEM = S_ON_CH_VAL_OFF_COOLING;
const char CONDITIONING__MOISTURIZING[] PROGMEM = S_ON_2CH_VAL_OFF_HUMIDIFICATION;
const char CONDITIONING__DRAINGE[] PROGMEM = S_ON_2CH_VAL_OFF_DRYING;
const char* const CONDITIONS_TYPE_P[] PROGMEM = {CONDITIONING__HEATING, CONDITIONING__COOLING, CONDITIONING__MOISTURIZING, CONDITIONING__DRAINGE};

enum sensorList
{
  NO_SENSORS = 0
#ifdef SUPLA_DS18B20
  ,
  SENSOR_DS18B20
#endif
#ifdef SUPLA_DHT11
  ,
  SENSOR_DHT11
#endif
#ifdef SUPLA_DHT22
  ,
  SENSOR_DHT22
#endif
#ifdef SUPLA_SI7021_SONOFF
  ,
  SENSOR_SI7021_SONOFF
#endif
#ifdef SUPLA_HC_SR04
  ,
  SENSOR_HC_SR04
#endif
#ifdef SUPLA_BME280
  ,
  SENSOR_BME280
#endif
#ifdef SUPLA_SHT3x
  ,
  SENSOR_SHT3x
#endif
#ifdef SUPLA_SI7021
  ,
  SENSOR_SI7021
#endif
#ifdef SUPLA_MAX6675
  ,
  SENSOR_MAX6675
#endif
#ifdef SUPLA_NTC_10K
  ,
  SENSOR_NTC_10K
#endif
#ifdef SUPLA_BMP280
  ,
  SENSOR_BMP280
#endif
#ifdef SUPLA_MPX_5XXX
  ,
  SENSOR_MPX_5XXX,
  SENSOR_MPX_5XXX_PERCENT
#endif

#ifdef SUPLA_ANALOG_READING_MAP
  ,
  SENSOR_ANALOG_READING_MAP
#endif

#ifdef SUPLA_VL53L0X
  ,
  SENSOR_VL53L0X
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
  ,
  SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR
#endif

#ifdef SUPLA_HDC1080
  ,
  SENSOR_HDC1080
#endif
  ,
  COUNT_SENSOR_LIST
};

#ifdef SUPLA_DS18B20
const char NAME_DS18B20[] PROGMEM = S_DS18B20;
#endif
#ifdef SUPLA_DHT11
const char NAME_DHT11[] PROGMEM = S_DHT11;
#endif
#ifdef SUPLA_DHT22
const char NAME_DHT22[] PROGMEM = S_DHT22;
#endif
#ifdef SUPLA_SI7021_SONOFF
const char NAME_SI7021_SONOFF[] PROGMEM = S_SI7021_SONOFF;
#endif
#ifdef SUPLA_HC_SR04
const char NAME_HC_SR04[] PROGMEM = S_HC_SR04;
#endif
#ifdef SUPLA_BME280
const char NAME_BME280[] PROGMEM = S_BME280;
#endif
#ifdef SUPLA_SHT3x
const char NAME_SHT3x[] PROGMEM = S_SHT3X;
#endif
#ifdef SUPLA_SI7021
const char NAME_SI7021[] PROGMEM = S_SI702;
#endif
#ifdef SUPLA_MAX6675
const char NAME_MAX6675[] PROGMEM = S_MAX6675;
#endif
#ifdef SUPLA_NTC_10K
const char NAME_NTC_10K[] PROGMEM = S_NTC_10K;
#endif
#ifdef SUPLA_BMP280
const char NAME_BMP280[] PROGMEM = S_BMP280;
#endif
#ifdef SUPLA_MPX_5XXX
const char NAME_SENSOR_MPX_5XXX[] PROGMEM = S_MPX_5XXX;
const char NAME_SENSOR_MPX_5XXX_PERCENT[] PROGMEM = S_MPX_5XXX_PERCENT;
#endif
#ifdef SUPLA_VL53L0X
const char NAME_VL53L0X[] PROGMEM = "VL53L0X";
#endif
#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
const char NAME_SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR[] PROGMEM = "Direct Links Temp";
#endif
#ifdef SUPLA_HDC1080
const char NAME_HDC1080[] PROGMEM = "HDC1080";
#endif

const char* const SENSOR_LIST_P[] PROGMEM = {OFF
#ifdef SUPLA_DS18B20
                                             ,
                                             NAME_DS18B20
#endif
#ifdef SUPLA_DHT11
                                             ,
                                             NAME_DHT11
#endif
#ifdef SUPLA_DHT22
                                             ,
                                             NAME_DHT22
#endif
#ifdef SUPLA_SI7021_SONOFF
                                             ,
                                             NAME_SI7021_SONOFF
#endif
#ifdef SUPLA_HC_SR04
                                             ,
                                             NAME_HC_SR04
#endif
#ifdef SUPLA_BME280
                                             ,
                                             NAME_BME280
#endif
#ifdef SUPLA_SHT3x
                                             ,
                                             NAME_SHT3x
#endif
#ifdef SUPLA_SI7021
                                             ,
                                             NAME_SI7021
#endif
#ifdef SUPLA_MAX6675
                                             ,
                                             NAME_MAX6675
#endif
#ifdef SUPLA_NTC_10K
                                             ,
                                             NAME_NTC_10K
#endif
#ifdef SUPLA_BMP280
                                             ,
                                             NAME_BMP280
#endif
#ifdef SUPLA_MPX_5XXX
                                             ,
                                             NAME_SENSOR_MPX_5XXX,
                                             NAME_SENSOR_MPX_5XXX_PERCENT
#endif

#ifdef SUPLA_ANALOG_READING_MAP
                                             ,
                                             "ANALOG READING"
#endif

#ifdef SUPLA_VL53L0X
                                             ,
                                             NAME_VL53L0X
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
                                             ,
                                             NAME_SENSOR_DIRECT_LINKS_SENSOR_THERMOMETR
#endif
#ifdef SUPLA_HDC1080
                                             ,
                                             NAME_HDC1080
#endif
};

#endif  // SuplaCommonPROGMEM_h
