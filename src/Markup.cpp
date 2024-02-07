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

#include "Markup.h"
#include "SuplaCommonPROGMEM.h"

void addForm(const String& method, const String& action) {
  WebServer->sendContent(F("<form method='") + method + F("' action='") + action + F("'>"));
}

void addFormEnd() {
  WebServer->sendContent(F("</form>"));
}

void addFormHeader(const String& name) {
  WebServer->sendContent(F("<div class='w'>"));
  if (name != "\n") {
    WebServer->sendContent(F("<h3>"));
    WebServer->sendContent(name);
    WebServer->sendContent(F("</h3>"));
  }
}

void addFormHeaderEnd() {
  WebServer->sendContent(F("</div>"));
}

void addBr() {
  WebServer->sendContent(F("<br>"));
}

void addLabel(const String& name) {
  WebServer->sendContent(F("<i><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label></i>"));
}

void addTextBox(const String& input_id,
                const String& name,
                const String& value,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly,
                bool password,
                bool underline) {
  if (underline) {
    WebServer->sendContent(F("<i>"));
  }
  else {
    WebServer->sendContent(F("<i style='border-bottom:none !important;'>"));
  }

  WebServer->sendContent(F("<input name='"));
  WebServer->sendContent(input_id);
  if (password) {
    if (ConfigESP->configModeESP != Supla::DEVICE_MODE_NORMAL) {
      WebServer->sendContent(F("' type='password"));
    }
  }

  WebServer->sendContent(F("' value='"));
  if (value != placeholder) {
    WebServer->sendContent(value);
  }

  if (placeholder != "") {
    WebServer->sendContent(F("' placeholder='"));
    WebServer->sendContent(placeholder);
  }

  if (minlength > 0) {
    WebServer->sendContent(F("' minlength='"));
    WebServer->sendContent(String(minlength).c_str());
  }
  if (maxlength > 0) {
    WebServer->sendContent(F("' maxlength='"));
    WebServer->sendContent(String(maxlength).c_str());
  }
  WebServer->sendContent(F("'"));
  if (readonly) {
    WebServer->sendContent(F(" readonly"));
  }

  if (required) {
    WebServer->sendContent(F(" required"));
  }

  WebServer->sendContent(F("><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label></i> "));
}

void addTextBox(const String& input_id,
                const String& name,
                uint8_t value_key,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly,
                bool password) {
  String value = String(ConfigManager->get(value_key)->getValue());
  return addTextBox(input_id, name, value, placeholder, minlength, maxlength, required, readonly, password);
}

void addTextBox(const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required, bool readonly) {
  return addTextBox(input_id, name, value_key, "", minlength, maxlength, required, readonly, false);
}

void addTextBox(const String& input_id, const String& name, const String& value, int minlength, int maxlength, bool required, bool readonly) {
  return addTextBox(input_id, name, value, "", minlength, maxlength, required, readonly, false);
}

void addTextBox(const String& value) {
  WebServer->sendContent(F("<style><input[name='board']{padding-left: 48px;width: calc(100% - 52px);}</style>"));
  WebServer->sendContent(F("<p style='color:#000;'>"));
  WebServer->sendContent(value);
  WebServer->sendContent(F("</p>"));
}
void addTextBoxPassword(const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required) {
  return addTextBox(input_id, name, value_key, "", minlength, maxlength, required, false, true);
}

