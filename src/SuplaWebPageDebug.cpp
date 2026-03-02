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

#include "SuplaWebPageDebug.h"

#ifdef SUPLA_DEBUG_MODE
void createWebPageDebug() {
  WebServer->httpServer->on(getURL(PATH_DEBUG), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

    if (WebServer->httpServer->method() == HTTP_GET)
      handleDebug();
    else
      handleDebugSave();
  });
}

void handleDebug(int save) {
  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_DEBUG);

  addForm(F("post"), PATH_DEBUG);
  addFormHeader(F("Debug Information"));

  // Memory information
  addLabel(String(F("Free Heap: ")) + String(ESP.getFreeHeap()) + F(" bytes"));
#ifdef ARDUINO_ARCH_ESP8266
  addLabel(String(F("Heap Fragmentation: ")) + String(ESP.getHeapFragmentation()) + F("%"));
  addLabel(String(F("Max Free Block: ")) + String(ESP.getMaxFreeBlockSize()) + F(" bytes"));
#endif

  // Uptime
  unsigned long uptime = millis() / 1000;
  unsigned long days = uptime / 86400;
  unsigned long hours = (uptime % 86400) / 3600;
  unsigned long minutes = (uptime % 3600) / 60;
  unsigned long seconds = uptime % 60;
  addLabel(String(F("Uptime: ")) + String(days) + F("d ") + String(hours) + F("h ") + 
           String(minutes) + F("m ") + String(seconds) + F("s"));

  // WiFi information
  addLabel(String(F("WiFi RSSI: ")) + String(WiFi.RSSI()) + F(" dBm"));
  addLabel(String(F("WiFi IP: ")) + WiFi.localIP().toString());
  addLabel(String(F("WiFi MAC: ")) + WiFi.macAddress());

  // Flash information
#ifdef ARDUINO_ARCH_ESP8266
  addLabel(String(F("Flash Size: ")) + String(ESP.getFlashChipSize()) + F(" bytes"));
  addLabel(String(F("Sketch Size: ")) + String(ESP.getSketchSize()) + F(" bytes"));
  addLabel(String(F("Free Sketch Space: ")) + String(ESP.getFreeSketchSpace()) + F(" bytes"));
#elif defined(ARDUINO_ARCH_ESP32)
  addLabel(String(F("Sketch Size: ")) + String(ESP.getSketchSize()) + F(" bytes"));
  addLabel(String(F("Free Sketch Space: ")) + String(ESP.getFreeSketchSpace()) + F(" bytes"));
  addLabel(String(F("Chip Model: ")) + String(ESP.getChipModel()));
  addLabel(String(F("Chip Revision: ")) + String(ESP.getChipRevision()));
  addLabel(String(F("CPU Freq: ")) + String(ESP.getCpuFreqMHz()) + F(" MHz"));
#endif

  // Device information
  addLabel(String(F("Max Relays: ")) + String(ConfigManager->get(KEY_MAX_RELAY)->getValueInt()));
  addLabel(String(F("Max Buttons: ")) + String(ConfigManager->get(KEY_MAX_BUTTON)->getValueInt()));

  #ifdef SUPLA_DEBUG_MODE_Bool 
 addLabel(String(F("Bool variable: ")) + String(SUPLA_DEBUG_MODE_Bool));
 addCheckBox("db", F("Set Bool variable"), SUPLA_DEBUG_MODE_Bool);
  #endif

  #ifdef SUPLA_DEBUG_MODE_Number
 addLabel(String(F("Number variable: ")) + String(SUPLA_DEBUG_MODE_Number));
 addNumberBox("dn", F("Set Number variable"), F("Number"), false, String(SUPLA_DEBUG_MODE_Number));
  #endif

  #ifdef SUPLA_DEBUG_MODE_Text
  addLabel(String(F("Text variable: ")) + String(SUPLA_DEBUG_MODE_Text));
  addTextBox("dt", F("Set Text variable"), String(SUPLA_DEBUG_MODE_Text), 0, 64, false);
  #endif

  
#ifdef SUPLA_RGBW
  addLabel(String(F("Max RGBW: ")) + String(ConfigManager->get(KEY_MAX_RGBW)->getValueInt()));
#endif

#ifdef SUPLA_DS18B20
  addLabel(String(F("Max DS18B20: ")) + String(ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt()));
#endif
  addButtonSubmit(S_SAVE);
  addFormEnd();

  addButton(S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleDebugSave() {
  // Debug page doesn't save anything currently
  // This is placeholder for future debug settings
  
  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleDebug(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleDebug(2);
      break;
  }
}

#endif  // SUPLA_DEBUG
