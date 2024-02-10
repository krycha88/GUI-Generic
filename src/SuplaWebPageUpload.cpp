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

#include "SuplaWebPageUpload.h"

#ifdef ARDUINO_ARCH_ESP8266
#include "FS.h"
#elif ARDUINO_ARCH_ESP32
#include "SPIFFS.h"
#endif

static const char uploadIndex[] PROGMEM =
    R"(<form class="formcenter" method="POST" action="/upload" enctype="multipart/form-data">
         <input type="file" accept=".dat" name="config"></br>
         <input type="checkbox" style="width:auto;" name='newGUID' value='1' />{g}</br></br>
         <input type="submit" value="{u}">
     </form>)";

File dataFile;

void createWebUpload() {
  WebServer->httpServer->on(getURL(PATH_UPLOAD), HTTP_GET, []() { handleUpload(); });

  WebServer->httpServer->on(
      getURL(PATH_UPLOAD), HTTP_POST,
      []() {
        if (WebServer->httpServer->hasArg("newGUID")) {
          if (WebServer->httpServer->arg("newGUID") == "1") {
            ConfigManager->setGUIDandAUTHKEY();
            ConfigManager->save();
          }
        }
        handleUpload(1);
      },
      handleFileUpload);
}

void handleUpload(int save) {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  WebServer->sendHeaderStart();

  String upload = FPSTR(uploadIndex);
  upload.replace("{g}", S_GENERATE_GUID_AND_KEY);
  upload.replace("{u}", S_UPLOAD);

  SuplaSaveResult(save);
  SuplaJavaScript(PATH_UPLOAD);
  WebServer->sendContent(F("<div class='w'>"));
  WebServer->sendContent(F("<h3>"));
  WebServer->sendContent(S_LOAD_CONFIGURATION);
  WebServer->sendContent(F("</h3><br>"));
  WebServer->sendContent(upload);
  WebServer->sendContent(F("</div>"));
  WebServer->sendContent(F("<a href='"));
  WebServer->sendContent(getURL(PATH_TOOLS));
  WebServer->sendContent(F("'><button>"));
  WebServer->sendContent(S_RETURN);
  WebServer->sendContent(F("</button></a><br><br>"));
  WebServer->sendHeaderEnd();
}

void handleFileUpload() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  if (SPIFFS.begin()) {
    HTTPUpload& upload = WebServer->httpServer->upload();

    if (upload.status == UPLOAD_FILE_START) {
      dataFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
      if (dataFile)
        dataFile.write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END) {
      if (dataFile) {
        dataFile.close();
        // WebServer->httpServer->sendHeader("Location", "/upload");
        // WebServer->httpServer->send(303);
        ConfigManager->load();
      }
      else {
        handleUpload(6);
        // WebServer->httpServer->send(500, "text/plain", "500: couldn't create file");
      }
    }
  }
}
