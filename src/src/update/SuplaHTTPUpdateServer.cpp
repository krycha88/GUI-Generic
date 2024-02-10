#include "SuplaHTTPUpdateServer.h"
#include "UpdateURL.h"
#include "UpdateBuilder.h"
#include "../../SuplaDeviceGUI.h"
#include "SuplaUpdateCommon.h"

#include <WiFiUdp.h>

#include <FS.h>
#include "StreamString.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <EEPROM.h>
#endif

HTTPUpdateServer::HTTPUpdateServer(bool serial_debug) {
  _serial_output = serial_debug;
#ifdef OPTIONS_HASH
  _optionsHash.reserve(33);
  _optionsHash = String(OPTIONS_HASH).c_str();
#endif
}

void HTTPUpdateServer::setup() {
  WebServer->httpServer->on(getURL(PATH_UPDATE_HENDLE), std::bind(&HTTPUpdateServer::handleFirmwareUp, this));

  WebServer->httpServer->on(getURL(PATH_UPDATE), HTTP_GET, [&]() {
    if (!WebServer->isLoggedIn())
      return;

    String index = FPSTR(serverIndex);
    index.replace("{l}", S_LANG);
    index.replace("{m}", String(ESP.getFlashChipSize() / 1024));
    index.replace("{s}", String(ESP.getFreeSketchSpace() / 1024));
    index.replace("{u}", String(ESP.getSketchSize() / 1024));
    index.replace("{M}", S_SKETCH_MEMORY_SIZE);
    index.replace("{S}", S_SKETCH_UPLOAD_MAX_SIZE);
    index.replace("{U}", S_SKETCH_LOADED_SIZE);
    index.replace("{b}", S_UPDATE_FIRMWARE);
    if ((ESP.getFlashChipSize() / 1024) == 1024) {
      index.replace("{g}", twoStepButton);
      index.replace("{gu}", PATH_UPDATE_HENDLE_2STEP);
      index.replace("{gg}", S_GG_UPDATER);
    }
    else {
      index.replace("{g}", "");
    }
    WebServer->httpServer->send(200, PSTR("text/html"), index.c_str());
  });

  WebServer->httpServer->on(
      getURL(PATH_UPDATE), HTTP_POST, [&]() { successUpdateManualRefresh(); }, [&]() { updateManual(); });
}

String HTTPUpdateServer::getUpdateBuilderUrl() {
#ifdef BUILDER_TEST
  return String(HOST_BUILDER) + "test.php?firmware=" + _optionsHash.c_str();
#else
  return String(HOST_BUILDER) + "?firmware=" + _optionsHash.c_str();
#endif
}

void HTTPUpdateServer::handleFirmwareUp() {
  if (!WebServer->isLoggedIn())
    return;

  WebServer->sendHeaderStart();

  String sCommand = WebServer->httpServer->arg(ARG_PARM_URL);

  if (!sCommand.isEmpty()) {
    UpdateURL* update = nullptr;

#ifdef OPTIONS_HASH
    if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_BUILDER) == 0 || strcasecmp_P(sCommand.c_str(), PATH_UPDATE_CHECK_BUILDER) == 0) {
      UpdateBuilder* updateBuilder = new UpdateBuilder(getUpdateBuilderUrl());

      switch (updateBuilder->check()) {
        case BUILDER_UPDATE_FAILED:
          suplaWebPageUpddate(SaveResult::UPDATE_ERROR, PATH_UPDATE_HENDLE);
          break;
        case BUILDER_UPDATE_NO_UPDATES:
          suplaWebPageUpddate(SaveResult::UPDATE_NO_UPDATES, PATH_UPDATE_HENDLE);
          break;
        case BUILDER_UPDATE_WAIT:
          suplaWebPageUpddate(SaveResult::UPDATE_WAIT, PATH_UPDATE_HENDLE);
          break;
        case BUILDER_UPDATE_READY:

          if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_CHECK_BUILDER) == 0) {
            suplaWebPageUpddate(SaveResult::UPDATE_NEW_VERSION, PATH_UPDATE_HENDLE);
            break;
          }

          if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_BUILDER) == 0) {
#ifdef ARDUINO_ARCH_ESP8266
            update = new UpdateURL(getUpdateBuilderUrl() + "&type=gz");
#elif ARDUINO_ARCH_ESP32
            update = new UpdateURL(getUpdateBuilderUrl() + "&type=bin");
#endif
          }
          break;
      }
    }