void addCheckBox(const String& input_id, const String& name, bool checked) {
  WebServer->sendContent(F("<i><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><input type='checkbox' name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("'"));
  if (checked) {
    WebServer->sendContent(F(" checked"));
  }
  WebServer->sendContent(F("></i>"));
}

void addNumberBox(const String& input_id, const String& name, uint8_t value_key, int max) {
  addNumberBox(input_id, name, String(ConfigManager->get(value_key)->getValue()).c_str(), max);
}

void addNumberBox(const String& input_id, const String& name, const String& value, int max) {
  WebServer->sendContent(F("<i><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><input name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("' type='number' placeholder='0' step='1' min='0'"));

  if (max >= 0) {
    WebServer->sendContent(F(" max='"));
    WebServer->sendContent(String(max));
    WebServer->sendContent(F("'"));
  }

  WebServer->sendContent(F(" value='"));
  WebServer->sendContent(value);
  WebServer->sendContent(F("'></i>"));
}

void addNumberBox(const String& input_id, const String& name, const String& placeholder, bool required, const String& value, bool underline) {
  if (underline) {
    WebServer->sendContent(F("<i>"));
  }
  else {
    WebServer->sendContent(F("<i style='border-bottom:none !important;'>"));
  }
  WebServer->sendContent(F("<label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><input name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("' type='number'"));
  if (!placeholder.isEmpty()) {
    WebServer->sendContent(F(" placeholder='"));
    WebServer->sendContent(placeholder.c_str());
    WebServer->sendContent(F("'"));
  }
  WebServer->sendContent(F(" step='0.01' value='"));
  WebServer->sendContent(value.c_str());
  WebServer->sendContent(F("'"));

  if (required) {
    WebServer->sendContent(F(" required"));
  }

  WebServer->sendContent(F("></i>"));
}

void addLinkBox(const String& name, const String& url) {
  WebServer->sendContent(F("<i>"));
  WebServer->sendContent(F("<label>"));
  WebServer->sendContent(F("<a href='"));
  WebServer->sendContent(PATH_START);
  WebServer->sendContent(url);
  WebServer->sendContent(F("'>"));
  WebServer->sendContent(name);
  // WebServer->sendContent(F(FPSTR(ICON_EDIT)));
  WebServer->sendContent(F("</a>"));
  WebServer->sendContent(F("</label>"));
  WebServer->sendContent(F("</i>"));
}

void addHyperlink(const String& name, const String& url) {
  WebServer->sendContent(F("<i>"));
  WebServer->sendContent(F("<label>"));
  WebServer->sendContent(F("<a href='"));
  WebServer->sendContent(url);
  WebServer->sendContent(F("' target='_self'>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</a>"));
  WebServer->sendContent(F("</label>"));
  WebServer->sendContent(F("</i>"));
}

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function) {
  addListGPIOBox(input_id, name, function, 0, true, url, true);
}

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr, bool no_number) {
  addListGPIOBox(input_id, name, function, nr, true, url, no_number);
}

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr) {
  addListGPIOBox(input_id, name, function, nr, true, url);
}

void addListGPIOBox(const String& input_id, const String& name, uint8_t function) {
  addListGPIOBox(input_id, name, function, 0, true, "", true);
}

void addListGPIOBox(const String& input_id, const String& name, uint8_t function, uint8_t nr, bool underline, const String& url, bool no_number) {
  uint8_t gpio;

  gpio = ConfigESP->getGpio(nr, function);

  if (underline) {
    WebServer->sendContent(F("<i>"));
  }
  else {
    WebServer->sendContent(F("<i style='border-bottom:none !important;'>"));
  }
  WebServer->sendContent(F("<label>"));

  if (!url.isEmpty() && gpio != OFF_GPIO) {
    WebServer->sendContent(F("<a href='"));
    WebServer->sendContent(PATH_START);
    WebServer->sendContent(url);
    WebServer->sendContent(String(nr).c_str());
    WebServer->sendContent(F("'>"));

    if (!no_number) {
      WebServer->sendContent(String(nr + 1).c_str());
      WebServer->sendContent(F("."));
    }

    WebServer->sendContent(F(" "));
    WebServer->sendContent(name);
    WebServer->sendContent(ICON_EDIT);
    WebServer->sendContent(F("</a>"));
  }
  else {
    if (!no_number) {
      WebServer->sendContent(String(nr + 1).c_str());
      WebServer->sendContent(F("."));
    }

    WebServer->sendContent(F(" "));
    WebServer->sendContent(name);
  }

  WebServer->sendContent(F("</label>"));

  WebServer->sendContent(F("<select name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(String(nr).c_str());
  WebServer->sendContent(F("'>"));

  if (function == FUNCTION_RELAY && nr < MAX_VIRTUAL_RELAY)
    addGPIOOptionValue(GPIO_VIRTUAL_RELAY, gpio, String(S_SPACE) + "VIRTUAL");

#ifdef ARDUINO_ARCH_ESP8266
  for (uint8_t suported = 0; suported <= OFF_GPIO; suported++)
    if (ConfigESP->checkBusyGpio(suported, function) || suported == gpio)
      addGPIOOptionValue(suported, gpio, FPSTR(GPIO_P[suported]));

#elif ARDUINO_ARCH_ESP32

  for (uint8_t suported = 0; suported <= OFF_GPIO; suported++)
    if ((ConfigESP->checkBusyGpio(suported, function) || suported == gpio))
      addGPIOOptionValue(suported, gpio, FPSTR(GPIO_P[suported]));

#endif

  WebServer->sendContent(F("</select>"));

  WebServer->sendContent(F("</i>"));
}

void addGPIOOptionValue(uint8_t gpio, uint8_t selectedGpio, const String& name) {
  WebServer->sendContent(F("<option value='"));
  WebServer->sendContent(String(gpio).c_str());
  WebServer->sendContent(F("'"));

  if (gpio == selectedGpio) {
    WebServer->sendContent(F(" selected"));
  }
  if (gpio == OFF_GPIO) {
    WebServer->sendContent(F(">"));
  }
  else {
    WebServer->sendContent(F("> GPIO"));
  }
  WebServer->sendContent(name);
}

#ifdef GUI_SENSOR_I2C_EXPENDER

void addListExpanderBox(const String& input_id, const String& name, uint8_t function, uint8_t nr, const String& url) {
  uint8_t type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (nr == 0) {
    addListBox(INPUT_EXPENDER_TYPE, S_TYPE, EXPENDER_LIST_P, EXPENDER_COUNT, type);
  }

  if (Expander->checkActiveExpander(function)) {
    if (nr < MAX_EXPANDER_FOR_FUNCTION) {
      addListExpanderGPIOBox(input_id, name, function, nr, url);
    }
    else {
      addListGPIOLinkBox(input_id, name, getParameterRequest(url, ARG_PARM_NUMBER), function, nr);
    }
  }
  else {
    addListGPIOLinkBox(input_id, name, getParameterRequest(url, ARG_PARM_NUMBER), function, nr);
  }
}

void addListExpanderGPIOBox(const String& input_id, const String& name, uint8_t function, uint8_t nr, const String& url) {
  uint8_t address, type, maxNr;
  const char* const* listAdressExpender;
  const char* const* listExpender;

  type = ConfigManager->get(KEY_ACTIVE_EXPENDER)->getElement(function).toInt();

  if (type == EXPENDER_PCF8574 || type == EXPENDER_PCF8574_I2C2) {
    maxNr = 8;
    listAdressExpender = EXPENDER_PCF8574_P;
    listExpender = GPIO_PCF_8574_P;
  }
  else if (type == EXPENDER_PCF8575 || type == EXPENDER_PCF8575_I2C2) {
    maxNr = 16;
    listAdressExpender = EXPENDER_P;
    listExpender = GPIO_PCF_PCF8575_P;
  }
  else {
    maxNr = 16;
    listAdressExpender = EXPENDER_P;
    listExpender = GPIO_MCP23017_P;
  }

  if (nr == 0 || nr == maxNr) {
    for (uint8_t gpio = nr; gpio <= OFF_GPIO_EXPENDER; gpio++) {
      address = Expander->getAdressExpander(gpio, function);
      if (address != OFF_ADDRESS_MCP23017) {
        break;
      }
    }

    if (url != "")
      addListLinkBox(String(INPUT_ADRESS_MCP23017) + nr, String(S_ADDRESS) + S_SPACE + 1, listAdressExpender, 5, address, url);
    else
      addListBox(String(INPUT_ADRESS_MCP23017) + nr, String(S_ADDRESS) + S_SPACE + 1, listAdressExpender, 5, address);
  }

  addListExpanderGPIO(input_id, name, function, nr, listExpender, 18, getParameterRequest(url, ARG_PARM_NUMBER) + nr);
}

void addListExpanderGPIO(
    const String& input_id, const String& name, uint8_t function, uint8_t nr, const char* const* array_P, uint8_t size, const String& url) {
  WebServer->sendContent(F("<i><label><a href='"));
  WebServer->sendContent(PATH_START);
  WebServer->sendContent(url);
  WebServer->sendContent(F("'>"));
  WebServer->sendContent(nr + 1);
  WebServer->sendContent(F(". "));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</a>"));
  WebServer->sendContent(F("</label><select name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("mcp"));
  WebServer->sendContent(nr);
  WebServer->sendContent(F("'>"));

  uint8_t selected = Expander->getGpioExpander(nr, function);

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      if (Expander->checkBusyGpioExpander(suported, nr, function) || selected == suported) {
        WebServer->sendContent(F("<option value='"));
        WebServer->sendContent(suported);
        WebServer->sendContent(F("'"));
        if (selected == suported) {
          WebServer->sendContent(F(" selected"));
        }
        WebServer->sendContent(F(">"));
        WebServer->sendContent_P(array_P[suported]);
      }
    }
  }

  WebServer->sendContent(F("</select></i>"));
}
#endif

void addListBox(const String& input_id, const String& name, const char* const* array_P, uint8_t size, uint8_t selected, uint8_t nr, bool underline) {
  if (underline) {
    WebServer->sendContent(F("<i>"));
  }
  else {
    WebServer->sendContent(F("<i style='border-bottom:none !important;'>"));
  }

  WebServer->sendContent(F("<label>"));
  if (nr != 0) {
    WebServer->sendContent(String(nr).c_str());
    WebServer->sendContent(F(". "));
  }
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><select name='"));
  WebServer->sendContent(input_id);
  if (nr != 0) {
    WebServer->sendContent(String(nr).c_str());
  }
  WebServer->sendContent(F("'>"));

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      WebServer->sendContent(F("<option value='"));
      WebServer->sendContent(String(suported).c_str());
      WebServer->sendContent(F("'"));
      if (selected == suported) {
        WebServer->sendContent(F(" selected"));
      }
      WebServer->sendContent(F(">"));
      WebServer->sendContent(array_P[suported]);
    }
  }

  WebServer->sendContent(F("</select></i>"));
}

void addListNumbersBox(const String& input_id, const String& name, uint8_t size, uint8_t selected) {
  WebServer->sendContent(F("<i><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><select name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("'>"));

  for (uint8_t suported = 0; suported < size; suported++) {
    WebServer->sendContent(F("<option value='"));
    WebServer->sendContent(String(suported).c_str());
    WebServer->sendContent(F("'"));
    if (selected == suported) {
      WebServer->sendContent(F(" selected"));
    }
    WebServer->sendContent(F(">"));
    WebServer->sendContent(String(suported + 1).c_str());
  }

  WebServer->sendContent(F("</select></i>"));
}

void addListNumbersSensorBox(const String& input_id, const String& name, uint8_t selected) {
  WebServer->sendContent(F("<i><label>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</label><select name='"));
  WebServer->sendContent(input_id);
  WebServer->sendContent(F("'>"));

  WebServer->sendContent(F("<option value='0'"));
  if (selected == 0) {
    WebServer->sendContent(F(" selected"));
  }
  WebServer->sendContent(F(">"));
  WebServer->sendContent(S_ABSENT);
  WebServer->sendContent(F("</option>"));

  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();
      uint8_t channelNumber = channel->getChannelNumber();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER || channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        WebServer->sendContent(F("<option value='"));
        WebServer->sendContent(String(channelNumber).c_str());
        WebServer->sendContent(F("'"));
        if (selected == channelNumber) {
          WebServer->sendContent(F(" selected"));
        }
        WebServer->sendContent(F(">"));
        WebServer->sendContent(String(channelNumber).c_str());
        WebServer->sendContent(F(" - "));

        if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
          WebServer->sendContent(channel->getValueDouble());
        }
        else if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
          WebServer->sendContent(String(channel->getValueDoubleFirst()).c_str());
        }
        WebServer->sendContent(S_CELSIUS);
      }
    }
  }
  WebServer->sendContent(F("</select></i>"));
}

