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
  client.setTimeout(5000); // timeout 5 sekund

  Serial.print("Connecting to ");
  Serial.print(parseURL->getHost());
  Serial.print(":");
  Serial.println(parseURL->getPort());

  if (!client.connect(parseURL->getHost().c_str(), parseURL->getPort())) {
    Serial.println("Connection failed");
    return BUILDER_UPDATE_FAILED;
  }

  // Wysłanie żądania GET
  client.print(String("GET ") + parseURL->getPath().c_str() + " HTTP/1.1\r\n" +
               "Host: " + parseURL->getHost().c_str() + "\r\n" +
               "User-Agent: ESPUpdater/1.0\r\n" +
               "Connection: close\r\n\r\n");

  // Odczyt nagłówków HTTP
  unsigned long startTime = millis();
  while (client.connected()) {
    if (millis() - startTime > 5000) { // timeout nagłówków
      Serial.println("Header read timeout");
      client.stop();
      return BUILDER_UPDATE_FAILED;
    }

    String line = client.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) { // koniec nagłówków
      Serial.println(F("UpdateBuilder - Headers received"));
      break;
    }
  }

  // Odczyt treści odpowiedzi do bufora
  const size_t BUFFER_SIZE = 512; // bezpieczny rozmiar bufora
  char buffer[BUFFER_SIZE];
  size_t pos = 0;

  startTime = millis();
  while (client.connected() || client.available()) {
    if (millis() - startTime > 5000) { // timeout odczytu
      Serial.println("Body read timeout");
      client.stop();
      return BUILDER_UPDATE_FAILED;
    }

    while (client.available() && pos < BUFFER_SIZE - 1) {
      char c = client.read();
      buffer[pos++] = c;
    }
  }

  buffer[pos] = '\0'; // zakończenie stringa
  client.stop();

  Serial.print("Update status: ");
  Serial.println(buffer);

  // Analiza odpowiedzi
  if (strstr(buffer, "NONE") != nullptr)   return BUILDER_UPDATE_NO_UPDATES;
  if (strstr(buffer, "WAIT") != nullptr)   return BUILDER_UPDATE_WAIT;
  if (strstr(buffer, "READY") != nullptr)  return BUILDER_UPDATE_READY;
  if (strstr(buffer, "UNKNOWN") != nullptr) return BUILDER_UPDATE_FAILED;
  if (strstr(buffer, "ERROR") != nullptr)  return BUILDER_UPDATE_FAILED;

  return BUILDER_UPDATE_FAILED;
}
#endif