#endif
    if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_URL) == 0) {
      if (strcmp(WebServer->httpServer->arg(INPUT_UPDATE_URL).c_str(), "") != 0) {
#ifdef ARDUINO_ARCH_ESP8266
        update = new UpdateURL(WebServer->httpServer->arg(INPUT_UPDATE_URL) + "&type=gz");
#elif ARDUINO_ARCH_ESP32
        update = new UpdateURL(WebServer->httpServer->arg(INPUT_UPDATE_URL) + "&type=bin");
#endif
      }
      else {
        suplaWebPageUpddate(SaveResult::UPDATE_ERROR, PATH_UPDATE_HENDLE);
      }
    }
    if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_HENDLE_2STEP) == 0) {
      update = new UpdateURL(String(HOST_BUILDER) + "files/GUI-GenericUploader.bin");
    }

    if (update) {
      switch (update->update()) {
        case HTTP_UPDATE_FAILED:
#ifdef ARDUINO_ARCH_ESP8266
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());

          if (ESPhttpUpdate.getLastError() == HTTP_UE_TOO_LESS_SPACE) {
            // suplaWebPageUpddate(SaveResult::UPDATE_TOO_LESS_SPACE, PATH_UPDATE_HENDLE);
            autoUpdate2Step();
            break;
          }

#elif ARDUINO_ARCH_ESP32
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
#endif
          suplaWebPageUpddate(SaveResult::UPDATE_ERROR, PATH_UPDATE_HENDLE);
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");

          if (strcasecmp_P(sCommand.c_str(), PATH_UPDATE_HENDLE_2STEP) == 0) {
            successUpdateManualRefresh();
          }
          else {
            suplaWebPageUpddate(SaveResult::UPDATE_SUCCESS, PATH_START);
            ConfigESP->rebootESP();
          }
          break;
      }
    }
  }
  else {
    suplaWebPageUpddate();
  }
  WebServer->sendHeaderEnd();
}

void HTTPUpdateServer::suplaWebPageUpddate(int save, const String& location) {
  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(location);

#ifdef OPTIONS_HASH
  addFormHeader(String(S_UPDATE) + S_SPACE + "automatyczna");
  addButton("Sprawdź aktualizację", getParameterRequest(PATH_UPDATE_HENDLE, ARG_PARM_URL, PATH_UPDATE_CHECK_BUILDER));
  addButton(S_UPDATE_FIRMWARE, getParameterRequest(PATH_UPDATE_HENDLE, ARG_PARM_URL, PATH_UPDATE_BUILDER));
  addHyperlink("Pobierz", getUpdateBuilderUrl());
  addFormHeaderEnd();
#endif

  addForm(F("post"), getParameterRequest(PATH_UPDATE_HENDLE, ARG_PARM_URL, PATH_UPDATE_URL));
  addFormHeader(String(S_UPDATE) + S_SPACE + "OTA URL");
  addTextBox(INPUT_UPDATE_URL, String(S_ADDRESS) + S_SPACE + "URL", S_EMPTY, 0, 600, false);
  addButtonSubmit(S_UPDATE_FIRMWARE);
  addFormEnd();
  addFormHeaderEnd();

  addFormHeader(String(S_UPDATE) + S_SPACE + "ręczna");
  WebServer->sendContent(F("<iframe src='"));
  WebServer->sendContent(getURL(PATH_UPDATE));
  WebServer->sendContent(F("' frameborder='0' width='330' height='200'></iframe>"));
  addFormHeaderEnd();
  addButton(S_RETURN, PATH_TOOLS);
  WebServer->sendHeaderEnd();
}

void HTTPUpdateServer::successUpdateManualRefresh() {
  String succes = FPSTR(successResponse);
  succes.replace("{m}", S_UPDATE_SUCCESS_REBOOTING);
  WebServer->httpServer->client().setNoDelay(true);
  WebServer->httpServer->send(200, F("text/html"), succes.c_str());
  delay(100);
  WebServer->httpServer->client().stop();
  ConfigESP->rebootESP();
}