void addListLinkBox(
    const String& input_id, const String& name, const char* const* array_P, uint8_t size, uint8_t selected, const String& url, uint8_t nr) {
  WebServer->sendContent(F("<i><label><a href='"));
  WebServer->sendContent(PATH_START);
  WebServer->sendContent(url);
  WebServer->sendContent(F("'>"));

  if (nr != 0) {
    WebServer->sendContent(String(nr).c_str());
    WebServer->sendContent(F(". "));
  }
  WebServer->sendContent(name);
  WebServer->sendContent(F("</a>"));
  WebServer->sendContent(F("</label><select name='"));
  WebServer->sendContent(input_id);
  if (nr != 0) {
    WebServer->sendContent(String(nr).c_str());
  }
  WebServer->sendContent(F("'>"));

  for (uint8_t suported = 0; suported < size; suported++) {
    if (!String(FPSTR(array_P[suported])).isEmpty()) {
      WebServer->sendContent(F("<option value='"));
      WebServer->sendContent(String(suported).c_str());
      WebServer->sendContent(F("'"));
      if (selected == suported) {
        WebServer->sendContent(F(" selected"));
      }
      WebServer->sendContent(F(">"));
      WebServer->sendContent(array_P[suported]);
    }
  }

  WebServer->sendContent(F("</select></i>"));
}

