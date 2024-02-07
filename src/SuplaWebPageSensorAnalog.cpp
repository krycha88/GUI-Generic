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

#include "SuplaWebPageSensorAnalog.h"

#ifdef GUI_SENSOR_ANALOG
void createWebPageSensorAnalog() {
  WebServer->httpServer->on(getURL(PATH_ANALOG), [&]() {
    if (!WebServer->isLoggedIn()) {
      return;
    }

#ifdef SUPLA_MPX_5XXX
    if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_MPX_5XX_EMPTY) {
      Supla::GUI::mpx->calibrateEmptyThank();
      handleSensorAnalog(1);
      return;
    }
    else if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_MPX_5XX_FULL) {
      Supla::GUI::mpx->calibrateFullThank();
      handleSensorAnalog(1);
      return;
    }
#endif

#if defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
    if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_ANALOG_READING_MAP_MIN) {
      int nr = WebServer->httpServer->arg(URL_ARG_NR).toInt();
      Supla::GUI::analog[nr]->calibrateMinValue();
      handleSensorAnalog(1);
      return;
    }
    else if (WebServer->httpServer->arg(ARG_PARM_URL) == PATH_ANALOG_READING_MAP_MAX) {
      int nr = WebServer->httpServer->arg(URL_ARG_NR).toInt();
      Supla::GUI::analog[nr]->calibrateMaxValue();
      handleSensorAnalog(1);
      return;
    }
#endif

    if (WebServer->httpServer->method() == HTTP_GET)
      handleSensorAnalog();
    else
      handleSensorAnalogSave();
  });
}

void handleSensorAnalog(int save) {
  WebServer->sendHeaderStart();

  SuplaSaveResult(save);
  SuplaJavaScript(PATH_ANALOG);

  addForm(F("post"), PATH_ANALOG);

#ifdef SUPLA_NTC_10K
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_NTC_10K);
  addListGPIOBox(INPUT_NTC_10K, F("ADC Pin"), FUNCTION_NTC_10K);
  addFormHeaderEnd();
#endif

#ifdef SUPLA_MPX_5XXX
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + S_MPX_5XXX);
  addListGPIOBox(INPUT_MPX_5XXX, F("ADC Pin"), FUNCTION_MPX_5XXX);
  if (ConfigESP->getGpio(FUNCTION_MPX_5XXX) != OFF_GPIO) {
    int16_t thankHeight = Supla::GUI::mpx->getThankHeight();
    addNumberBox(INPUT_THANK_HEIGHT, String(F("Głębokość zbiornika")) + F("[cm]"), F("cm"), false, String(thankHeight));
    int16_t thankEmpty = Supla::GUI::mpx->getEmptyValue();
    addNumberBox(INPUT_THANK_EMPTY, F("Pusty zbiornik"), F("wartość kalibracji"), false, String(thankEmpty));
    int16_t thankFull = Supla::GUI::mpx->getFullValue();
    addNumberBox(INPUT_THANK_FULL, F("Pełny zbiornik"), F("wartość kalibracji"), false, String(thankFull));
    addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla pustego zbiornika"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_MPX_5XX_EMPTY);
    addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla pełnego zbiornika"),
               getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + PATH_MPX_5XX_FULL);
  }
  addFormHeaderEnd();
#endif

#if defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
  String input;
