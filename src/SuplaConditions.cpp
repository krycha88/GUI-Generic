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

#include "SuplaConditions.h"

#ifdef SUPLA_CONDITIONS

const char *CONDITIONS_SENSOR_LIST[COUNT_SENSOR_LIST] = {};
const char *CONDITIONS_EXECUTIVE_TYPE_LIST[COUNT_EXECUTIVE_LIST] = {};

void createWebConditions() {
  WebServer->httpServer->on(getURL(PATH_CONDITIONS), [&]() {
    if (!WebServer->isLoggedIn())
      return;

    if (WebServer->httpServer->method() == HTTP_GET)
      handleConditions();
    else
      handleConditionsSave();
  });
}

void handleConditions(int save) {
  WebServer->sendHeaderStart();
  SuplaSaveResult(save);
  SuplaJavaScript(PATH_CONDITIONS);

  addForm(F("post"), PATH_CONDITIONS);
  if (COUNT_SENSOR_LIST > 1) {
    addFormHeader(S_CONDITIONING);
    addNumberBox(INPUT_CONDITIONING_MAX, S_QUANTITY, KEY_MAX_CONDITIONS, 10);
    addFormHeaderEnd();

    for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
      addFormHeader(String(S_CONDITION) + S_SPACE + (nr + 1));
      uint8_t selected = ConfigManager->get(KEY_CONDITIONS_CLIENT_TYPE)->getElement(nr).toInt();

      addListBox(String(INPUT_CONDITIONS_TYPE_CLIENT) + nr, "Wykonaj dla", CONDITIONS_EXECUTIVE_TYPE_LIST, COUNT_EXECUTIVE_LIST,
                 selected, 0, false);

      selected = ConfigManager->get(KEY_CONDITIONS_CLIENT_TYPE_NUMBER)->getElement(nr).toInt();
      addListNumbersBox(String(INPUT_CONDITIONS_CLIENT_NUMBER) + nr, "Numer", 20, selected);

      selected = ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt();
      addListBox(String(INPUT_CONDITIONS_SENSOR_TYPE) + nr, "Sensor", CONDITIONS_SENSOR_LIST, COUNT_SENSOR_LIST, selected, 0,
                 false);

      selected = ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt();
      addListNumbersBox(String(INPUT_CONDITIONS_SENSOR_NUMBER) + nr, "Numer", 20, selected);

      selected = ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt();
      addListBox(String(INPUT_CONDITIONS_TYPE) + nr, "Jeżeli wartość", CONDITIONS_TYPE_P, CONDITION_COUNT, selected, 0, false);

      String value = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr);
      addNumberBox(String(INPUT_CONDITIONS_MIN) + nr, S_ON, S_SWITCH_ON_VALUE, false, value, false);
      value = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr);
      addNumberBox(String(INPUT_CONDITIONS_MAX) + nr, S_OFF, S_SWITCH_OFF_VALUE, false, value, false);
      addFormHeaderEnd();
    }
  }

  addButtonSubmit(S_SAVE);
  addFormEnd();
  addButton(S_RETURN, PATH_DEVICE_SETTINGS);

  WebServer->sendHeaderEnd();
}

void handleConditionsSave() {
  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    String input = INPUT_CONDITIONS_TYPE_CLIENT;
    input += nr;
    Serial.println(input);
    ConfigManager->setElement(KEY_CONDITIONS_CLIENT_TYPE, nr, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    input = INPUT_CONDITIONS_CLIENT_NUMBER;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_CLIENT_TYPE_NUMBER, nr, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    input = INPUT_CONDITIONS_SENSOR_TYPE;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_SENSOR_TYPE, nr, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    input = INPUT_CONDITIONS_TYPE;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_TYPE, nr, static_cast<int>(WebServer->httpServer->arg(input).toInt()));
    input = INPUT_CONDITIONS_MIN;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_MIN, nr, WebServer->httpServer->arg(input).c_str());
    input = INPUT_CONDITIONS_MAX;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_MAX, nr, WebServer->httpServer->arg(input).c_str());
    input = INPUT_CONDITIONS_SENSOR_NUMBER;
    input += nr;
    ConfigManager->setElement(KEY_CONDITIONS_SENSOR_NUMBER, nr, WebServer->httpServer->arg(input).c_str());
  }

  ConfigManager->set(KEY_MAX_CONDITIONS, WebServer->httpServer->arg(INPUT_CONDITIONING_MAX).c_str());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      handleConditions(DATA_SAVE);
      break;
    case E_CONFIG_FILE_OPEN:
      handleConditions(RESTART_MODULE);
      break;
  }
}