void addButton(const String& name, const String& url) {
  WebServer->sendContent(F("<a href='"));
  WebServer->sendContent(getURL(url));
  WebServer->sendContent(F("'><button>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</button></a>"));
  WebServer->sendContent(F("<br><br>"));
}

void addButtonSubmit(const String& name) {
  WebServer->sendContent(F("<button type='submit'>"));
  WebServer->sendContent(name);
  WebServer->sendContent(F("</button>"));
  WebServer->sendContent(F("<br><br>"));
}

String getURL(const String& url) {
  return String(PATH_START) + url;
}

String getURL(const String& url, uint8_t nr) {
  return String(PATH_START) + url + nr;
}

String getInput(const String& input, uint8_t nr) {
  return input + nr;
}

String getParameterRequest(const String& url, const String& param, const String& value) {
  return url + String(F("?")) + param + String(F("=")) + value;
}

void SuplaJavaScript(const String& java_return) {
  String java_script =
      F("<script type='text/javascript'>setTimeout(function(){var element=document.getElementById('msg');if(element != "
        "null){element.style.visibility='hidden';var url = window.location.pathname + window.location.search; if(url != '/");
  java_script += java_return;
  java_script += F("'){location.href='");
  java_script += java_return;
  java_script += F("'};}},4000);");
  java_script += F("iwindow.top.location != window.location{window.top.location.href = window.location.href;}</script>\n");
  WebServer->sendContent(java_script);
}

