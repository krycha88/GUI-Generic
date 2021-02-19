#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <flash_hal.h>
#include <FS.h>
#include "StreamString.h"
#include "SuplaHTTPUpdateServer.h"

#include "SuplaWebServer.h"
#include "SuplaDeviceGUI.h"
#include "GUIGenericCommon.h"

static const char serverIndex[] PROGMEM =
    R"(<!DOCTYPE html>
     <html lang='en'>
     <head>
         <meta charset='utf-8'>
         <meta name='viewport' content='width=device-width,initial-scale=1'/>
     </head>
     <body>
     <div>Flash Size: {f}kB</div>
     <div>Sketch Max Size: {s}kB</div>
     <form method='POST' action='' enctype='multipart/form-data'>
         <input type='file' accept='.bin,.bin.gz' name='firmware'>
         </br>
         <input type='submit' value='Update Firmware'>
     </form>
     </body>
     </html>)";
static const char successResponse[] PROGMEM = "<META http-equiv='refresh' content='10'>Update Success! Rebooting...";
static const char twoStepResponse[] PROGMEM =
    "<META http-equiv='refresh' content='5'><b>WARNING</b> only use 2-step OTA update. Use GUI-GenericUpdater.bin";

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug) {
  _serial_output = serial_debug;
  _server = NULL;
  _username = emptyString;
  _password = emptyString;
  _authenticated = false;
}

void ESP8266HTTPUpdateServer::setup(ESP8266WebServer* server, const String& path, const String& username, const String& password) {
  _server = server;
  _username = username;
  _password = password;

  _server->on(getURL(PATH_UPDATE_HENDLE), std::bind(&ESP8266HTTPUpdateServer::handleFirmwareUp, this));
  // handler for the /update form page
  _server->on(path.c_str(), HTTP_GET, [&]() {
    if (_username != emptyString && _password != emptyString && !_server->authenticate(_username.c_str(), _password.c_str()))
      return _server->requestAuthentication();

    String index = FPSTR(serverIndex);
    index.replace("{f}", String(ESP.getFlashChipRealSize() / 1024));
    index.replace("{s}", String(ESP.getFreeSketchSpace() / 1024));
    _server->send(200, PSTR("text/html"), index);
  });

  // handler for the /update form POST (once file upload finishes)
  _server->on(
      path.c_str(), HTTP_POST,
      [&]() {
        if (!_authenticated)
          return _server->requestAuthentication();
        if (Update.hasError()) {
          _server->send(200, F("text/html"), String(F("Update error: ")) + _updaterError);
        }
        else {
          _server->client().setNoDelay(true);
          _server->send_P(200, PSTR("text/html"), successResponse);
          delay(100);
          _server->client().stop();
          ESP.restart();
        }
      },
      [&]() {
        // handler for the file upload, get's the sketch bytes, and writes
        // them through the Update object
        HTTPUpload& upload = _server->upload();

        if (upload.status == UPLOAD_FILE_START) {
          _updaterError = String();
          if (_serial_output)
            Serial.setDebugOutput(true);

          _authenticated = (_username == emptyString || _password == emptyString || _server->authenticate(_username.c_str(), _password.c_str()));
          if (!_authenticated) {
            if (_serial_output)
              Serial.printf("Unauthenticated Update\n");
            return;
          }

          WiFiUDP::stopAll();
          if (_serial_output)
            Serial.printf("Update: %s\n", upload.filename.c_str());
          if (upload.name == "filesystem") {
            size_t fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
            close_all_fs();
            if (!Update.begin(fsSize, U_FS)) {  // start with max available size
              if (_serial_output)
                Update.printError(Serial);
            }
          }
          else {
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace, U_FLASH)) {  // start with max available size
              _setUpdaterError();
            }
          }
        }
        else if (_authenticated && upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()) {
          if (_serial_output)
            Serial.printf(".");
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            _server->send_P(200, PSTR("text/html"), twoStepResponse);

            _setUpdaterError();
          }
        }
        else if (_authenticated && upload.status == UPLOAD_FILE_END && !_updaterError.length()) {
          if (Update.end(true)) {  // true to set the size to the current progress
            if (_serial_output)
              Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          }
          else {
            _setUpdaterError();
          }
          if (_serial_output)
            Serial.setDebugOutput(false);
        }
        else if (_authenticated && upload.status == UPLOAD_FILE_ABORTED) {
          Update.end();
          if (_serial_output)
            Serial.println(F("Update was aborted"));
        }
        delay(0);
      });
}

void ESP8266HTTPUpdateServer::handleFirmwareUp() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  suplaWebPageUpddate();
}

void ESP8266HTTPUpdateServer::suplaWebPageUpddate() {
  webContentBuffer += SuplaJavaScript();
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += S_SOFTWARE_UPDATE;
  webContentBuffer += F("</h3>");
  webContentBuffer += F("<br>");
  webContentBuffer += F("<center>");
  webContentBuffer += F("<iframe src='");
  webContentBuffer += PATH_UPDATE;
  webContentBuffer += F("' frameborder='0' height='200'></iframe>");
  webContentBuffer += F("</center>");
  webContentBuffer += F("</div>");
  webContentBuffer += F("<a href='");
  webContentBuffer += PATH_TOOLS;
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");
  WebServer->sendContent();
}

void ESP8266HTTPUpdateServer::_setUpdaterError() {
  if (_serial_output)
    Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  Serial.println(str.c_str());
  _updaterError = str.c_str();
}
