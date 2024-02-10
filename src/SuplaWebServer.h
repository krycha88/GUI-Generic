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

#ifndef SuplaWebServer_h
#define SuplaWebServer_h

#include "GUIGenericCommonDefined.h"
#include "GUI-Generic_Config.h"
#include "SuplaTemplateBoard.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WebServer.h>
#elif ARDUINO_ARCH_ESP32
#include <supla/ESP32WebServer/ESP32WebServer.h>
#endif

#ifdef SUPLA_OTA
#include "src/update/SuplaHTTPUpdateServer.h"
#endif

#include <supla/element.h>

#define ARG_PARM_URL    "url"
#define ARG_PARM_NUMBER "number"

#define PATH_START     "/"

#ifdef GUI_SENSOR_I2C_EXPENDER
#define INPUT_ADRESS_MCP23017 "iam"
#define INPUT_EXPENDER_TYPE   "uet"
#endif

class SuplaWebServer : public Supla::Element {
 public:
  SuplaWebServer();
  void begin();
  void sendHeaderStart();
  void sendContent(const String& content);
  void sendContent(double content);
  void sendHeaderEnd();

#ifdef ARDUINO_ARCH_ESP8266
  ESP8266WebServer* httpServer;
#elif ARDUINO_ARCH_ESP32
  ESP32WebServer* httpServer;
#endif

#ifdef SUPLA_OTA
  HTTPUpdateServer* httpUpdater;
#endif

  bool isLoggedIn(bool force = false);
  bool saveGPIO(const String& _input, uint8_t function, uint8_t nr = 0, const String& input_max = "\n");
#ifdef GUI_SENSOR_I2C_EXPENDER
  bool saveGpioMCP23017(const String& _input, uint8_t function, uint8_t nr = 0, const String& input_max = "\n");
#endif

 private:
  void iterateAlways();
  void createWebServer();
  void handleNotFound();

  bool chunkedSendHeader = false;
  bool isRunningWebServer = false;
};
void printFreeMemory();

#if defined(ESP8266)
#include <md5.h>
#endif
#if defined(ESP8266)

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup();
#endif
#endif  // SuplaWebServer_h