#ifdef ARDUINO_ARCH_ESP8266
void HTTPUpdateServer::autoUpdate2Step() {
#define MAX_HASH 33

  struct {
    char SSID[MAX_SSID] = "";
    char PASS[MAX_PASSWORD] = "";
    char HASH[MAX_HASH] = "";
    int CFG_LED = -1;
  } settings;

  EEPROM.begin(1024);

  strncpy(settings.SSID, ConfigManager->get(KEY_WIFI_SSID)->getValue(), MAX_SSID);
  strncpy(settings.PASS, ConfigManager->get(KEY_WIFI_PASS)->getValue(), MAX_PASSWORD);
  strncpy(settings.HASH, _optionsHash.c_str(), MAX_HASH);

  if (ConfigESP->getGpio(FUNCTION_CFG_LED) != OFF_GPIO)
    settings.CFG_LED = ConfigESP->getGpio(FUNCTION_CFG_LED);

  unsigned int address = 800;
  EEPROM.put(address, settings);

  EEPROM.end();

  UpdateURL* update = new UpdateURL(String(HOST_BUILDER) + "files/AutoUploader.bin");

  if (update) {
    switch (update->update()) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());

        if (ESPhttpUpdate.getLastError() == HTTP_UE_TOO_LESS_SPACE) {
          suplaWebPageUpddate(SaveResult::UPDATE_TOO_LESS_SPACE, PATH_UPDATE_HENDLE);
          break;
        }
        suplaWebPageUpddate(SaveResult::UPDATE_ERROR, PATH_UPDATE_HENDLE);
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        suplaWebPageUpddate(SaveResult::UPDATE_2STEP, PATH_START);
        ConfigESP->rebootESP();
        break;
    }
  }
}

void HTTPUpdateServer::setUpdaterError() {
  if (_serial_output)
    Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  Serial.println(str.c_str());
  _updaterError = str.c_str();
}

void HTTPUpdateServer::updateManual() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  WebServer->sendHeaderStart();

  // handler for the file upload, get's the sketch bytes, and writes
  // them through the Update object
  HTTPUpload& upload = WebServer->httpServer->upload();

  if (upload.status == UPLOAD_FILE_START) {
    _updaterError = String();
    if (_serial_output)
      Serial.setDebugOutput(true);

    WiFiUDP::stopAll();
    if (_serial_output)
      Serial.printf("Update: %s\n", upload.filename.c_str());
    if (upload.name == "filesystem") {
      size_t fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
      close_all_fs();
      if (!Update.begin(fsSize, U_FS, ConfigESP->getGpio(FUNCTION_CFG_LED),
                        ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)))) {  // start with max available size
        if (_serial_output)
          Update.printError(Serial);
      }
    }
    else {
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace, U_FLASH, ConfigESP->getGpio(FUNCTION_CFG_LED),
                        ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)))) {  // start with max available size
        setUpdaterError();
      }
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()) {
    if (_serial_output)
      Serial.printf(".");
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      String twoStep = FPSTR(twoStepResponse);
      twoStep.replace("{w}", S_WARNING);
      twoStep.replace("{o}", S_ONLY_2_STEP_OTA);
      twoStep.replace("{gg}", S_GG_UPDATER);
      WebServer->httpServer->send(200, F("text/html"), twoStep.c_str());

      setUpdaterError();
    }
  }
  else if (upload.status == UPLOAD_FILE_END && !_updaterError.length()) {
    if (Update.end(true)) {  // true to set the size to the current progress
      if (_serial_output)
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    }
    else {
      setUpdaterError();
    }
    if (_serial_output)
      Serial.setDebugOutput(false);
  }
  else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.end();
    if (_serial_output)
      Serial.println(F("Update was aborted"));
  }
  delay(0);
  WebServer->sendHeaderEnd();
}
#elif ARDUINO_ARCH_ESP32

void HTTPUpdateServer::updateManual() {
  if (!WebServer->isLoggedIn()) {
    return;
  }
  WebServer->sendHeaderStart();
  HTTPUpload& upload = WebServer->httpServer->upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH, ConfigESP->getGpio(FUNCTION_CFG_LED),
                      ConfigESP->getLevel(ConfigESP->getGpio(FUNCTION_CFG_LED)))) {  // start with max available size
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {  // true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    }
    else {
      Update.printError(Serial);
    }
  }
  delay(0);
  WebServer->sendHeaderEnd();
}
#endif