#ifdef ARDUINO_ARCH_ESP8266
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + "pomiaru Analog");
  addListGPIOBox(INPUT_ANALOG_READING_MAP, F("ADC Pin"), FUNCTION_ANALOG_READING);

  if (ConfigESP->getGpio(FUNCTION_ANALOG_READING) != OFF_GPIO) {
    addNumberBox(INPUT_MAX_ANALOG_READING, S_QUANTITY, KEY_MAX_ANALOG_READING, ConfigESP->countFreeGpio(FUNCTION_ANALOG_READING));

    for (int nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
      float value = Supla::GUI::analog[nr]->getMinValue();
      input = INPUT_ANALOG_READING_MAP_MIN;
      input += nr;
      addNumberBox(input, F("MIN IN"), F("wartość kalibracji min"), false, String(value));

      value = Supla::GUI::analog[nr]->getMaxValue();
      input = INPUT_ANALOG_READING_MAP_MAX;
      input += nr;
      addNumberBox(input, F("MAX IN"), F("wartość kalibracji max"), false, String(value));

      value = Supla::GUI::analog[nr]->getMinDesiredValue();
      input = INPUT_ANALOG_READING_MAP_MIN_DESIRED;
      input += nr;
      addNumberBox(input, F("MIN OUT"), F("wartość porządana min"), false, String(value));

      value = Supla::GUI::analog[nr]->getMaxDesiredValue();
      input = INPUT_ANALOG_READING_MAP_MAX_DESIRED;
      input += nr;
      addNumberBox(input, F("MAX OUT"), F("wartość porządana max"), false, String(value));

      addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla MIN IN"),
                 getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + String(PATH_ANALOG_READING_MAP_MIN) + "&" + URL_ARG_NR + "=" + nr);
      addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla MAX IN"),
                 getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + String(PATH_ANALOG_READING_MAP_MAX) + "&" + URL_ARG_NR + "=" + nr);
    }
  }
  addFormHeaderEnd();
#endif

#ifdef ARDUINO_ARCH_ESP32
  addFormHeader(String(S_GPIO_SETTINGS_FOR) + S_SPACE + "pomiaru Analog");

  addNumberBox(INPUT_MAX_ANALOG_READING, S_QUANTITY, KEY_MAX_ANALOG_READING, ConfigESP->countFreeGpio(FUNCTION_ANALOG_READING));

  for (int nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
    addListGPIOBox(INPUT_ANALOG_READING_MAP, F("ADC Pin"), FUNCTION_ANALOG_READING, nr);

    if (ConfigESP->getGpio(nr, FUNCTION_ANALOG_READING) != OFF_GPIO) {
      float value = Supla::GUI::analog[nr]->getMinValue();
      input = INPUT_ANALOG_READING_MAP_MIN;
      input += nr;
      addNumberBox(input, F("MIN IN"), F("wartość kalibracji min"), false, String(value));

      value = Supla::GUI::analog[nr]->getMaxValue();
      input = INPUT_ANALOG_READING_MAP_MAX;
      input += nr;
      addNumberBox(input, F("MAX IN"), F("wartość kalibracji max"), false, String(value));

      value = Supla::GUI::analog[nr]->getMinDesiredValue();
      input = INPUT_ANALOG_READING_MAP_MIN_DESIRED;
      input += nr;
      addNumberBox(input, F("MIN OUT"), F("wartość porządana min"), false, String(value));

      value = Supla::GUI::analog[nr]->getMaxDesiredValue();
      input = INPUT_ANALOG_READING_MAP_MAX_DESIRED;
      input += nr;
      addNumberBox(input, F("MAX OUT"), F("wartość porządana max"), false, String(value));

      addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla MIN IN"),
                 getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + String(PATH_ANALOG_READING_MAP_MIN) + "&" + URL_ARG_NR + "=" + nr);
      addLinkBox(String(S_CALIBRATION) + S_SPACE + F("dla MAX IN"),
                 getParameterRequest(PATH_ANALOG, ARG_PARM_URL) + String(PATH_ANALOG_READING_MAP_MAX) + "&" + URL_ARG_NR + "=" + nr);
    }
  }
  addFormHeaderEnd();
#endif
#endif

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_DEVICE_SETTINGS);
  WebServer->sendHeaderEnd();
}

