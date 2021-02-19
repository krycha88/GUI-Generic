#include "SuplaWebPageUpload.h"
#include "SuplaDeviceGUI.h"
#include "FS.h"

static const char uploadIndex[] PROGMEM =
    R"(<form class="formcenter" method="POST" action="/upload" enctype="multipart/form-data">
         <input type="file" accept=".dat" name="config"></br>
         <input type="checkbox" name='generateGUIDandAUTHKEY' value='1'>Generuj GUID & AUTHKEY</br></br>
         <input type="submit" value="Upload">
     </form>)";

File dataFile;

void createWebUpload() {
  // WebServer->httpServer->on(F("/upload"), HTTP_GET, handleUpload);
  WebServer->httpServer->on(getURL(PATH_UPLOAD), HTTP_GET, []() { handleUpload(); });
  // WebServer->httpServer->on(F("/upload"), HTTP_POST, handleFileUpload);
  WebServer->httpServer->on(
      getURL(PATH_UPLOAD), HTTP_POST,
      []() {
        if (WebServer->httpServer->hasArg("generateGUIDandAUTHKEY")) {
          if (WebServer->httpServer->arg("generateGUIDandAUTHKEY") == "1") {
            ConfigManager->setGUIDandAUTHKEY();
            ConfigManager->save();
          }
        }
        WebServer->httpServer->send(200);
      },
      handleFileUpload);
}

void handleUpload(int save) {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  webContentBuffer += SuplaSaveResult(save);
  webContentBuffer += SuplaJavaScript(PATH_UPLOAD);
  webContentBuffer += F("<div class='w'>");
  webContentBuffer += F("<h3>");
  webContentBuffer += F("Wgraj konfiguracje");
  webContentBuffer += F("</h3>");
  webContentBuffer += F("<br>");
  webContentBuffer += FPSTR(uploadIndex);
  webContentBuffer += F("</div>");
  webContentBuffer += F("<a href='");
  webContentBuffer += getURL(PATH_TOOLS);
  webContentBuffer += F("'><button>");
  webContentBuffer += S_RETURN;
  webContentBuffer += F("</button></a><br><br>");

  WebServer->sendContent();
  // WebServer->httpServer->send(200, PSTR("text/html"), FPSTR(uploadIndex));
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
        handleUpload(1);
      }
      else {
        handleUpload(6);
        // WebServer->httpServer->send(500, "text/plain", "500: couldn't create file");
      }
    }
  }
}