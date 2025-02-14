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
#ifdef SUPLA_OTA
#include "UpdateBuilder.h"
#include "../../SuplaDeviceGUI.h"

UpdateBuilder::UpdateBuilder(const String& url) {
  parseURL = new ParseURL(url + "&ver=" + String(BUILD_VERSION));
}

int UpdateBuilder::check() {
  WiFiClient client;

  Serial.print("connecting to ");
  Serial.print(parseURL->getHost());
  Serial.print(":");
  Serial.println(parseURL->getPort());

  if (!client.connect(parseURL->getHost().c_str(), parseURL->getPort())) {
    Serial.println("connection failed");
    return BUILDER_UPDATE_FAILED;
  }

  client.print(String("GET ") + parseURL->getPath().c_str() + " HTTP/1.1\r\n" + "Host: " + parseURL->getHost().c_str() + "\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected() || client.available()) {
    if (client.readStringUntil('\n') == "\r") {
      Serial.println(F("UpdateBuilder - Headers received"));
      break;
    }
  }

  String result = client.readStringUntil('\n');

  // #ifdef ARDUINO_ARCH_ESP8266
  //   String result = "";
  //   while (client.connected()) {
  //     if (client.available()) {
  //       char c = client.read();
  //       // Serial.write(c);
  //       if (c == '\n') {  // Headers received
  //         result = "";
  //       }
  //       else if (c != '\r') {
  //         result += c;
  //       }
  //     }
  //   }
  // #elif ARDUINO_ARCH_ESP32
  //   while (client.connected() || client.available()) {
  //     if (client.readStringUntil('\n') == "\r") {
  //       Serial.println(F("UpdateBuilder - Headers received"));
  //       break;
  //     }
  //   }

  //   String result = "";
  //   while (client.connected() || client.available()) {
  //     char c = client.read();
  //     result += c;
  //   }
  // #endif

  client.stop();

  Serial.print("Update status: ");

  if (result.endsWith("NONE")) {
    Serial.println("NONE");
    return BUILDER_UPDATE_NO_UPDATES;
  }
  if (result.endsWith("WAIT")) {
    Serial.println("WAIT");
    return BUILDER_UPDATE_WAIT;
  }

  if (result.endsWith("READY")) {
    Serial.println("READY");
    return BUILDER_UPDATE_READY;
  }

  if (result.endsWith("UNKNOWN")) {
    Serial.println("UNKNOWN");
    return BUILDER_UPDATE_FAILED;
  }

  if (result.endsWith("ERROR")) {
    Serial.println("ERROR");
    return BUILDER_UPDATE_FAILED;
  }

  return BUILDER_UPDATE_FAILED;
}
#endif