void handleSensorAnalogSave() {
#ifdef SUPLA_NTC_10K
  if (!WebServer->saveGPIO(INPUT_NTC_10K, FUNCTION_NTC_10K)) {
    handleSensorAnalog(6);
    return;
  }
#endif

#ifdef SUPLA_MPX_5XXX
  if (!WebServer->saveGPIO(INPUT_MPX_5XXX, FUNCTION_MPX_5XXX)) {
    handleSensorAnalog(6);
    return;
  }
  else {
    if (ConfigESP->getGpio(FUNCTION_MPX_5XXX) != OFF_GPIO) {
      if (Supla::GUI::mpx == NULL) {
        Supla::GUI::mpx = new Supla::Sensor::MPX_5XXX(A0);
      }

      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_HEIGHT).c_str(), "") != 0) {
        Supla::GUI::mpx->setThankHeight(WebServer->httpServer->arg(INPUT_THANK_HEIGHT).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_EMPTY).c_str(), "") != 0) {
        Supla::GUI::mpx->setEmptyValue(WebServer->httpServer->arg(INPUT_THANK_EMPTY).toInt());
      }
      if (strcmp(WebServer->httpServer->arg(INPUT_THANK_FULL).c_str(), "") != 0) {
        Supla::GUI::mpx->setFullValue(WebServer->httpServer->arg(INPUT_THANK_FULL).toInt());
      }
    }
  }

#endif

#if defined(SUPLA_ANALOG_READING_MAP) || defined(SUPLA_ANALOG_READING_KPOP)
  String input;
#ifdef ARDUINO_ARCH_ESP8266
  if (!WebServer->saveGPIO(INPUT_ANALOG_READING_MAP, FUNCTION_ANALOG_READING)) {
    handleSensorAnalog(6);
    return;
  }
  else {
    if (ConfigESP->getGpio(FUNCTION_ANALOG_READING) != OFF_GPIO) {
      for (int nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
        if (Supla::GUI::analog == NULL) {
          Supla::GUI::analog[nr] = new Supla::Sensor::AnalogRedingMap(ConfigESP->getGpio(FUNCTION_ANALOG_READING));
        }

        input = INPUT_ANALOG_READING_MAP_MIN;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMinValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MAX;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMaxValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MIN_DESIRED;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMinDesiredValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MAX_DESIRED;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMaxDesiredValue(WebServer->httpServer->arg(input).toFloat());
        }
      }
    }
  }
#endif
#ifdef ARDUINO_ARCH_ESP32
  for (int nr = 0; nr < ConfigManager->get(KEY_MAX_ANALOG_READING)->getValueInt(); nr++) {
    if (!WebServer->saveGPIO(INPUT_ANALOG_READING_MAP, FUNCTION_ANALOG_READING, nr)) {
      handleSensorAnalog(6);
      return;
    }
    else {
      if (ConfigESP->getGpio(nr, FUNCTION_ANALOG_READING) != OFF_GPIO) {
        if (Supla::GUI::analog == NULL) {
#ifdef SUPLA_ANALOG_READING_MAP
          Supla::GUI::analog[nr] = new Supla::Sensor::AnalogRedingMap(ConfigESP->getGpio(nr, FUNCTION_ANALOG_READING));
#elif SUPLA_ANALOG_READING_KPOP
          Supla::GUI::analog[nr] = new Supla::Sensor::AnalogReding(ConfigESP->getGpio(nr, FUNCTION_ANALOG_READING));
#endif
        }

        input = INPUT_ANALOG_READING_MAP_MIN;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMinValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MAX;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMaxValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MIN_DESIRED;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMinDesiredValue(WebServer->httpServer->arg(input).toFloat());
        }
        input = INPUT_ANALOG_READING_MAP_MAX_DESIRED;
        input += nr;
        if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
          Supla::GUI::analog[nr]->setMaxDesiredValue(WebServer->httpServer->arg(input).toFloat());
        }
      }
    }
  }
#endif
  input = INPUT_MAX_ANALOG_READING;
  if (strcmp(WebServer->httpServer->arg(input).c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_ANALOG_READING, WebServer->httpServer->arg(input).c_str());
  }
#endif

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleSensorAnalog(1);
      break;
    case E_CONFIG_FILE_OPEN:
      handleSensorAnalog(2);
      break;
  }
}
#endif