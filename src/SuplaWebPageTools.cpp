#include "SuplaWebPageTools.h"
#include "SuplaDeviceGUI.h"

void createWebTools() {
  WebServer->httpServer->on(getURL(PATH_TOOLS), handleTools);

  WebServer->httpServer->on(getURL(PATH_FACTORY_RESET), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }
    WebServer->httpServer->sendHeader("Location", "/");
    WebServer->httpServer->send(303);
    WebServer->supla_webpage_start(0);
    ConfigESP->factoryReset(true);
  });
}

void handleTools() {
  if (!WebServer->isLoggedIn()) {
    return;
  }

  addFormHeader(webContentBuffer, F("Narzędzia"));
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, F("Zapisz konfigurację"), PATH_DOWNLOAD);
  //#endif
  //#ifdef SUPLA_BUTTON
  addButton(webContentBuffer, F("Wczytaj konfigurację"), PATH_UPLOAD);
  //#endif
#ifdef SUPLA_OTA
  addButton(webContentBuffer, S_UPDATE, PATH_UPDATE_HENDLE);
#endif
  addButton(webContentBuffer, F("Przywróć ustawienia fabryczne"), PATH_FACTORY_RESET);
  addFormHeaderEnd(webContentBuffer);
  addButton(webContentBuffer, S_RETURN, "");

  WebServer->sendContent();
}