namespace Supla {
namespace GUI {
namespace Conditions {

ConditionsStruct *conditions = nullptr;

void addConditionsElement() {
  if (conditions == nullptr) {
    conditions = new ConditionsStruct[ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt()];
  }

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    conditions[nr].functionClient = ConfigManager->get(KEY_CONDITIONS_CLIENT_TYPE)->getElement(nr).toInt();
    conditions[nr].nrClient = ConfigManager->get(KEY_CONDITIONS_CLIENT_TYPE_NUMBER)->getElement(nr).toInt();
    conditions[nr].functionSensor = ConfigManager->get(KEY_CONDITIONS_SENSOR_TYPE)->getElement(nr).toInt();
    conditions[nr].nrSensor = ConfigManager->get(KEY_CONDITIONS_SENSOR_NUMBER)->getElement(nr).toInt();

    Serial.print("addConditionsElement:");
    Serial.print("functionClient: ");
    Serial.print(conditions[nr].functionClient);
    Serial.print(", nrClient: ");
    Serial.print(conditions[nr].nrClient);
    Serial.print(", functionSensor: ");
    Serial.print(conditions[nr].functionSensor);
    Serial.print(", nrSensor : ");
    Serial.println(conditions[nr].nrSensor);
  }
}

void addConditionsExecutive(int functionClient, const char *nameExecutive, Supla::ActionHandler *client, uint8_t nrClient) {
  CONDITIONS_EXECUTIVE_TYPE_LIST[functionClient] = nameExecutive;

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    if (conditions[nr].functionClient == functionClient && conditions[nr].nrClient == nrClient) {
      conditions[nr].client = client;

      Serial.print("addConditionsExecutive: ");
      Serial.print("functionClient: ");
      Serial.print(conditions[nr].functionClient);
      Serial.print(", nrClient: ");
      Serial.println(conditions[nr].nrClient);
    }
  }
}

void addConditionsSensor(int functionSensor, const char *nameSensor, Supla::ChannelElement *sensor, uint8_t nrSensor) {
  CONDITIONS_SENSOR_LIST[functionSensor] = nameSensor;

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    if (conditions[nr].functionSensor == functionSensor && conditions[nr].nrSensor == nrSensor) {
      conditions[nr].sensorElement = sensor;

      Serial.print("addConditionsSensor: ");
      Serial.print("functionClient: ");
      Serial.print(conditions[nr].functionClient);
      Serial.print(", nrClient: ");
      Serial.print(conditions[nr].nrClient);
      Serial.print(" functionSensor: ");
      Serial.print(conditions[nr].functionSensor);
      Serial.print(", nrSensor : ");
      Serial.println(conditions[nr].nrSensor);
    }
  }
}

void addConditionsSensor(int functionSensor, const char *nameSensor, Supla::ElementWithChannelActions *sensor, uint8_t nrSensor) {
  CONDITIONS_SENSOR_LIST[functionSensor] = nameSensor;

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    if (conditions[nr].functionSensor == functionSensor && conditions[nr].nrSensor == nrSensor) {
      conditions[nr].sensorElementWithChannelActions = sensor;

      Serial.print("addConditionsSensorElementWithChannelActions: ");
      Serial.print("functionClient: ");
      Serial.print(conditions[nr].functionClient);
      Serial.print(", nrClient: ");
      Serial.print(conditions[nr].nrClient);
      Serial.print(" functionSensor: ");
      Serial.print(conditions[nr].functionSensor);
      Serial.print(", nrSensor : ");
      Serial.println(conditions[nr].nrSensor);
    }
  }
}