// TODO: @krycha88 Usunąć z SuplaSaveResult nieużywany status WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING```
void SuplaSaveResult(int save) {
  String saveresult = "";
  if (save == SaveResult::DATA_SAVED_RESTART_MODULE || save == SaveResult::RESTART_MODULE) {
    saveresult += "<meta http-equiv=\"refresh\" content=\"2;url=/\">\n";
  }

  saveresult += F("<div id=\"msg\" class=\"c\">");

  switch (save) {
    case SaveResult::DATA_SAVE:
      saveresult += S_DATA_SAVED;
      break;
    case SaveResult::RESTART_MODULE:
      saveresult += S_RESTART_MODULE;
      break;
    case SaveResult::DATA_ERASED_RESTART_DEVICE:
      saveresult += S_DATA_ERASED_RESTART_DEVICE;  // do usunięcia
      break;
    case SaveResult::WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING:  // do usunięcia
      saveresult += S_WRITE_ERROR_UNABLE_TO_READ_FILE_FS_PARTITION_MISSING;
      break;
    case SaveResult::DATA_SAVED_RESTART_MODULE:
      saveresult += S_DATA_SAVED_RESTART_MODULE;
      break;
    case SaveResult::WRITE_ERROR_BAD_DATA:
      saveresult += S_WRITE_ERROR_BAD_DATA;
      break;
    case SaveResult::DATA_SAVE_MODE_CONFIG:
      saveresult += F("data saved");
      break;
    case SaveResult::UPDATE_SUCCESS:
      saveresult += F("Aktualizacja zakończona.");
      break;
    case SaveResult::UPDATE_ERROR:
      saveresult += F("Błąd aktualizacji.");
      break;
    case SaveResult::UPDATE_WAIT:
      saveresult += F("Zostanie wygenerowana nowa wersja. Spróbuj ponownie za 5min.");
      break;
    case SaveResult::UPDATE_NO_UPDATES:
      saveresult += F("Brak aktualizacji.");
      break;
    case SaveResult::UPDATE_TOO_LESS_SPACE:
      saveresult += F("Wykonaj aktualizację 2 etapową.");
      break;
    case SaveResult::UPDATE_NEW_VERSION:
      saveresult += F("Dostępna nowa wersja.");
      break;
    case SaveResult::UPDATE_2STEP:
      saveresult += F("Aktualizacja 2 etapowa.");
      break;

    default:
      saveresult = emptyString;
      return;
      break;
  }

  saveresult += F("</div>");
  WebServer->sendContent(saveresult);
}