void addConditionsSensor(int functionSensor, const char *nameSensor, Supla::Sensor::ElectricityMeter *electricityMete, uint8_t nrSensor) {
  CONDITIONS_SENSOR_LIST[functionSensor] = nameSensor;

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    if (conditions[nr].functionSensor == functionSensor && conditions[nr].nrSensor == nrSensor) {
      conditions[nr].electricityMete = electricityMete;

      Serial.print("addConditionsElectricityMete: ");
      Serial.print("functionClient: ");
      Serial.print(conditions[nr].functionClient);
      Serial.print(", nrClient: ");
      Serial.print(conditions[nr].nrClient);
      Serial.print(" functionSensor: ");
      Serial.print(conditions[nr].functionSensor);
      Serial.print(", nrSensor : ");
      Serial.println(conditions[nr].nrSensor);
    }
  }
}

void addConditions() {
  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_CONDITIONS)->getValueInt(); nr++) {
    if (conditions[nr].client != nullptr) {
      int actionON = Supla::TURN_ON;
      int actionOFF = Supla::TURN_OFF;

      if (ConfigManager->get(KEY_CONDITIONS_CLIENT_TYPE)->getElement(nr).toInt() == CONDITIONS::EXECUTIVE_ROLLER_SHUTTER) {
        actionON = Supla::OPEN;
        actionOFF = Supla::CLOSE;
      }

      addCondition(&conditions[nr], nr, actionON, actionOFF);
    }
  }
}

void addCondition(ConditionsStruct *condition, uint8_t nr, int actionON, int actionOFF) {
  if (condition->sensorElementWithChannelActions != nullptr) {
    addConditionForSensorElementWithChannelActions(condition, nr, actionON, actionOFF);
  }
  else if (condition->sensorElement != nullptr) {
    addConditionForSensor(condition, nr, actionON, actionOFF);
  }
  else if (condition->electricityMete != nullptr) {
    addConditionForElectricityMeter(condition, nr);
  }
}

void addConditionForSensorElementWithChannelActions(ConditionsStruct *sensor, uint8_t nr, int actionON, int actionOFF) {
  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditionForSensorElementWithChannelActions MIN: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();
    sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS:
        sensor->sensorElementWithChannelActions->addAction(actionON, sensor->client, OnLessEq(threshold));
        break;
      case CONDITION_ON_GREATER:
        sensor->sensorElementWithChannelActions->addAction(actionON, sensor->client, OnGreaterEq(threshold));
        break;
      case CONDITION_ON_LESS_HUMIDITY:
        sensor->sensorElementWithChannelActions->addAction(actionON, sensor->client, OnLessEq(threshold, true));
        break;
      case CONDITION_ON_GREATER_HUMIDITY:
        sensor->sensorElementWithChannelActions->addAction(actionON, sensor->client, OnGreaterEq(threshold, true));
        break;
      case CONDITION_GPIO:
        if (threshold == 0) {
          actionON = Supla::TURN_OFF;
        }
        sensor->sensorElementWithChannelActions->addAction(actionON, sensor->client, Supla::ON_TURN_ON);
        break;
    }
  }

  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditionForSensorElementWithChannelActions MAX: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();
    sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS:
        sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnGreaterEq(threshold));
        break;
      case CONDITION_ON_GREATER:
        sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnLessEq(threshold));
        break;
      case CONDITION_ON_LESS_HUMIDITY:
        sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnGreaterEq(threshold, true));
        break;
      case CONDITION_ON_GREATER_HUMIDITY:
        sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, OnLessEq(threshold, true));
        break;
      case CONDITION_GPIO:
        if (threshold == 1) {
          actionOFF = Supla::TURN_ON;
        }
        sensor->sensorElementWithChannelActions->addAction(actionOFF, sensor->client, Supla::ON_TURN_OFF);
        break;
    }
  }
}

void addConditionForSensor(ConditionsStruct *sensor, uint8_t nr, int actionON, int actionOFF) {
  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditions MIN: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();
    sensor->sensorElement->addAction(actionOFF, sensor->client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS:
        sensor->sensorElement->addAction(actionON, sensor->client, OnLessEq(threshold));
        break;
      case CONDITION_ON_GREATER:
        sensor->sensorElement->addAction(actionON, sensor->client, OnGreaterEq(threshold));
        break;
      case CONDITION_ON_LESS_HUMIDITY:
        sensor->sensorElement->addAction(actionON, sensor->client, OnLessEq(threshold, true));
        break;
      case CONDITION_ON_GREATER_HUMIDITY:
        sensor->sensorElement->addAction(actionON, sensor->client, OnGreaterEq(threshold, true));
        break;
      case CONDITION_GPIO:
        if (threshold == 0) {
          actionON = Supla::TURN_OFF;
        }
        sensor->sensorElement->addAction(actionON, sensor->client, Supla::ON_TURN_ON);
        break;
    }
  }

  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditions MAX: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();
    sensor->sensorElement->addAction(actionOFF, sensor->client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS:
        sensor->sensorElement->addAction(actionOFF, sensor->client, OnGreaterEq(threshold));
        break;
      case CONDITION_ON_GREATER:
        sensor->sensorElement->addAction(actionOFF, sensor->client, OnLessEq(threshold));
        break;
      case CONDITION_ON_LESS_HUMIDITY:
        sensor->sensorElement->addAction(actionOFF, sensor->client, OnGreaterEq(threshold, true));
        break;
      case CONDITION_ON_GREATER_HUMIDITY:
        sensor->sensorElement->addAction(actionOFF, sensor->client, OnLessEq(threshold, true));
        break;
      case CONDITION_GPIO:
        if (threshold == 1) {
          actionOFF = Supla::TURN_ON;
        }
        sensor->sensorElement->addAction(actionOFF, sensor->client, Supla::ON_TURN_OFF);
        break;
    }
  }
}

void addConditionForElectricityMeter(ConditionsStruct *meter, uint8_t nr) {
  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditions MIN - ElectricityMeter: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MIN)->getElement(nr).toDouble();
    // conditions[nr].electricityMete->addAction(Supla::TURN_OFF, conditions[nr].client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS_VOLTAGE:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnLessEq(threshold, EmVoltage()));
        break;
      case CONDITION_ON_LESS_CURRENT:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnLessEq(threshold, EmTotalCurrent()));
        break;
      case CONDITION_ON_LESS_POWER_ACTIVE:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnLessEq(threshold, EmTotalPowerActiveW()));
        break;

      case CONDITION_ON_GREATER_VOLTAGE:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnGreaterEq(threshold, EmVoltage()));
        break;
      case CONDITION_ON_GREATER_CURRENT:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnGreaterEq(threshold, EmTotalCurrent()));
        break;
      case CONDITION_ON_GREATER_POWER_ACTIVE:
        meter->electricityMete->addAction(Supla::TURN_ON, meter->client, OnGreaterEq(threshold, EmTotalPowerActiveW()));
        break;
    }
  }

  if (strcmp(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str(), "") != 0) {
    Serial.print("addConditions MAX - ElectricityMeter: ");
    Serial.println(ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).c_str());

    double threshold = ConfigManager->get(KEY_CONDITIONS_MAX)->getElement(nr).toDouble();
    //   meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnInvalid());

    switch (ConfigManager->get(KEY_CONDITIONS_TYPE)->getElement(nr).toInt()) {
      case CONDITION_ON_LESS_VOLTAGE:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnGreaterEq(threshold, EmVoltage()));
        break;
      case CONDITION_ON_LESS_CURRENT:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnGreaterEq(threshold, EmTotalCurrent()));
        break;
      case CONDITION_ON_LESS_POWER_ACTIVE:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnGreaterEq(threshold, EmTotalPowerActiveW()));
        break;

      case CONDITION_ON_GREATER_VOLTAGE:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnLessEq(threshold, EmVoltage()));
        break;
      case CONDITION_ON_GREATER_CURRENT:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnLessEq(threshold, EmTotalCurrent()));
        break;
      case CONDITION_ON_GREATER_POWER_ACTIVE:
        meter->electricityMete->addAction(Supla::TURN_OFF, meter->client, OnLessEq(threshold, EmTotalPowerActiveW()));
        break;
    }
  }
}

}  // namespace Conditions
}  // namespace GUI
}  // namespace Supla
#endif
