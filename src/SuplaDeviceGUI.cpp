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
#include "SuplaDeviceGUI.h"
#include "SuplaConfigManager.h"

Supla::SPIFFSConfig configSupla;
Supla::Eeprom eeprom(STORAGE_OFFSET);

namespace Supla {
namespace GUI {
void begin() {
  setupConnection();
  enableConnectionSSL(ConfigESP->checkSSL());

#ifdef BUILD_VERSION
  String ver = "GG v" + String(BUILD_VERSION);
  ver.reserve(16);
  SuplaDevice.setSwVersion(ver.c_str());
#endif

#ifdef SUPLA_THERMOSTAT
  SuplaDevice.begin(21);
#else
  SuplaDevice.begin();
#endif

  if (ConfigESP->configModeESP == Supla::DEVICE_MODE_CONFIG)
    Supla::Network::SetConfigMode();

  if (getCountChannels() == 0)
    ConfigESP->configModeInit();
}

void setupConnection() {
#ifdef SUPLA_WT32_ETH01_LAN8720
  if (eth == nullptr) {
    eth = new Supla::GUI_WT32_ETH01(1);  // uint_t ETH_ADDR = I²C-address of Ethernet PHY (0 or 1)
  }
#elif defined(SUPLA_ETH01_LAN8720)
  if (eth == nullptr) {
    eth = new Supla::GUI_ETH01(1);  // uint_t ETH_ADDR = I²C-address of Ethernet PHY (0 or 1)
  }
#else
  if (wifi) {
    wifi->setSsid(ConfigManager->get(KEY_WIFI_SSID)->getValue());
    wifi->setPassword(ConfigManager->get(KEY_WIFI_PASS)->getValue());
    SuplaDevice.enableNetwork();
  }
  else {
    wifi = new Supla::GUIESPWifi(ConfigManager->get(KEY_WIFI_SSID)->getValue(), ConfigManager->get(KEY_WIFI_PASS)->getValue());
  }

  String suplaHostname = ConfigManager->get(KEY_HOST_NAME)->getValue();
  suplaHostname.replace(" ", "_");
  wifi->setHostName(suplaHostname.c_str());

#endif
}

void enableConnectionSSL(bool value) {
#if defined(SUPLA_WT32_ETH01_LAN8720) || defined(SUPLA_ETH01_LAN8720)
  if (eth) {
    if (ConfigESP->configModeESP == Supla::DEVICE_MODE_CONFIG) {
      eth->setSSLEnabled(false);
    }
    else {
      eth->setSSLEnabled(value);
    }
  }

#else
  if (wifi) {
    if (ConfigESP->configModeESP == Supla::DEVICE_MODE_CONFIG) {
      wifi->enableSSL(false);
    }
    else {
      wifi->enableSSL(value);
    }
  }
#endif
}

void crateWebServer() {
  if (WebServer == NULL) {
    WebServer = new SuplaWebServer();
  }
  WebServer->begin();
}

void addRelayOrThermostat(int nr) {
  if (ConfigESP->getGpio(nr, FUNCTION_RELAY) != OFF_GPIO) {
#ifdef SUPLA_RELAY
    if (ConfigManager->get(KEY_THERMOSTAT_TYPE)->getElement(nr).toInt() == Supla::GUI::THERMOSTAT_OFF) {
      Supla::GUI::addRelay(nr);
    }
    else {
#ifdef SUPLA_THERMOSTAT
      thermostatArray[nr] = new Supla::Control::GUI::ThermostatGUI(nr, &SuplaDevice);
      relay.push_back(nullptr);
#endif
    }
#endif
  }
}

#ifdef SUPLA_RELAY
void addRelay(uint8_t nr) {
  uint8_t pinRelay, pinLED;
  bool highIsOn, levelLed;

  pinRelay = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getLevel(pinLED);
  highIsOn = ConfigESP->getLevel(pinRelay);

  Supla::Control::Relay *newRelay = nullptr;

  if (pinRelay != OFF_GPIO) {
    if (pinRelay == GPIO_VIRTUAL_RELAY) {
      newRelay = new Supla::Control::VirtualRelay();
      newRelay->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);
    }
    else if (ConfigESP->getLightRelay(pinRelay)) {
      newRelay = new Supla::Control::LightRelay(pinRelay, highIsOn);
      newRelay = static_cast<Supla::Control::Relay *>(newRelay);
      newRelay->getChannel()->setDefault(SUPLA_CHANNELFNC_LIGHTSWITCH);
    }
    else {
      newRelay = Supla::Control::GUI::Relay(pinRelay, highIsOn, nr);
      newRelay->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);
    }

#ifdef SUPLA_BUTTON
    Supla::GUI::addButtonToRelay(nr, newRelay);
#endif

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_RELAY, S_RELAY, newRelay, nr);
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_RELAY, S_RELAY, newRelay, nr);
#endif

    switch (ConfigESP->getMemory(pinRelay, nr)) {
      case MEMORY_OFF:
        newRelay->setDefaultStateOff();
        break;
      case MEMORY_ON:
        newRelay->setDefaultStateOn();
        break;
      case MEMORY_RESTORE:
        newRelay->setDefaultStateRestore();
        break;
    }

    newRelay->keepTurnOnDuration();

    if (pinLED != OFF_GPIO) {
      new Supla::Control::PinStatusLedGUI(pinRelay, pinLED, !levelLed);
    }
  }

  relay.push_back(newRelay);
}

void addButtonToRelay(uint8_t nrRelay, Supla::Control::Relay *relay) {
  addButtonToRelay(nrRelay, relay, relay, relay);
}

void addButtonToRelay(uint8_t nrRelay, Supla::Element *element, Supla::ActionHandler *client, Supla::Control::Relay *relay) {
  uint8_t pinButton, nrButton, pinRelay, buttonAction, buttonEvent;

  for (uint8_t nr = 0; nr < ConfigManager->get(KEY_MAX_BUTTON)->getValueInt(); nr++) {
    nrButton = ConfigESP->getNumberButton(nr);
    pinButton = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
    pinRelay = ConfigESP->getGpio(nrButton, FUNCTION_RELAY);
    buttonAction = ConfigESP->getActionInternal(pinButton);
    buttonEvent = ConfigESP->getEvent(pinButton);

    if (pinButton != OFF_GPIO && pinRelay != OFF_GPIO && nrRelay == nrButton) {
      Supla::Control::Button *button = nullptr;

#ifdef ARDUINO_ARCH_ESP8266
      if (pinButton == A0) {
        button = new Supla::Control::ButtonAnalog(A0, ConfigManager->get(KEY_ANALOG_INPUT_EXPECTED)->getElement(nr).toInt());
      }
#endif
      if (button == nullptr) {
        button = Supla::Control::GUI::Button(pinButton, ConfigESP->getPullUp(pinButton), ConfigESP->getInversed(pinButton), nrButton);
      }

      int muliclickTimeMs = ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValueFloat() * 1000;
      int holdTimeMs = ConfigManager->get(KEY_AT_HOLD_TIME)->getValueFloat() * 1000;

      switch (buttonEvent) {
          // case Supla::Event::ON_PRESS:
          //   button->setButtonType(Supla::Control::Button::ButtonType::MONOSTABLE);
          //   relay[nrButton]->attach(button);
          //   break;

          // case Supla::Event::ON_RELEASE:
          //   button->addAction(buttonAction, relay[nrButton], buttonEvent);
          //   break;

          // case Supla::Event::ON_CHANGE:
          //   button->setButtonType(Supla::Control::Button::ButtonType::BISTABLE);
          //   relay[nrButton]->attach(button);
          //   break;
        case Supla::GUI::Event::ON_MOTION_SENSOR:
          button->setButtonType(Supla::Control::Button::ButtonType::MOTION_SENSOR);
          if (relay) {
            relay->attach(button);
          }
          break;

        case Supla::GUI::Event::ON_HOLD:
          button->addAction(buttonAction, client, Supla::Event::ON_HOLD);
          break;

        default:
          if (ConfigESP->getAction(pinButton) == Supla::GUI::Action::AUTOMATIC_STAIRCASE) {
            if (buttonEvent == Supla::GUI::Event::ON_CHANGE) {
              button->setMulticlickTime(ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValueFloat() * 1000, true);

              button->addAction(Supla::Action::TOGGLE, client, Supla::Event::ON_CLICK_1);
              button->addAction(Supla::Action::TURN_ON_WITHOUT_TIMER, client, Supla::Event::ON_CLICK_2);
            }
            else {
              button->addAction(Supla::Action::TOGGLE, client, Supla::Event::ON_CLICK_1);
              button->addAction(Supla::Action::TURN_ON_WITHOUT_TIMER, client, Supla::Event::ON_HOLD);
            }
          }
          else if (ConfigESP->getAction(pinButton) == Supla::GUI::Action::DECREASE_TEMPERATURE ||
                   ConfigESP->getAction(pinButton) == Supla::GUI::Action::INCREASE_TEMPERATURE) {
            button->addAction(buttonAction, client, Supla::Event::ON_HOLD);
            button->addAction(buttonAction, client, Supla::Event::ON_CLICK_1);
            button->repeatOnHoldEvery(250);
          }
          else if (ConfigESP->getAction(pinButton) == Supla::GUI::Action::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES_HOLD_OFF) {
            button->addAction(Supla::Action::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES, client, Supla::Event::ON_CLICK_1);
            if (ConfigManager->get(KEY_ACTIVE_SENSOR)->getElement(SENSOR_I2C_OLED).toInt()) {
              button->addAction(Supla::GUI::Action::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES_HOLD_OFF, client, Supla::Event::ON_HOLD);
            }
            else {
              button->addAction(Supla::Action::TURN_OFF, client, Supla::Event::ON_HOLD);
            }
            button->repeatOnHoldEvery(250);
          }
          else {
            button->addAction(buttonAction, client, Supla::Event::ON_CLICK_1);
          }
          break;
      }

      button->setMulticlickTime(muliclickTimeMs);
      button->setHoldTime(holdTimeMs);
      button->setSwNoiseFilterDelay(50);

#ifdef SUPLA_ACTION_TRIGGER
      addActionTriggerRelatedChannel(nr, button, ConfigESP->getEvent(pinButton), element, muliclickTimeMs, holdTimeMs);
#endif
    }
    delay(0);
  }
}
#endif

#ifdef SUPLA_ACTION_TRIGGER
ActionTrigger *actionTrigger = nullptr;

void addActionTriggerRelatedChannel(
    uint8_t nr, Supla::Control::Button *button, int eventButton, Supla::Element *element, int muliclickTimeMs, int holdTimeMs) {
  auto at = new Supla::Control::ActionTrigger();

  if (muliclickTimeMs == 0) {
    muliclickTimeMs = ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValueFloat() * 1000;
  }
  if (holdTimeMs == 0) {
    holdTimeMs = ConfigManager->get(KEY_AT_HOLD_TIME)->getValueFloat() * 1000;
  }

  if (eventButton == Supla::ON_CHANGE) {
    button->setMulticlickTime(muliclickTimeMs, true);
  }
  else {
    button->setMulticlickTime(muliclickTimeMs);
    button->setHoldTime(holdTimeMs);
  }
  if (element != NULL) {
    at->setRelatedChannel(element);
  }
  at->attach(button);

  actionTrigger[nr].active = true;
}

void addButtonActionTrigger(uint8_t nr) {
  uint8_t pinButton = ConfigESP->getGpio(nr, FUNCTION_BUTTON);

  if (pinButton != OFF_GPIO && !actionTrigger[nr].active) {
    auto button = Supla::Control::GUI::Button(pinButton, ConfigESP->getPullUp(pinButton), ConfigESP->getInversed(pinButton), nr);
    button->setSwNoiseFilterDelay(50);
    auto at = new Supla::Control::ActionTrigger();

    button->addAction(ConfigESP->getActionInternal(pinButton), at, ConfigESP->getEvent(pinButton));

    int muliclickTimeMs = ConfigManager->get(KEY_AT_MULTICLICK_TIME)->getValueFloat() * 1000;
    int holdTimeMs = ConfigManager->get(KEY_AT_HOLD_TIME)->getValueFloat() * 1000;

    if (ConfigESP->getEvent(pinButton) == Supla::ON_CHANGE) {
      button->setMulticlickTime(muliclickTimeMs, true);
    }
    else {
      button->setMulticlickTime(muliclickTimeMs);
      button->setHoldTime(holdTimeMs);
    }

    at->attach(button);
  }
}

int calculateElementCountActionTrigger() {
  int maxButtonValue = ConfigManager->get(KEY_MAX_BUTTON)->getValueInt();

#ifdef SUPLA_RF_BRIDGE
  return maxButtonValue + MAX_BRIDGE_RF;
#else
  return maxButtonValue;
#endif
}
#endif

#if defined(SUPLA_RF_BRIDGE)
void addRelayBridge(uint8_t nr) {
  uint8_t pinRelay, pinLED, pinTransmitter;
  bool highIsOn, levelLed;

  pinTransmitter = ConfigESP->getGpio(FUNCTION_RF_BRIDGE_TRANSMITTER);

  pinRelay = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getLevel(pinLED);

  if (pinRelay != OFF_GPIO && pinTransmitter != OFF_GPIO) {
    if (pinRelay == GPIO_VIRTUAL_RELAY) {
      auto bridgeVirtualRelay = new Supla::Control::RFBridgeVirtualRelay(pinTransmitter);
      bridgeVirtualRelay->setRepeatProtocol(ConfigManager->get(KEY_RF_BRIDGE_PROTOCOL)->getElement(nr).toInt());
      bridgeVirtualRelay->setPulseLengthint(ConfigManager->get(KEY_RF_BRIDGE_PULSE_LENGTHINT)->getElement(nr).toInt());
      bridgeVirtualRelay->setRepeatTransmit(20);
      bridgeVirtualRelay->setRepeatSending(ConfigManager->get(KEY_RF_BRIDGE_REPEAT)->getElement(nr).toInt());

      bridgeVirtualRelay->setCodeLength(ConfigManager->get(KEY_RF_BRIDGE_LENGTH)->getElement(nr).toInt());
      bridgeVirtualRelay->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
      bridgeVirtualRelay->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

      relay.push_back(bridgeVirtualRelay);
    }
    else {
      highIsOn = ConfigESP->getLevel(pinRelay);
      auto bridgeRelay = new Supla::Control::RFBridgeRelay(pinTransmitter, pinRelay, highIsOn);
      bridgeRelay->setRepeatProtocol(ConfigManager->get(KEY_RF_BRIDGE_PROTOCOL)->getElement(nr).toInt());
      bridgeRelay->setPulseLengthint(ConfigManager->get(KEY_RF_BRIDGE_PULSE_LENGTHINT)->getElement(nr).toInt());
      bridgeRelay->setRepeatTransmit(20);
      bridgeRelay->setRepeatSending(ConfigManager->get(KEY_RF_BRIDGE_REPEAT)->getElement(nr).toInt());

      bridgeRelay->setCodeLength(ConfigManager->get(KEY_RF_BRIDGE_LENGTH)->getElement(nr).toInt());
      bridgeRelay->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
      bridgeRelay->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

      relay.push_back(bridgeRelay);
    }

    int size = relay.size() - 1;

    switch (ConfigESP->getMemory(pinRelay, nr)) {
      case MEMORY_OFF:
        relay[size]->setDefaultStateOff();
        break;
      case MEMORY_ON:
        relay[size]->setDefaultStateOn();
        break;
      case MEMORY_RESTORE:
        relay[size]->setDefaultStateRestore();
        break;
    }

    relay[size]->keepTurnOnDuration();
    relay[size]->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);

    if (pinLED != OFF_GPIO) {
      new Supla::Control::PinStatusLedGUI(pinRelay, pinLED, !levelLed);
    }
  }
  delay(0);
}

void addButtonBridge(uint8_t nr) {
  uint8_t pinButton;
  pinButton = ConfigESP->getGpio(FUNCTION_RF_BRIDGE_RECEIVE);

  if (pinButton != OFF_GPIO) {
    auto receiveBridge = new Supla::Control::RFBridgeReceive(pinButton);

    receiveBridge->setCodeON(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).toInt());
    receiveBridge->setCodeOFF(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).toInt());

    if (strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).c_str(),
               ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).c_str()) != 0 &&
        strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_ON)->getElement(nr).c_str(), "") != 0 &&
        strcmp(ConfigManager->get(KEY_RF_BRIDGE_CODE_OFF)->getElement(nr).c_str(), "") != 0) {
      receiveBridge->addAction(Supla::TURN_ON, relay[nr], Supla::ON_PRESS);
      receiveBridge->addAction(Supla::TURN_OFF, relay[nr], Supla::ON_RELEASE);
#ifdef SUPLA_ACTION_TRIGGER
      addActionTriggerRelatedChannel(nr, receiveBridge, Supla::ON_CHANGE, relay[nr]);
#endif
    }
    else {
      receiveBridge->addAction(Supla::TOGGLE, relay[nr], Supla::ON_PRESS);
      receiveBridge->isMonostable();
#ifdef SUPLA_ACTION_TRIGGER
      addActionTriggerRelatedChannel(nr, receiveBridge, Supla::ON_PRESS, relay[nr]);
#endif
    }
  }
}
#endif

#ifdef SUPLA_PUSHOVER
void addPushover(uint8_t nr) {
  if (strcmp(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr).c_str(), "") != 0 &&
      strcmp(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), "") != 0 && strcmp(ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), "") != 0) {
    auto pushover =
        new Supla::Control::Pushover(ConfigManager->get(KEY_PUSHOVER_TOKEN)->getValue(), ConfigManager->get(KEY_PUSHOVER_USER)->getValue(), true);

    String title = ConfigManager->get(KEY_HOST_NAME)->getValue();

    // if (title.indexOf(" ", title.length() - 1) == -1) {
    //   title = String(ConfigManager->get(KEY_HOST_NAME)->getValue()) + S_SPACE + "-" + S_SPACE + name + S_SPACE + (nr + 1);
    // }

    pushover->setTitle(title.c_str());
    pushover->setMessage(ConfigManager->get(KEY_PUSHOVER_MASSAGE)->getElement(nr).c_str());
    pushover->setSound(ConfigManager->get(KEY_PUSHOVER_SOUND)->getElement(nr).toInt());
#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_PUSHOVER, S_PUSHOVER, pushover, nr);
#endif
  }
}
#endif

#if defined(SUPLA_DIRECT_LINKS)
void addDirectLinks(uint8_t nr) {
  if (nr <= MAX_DIRECT_LINK) {
    if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str(), "") != 0 ||
        strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str(), "") != 0) {
      auto directLink = new Supla::Control::DirectLinks(ConfigManager->get(KEY_SUPLA_SERVER)->getValue());

      if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str(), "") != 0) {
        directLink->setUrlON(ConfigManager->get(KEY_DIRECT_LINKS_ON)->getElement(nr).c_str());
        relay[nr]->addAction(DirectLinks::SEND_DIRECT_LINKS_ON, directLink, Supla::ON_TURN_ON);
      }
      if (strcmp(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str(), "") != 0) {
        directLink->setUrlOFF(ConfigManager->get(KEY_DIRECT_LINKS_OFF)->getElement(nr).c_str());
        relay[nr]->addAction(DirectLinks::SEND_DIRECT_LINKS_OFF, directLink, Supla::ON_TURN_OFF);
      }
    }
  }
}
#endif

#ifdef SUPLA_DS18B20
void addDS18B20MultiThermometer(int pinNumber) {
  uint8_t maxDevices = ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt();

  DS18B20::initSharedResources(pinNumber);
  ThermHygroMeterCorrectionHandler &correctionHandler = ThermHygroMeterCorrectionHandler::getInstance();

  if (maxDevices > 1) {
    if (strcmp(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(0).c_str(), "") == 0) {
      findAndSaveDS18B20Addresses();
    }

    for (int i = 0; i < maxDevices; ++i) {
      auto ds = new DS18B20(HexToBytes(ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i)));
      sensorDS.push_back(ds);
      correctionHandler.addThermHygroMeter(ds);

      supla_log(LOG_DEBUG, "Index %d - address %s", i, ConfigManager->get(KEY_ADDR_DS18B20)->getElement(i).c_str());

#ifdef SUPLA_CONDITIONS
      Supla::GUI::Conditions::addConditionsSensor(SENSOR_DS18B20, S_DS18B20, sensorDS[i], i);
#endif
    }
  }
  else {
    auto ds = new DS18B20(nullptr);

    sensorDS.push_back(ds);
    correctionHandler.addThermHygroMeter(ds);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_DS18B20, S_DS18B20, sensorDS[0]);
#endif
  }
}
#endif

#ifdef SUPLA_CONFIG
void addConfigESP(int pinNumberConfig, int pinLedConfig) {
  ConfigESP->addConfigESP(pinNumberConfig, pinLedConfig);
}
#endif

#ifdef SUPLA_ROLLERSHUTTER
void addRolleShutter(uint8_t nr) {
  int pinRelayUp, pinRelayDown, pinButtonUp, pinButtonDown, pinButtonStop, pullupButtonUp, pullupButtonDown, pullupButtonStop, inversedButtonUp,
      inversedButtonDown, inversedButtonStop, pinLedUp, pinLedDown, actionButtonUp, actionButtonDown = Supla::Action::MOVE_DOWN_OR_STOP,
                                                                                    eventButtonUp, eventButtonStop;
  bool highIsOn, levelLedUp, levelLedDown;

  pinRelayUp = ConfigESP->getGpio(nr, FUNCTION_RELAY);
  relay.push_back(nullptr);
#ifdef SUPLA_THERMOSTAT
#endif
  pinRelayDown = ConfigESP->getGpio(nr + 1, FUNCTION_RELAY);
  relay.push_back(nullptr);
#ifdef SUPLA_THERMOSTAT
#endif

  pinButtonUp = ConfigESP->getGpio(nr, FUNCTION_BUTTON);
  pinButtonDown = ConfigESP->getGpio(nr + 1, FUNCTION_BUTTON);
  pinButtonStop = ConfigESP->getGpio(nr, FUNCTION_BUTTON_STOP);

  pullupButtonUp = ConfigESP->getPullUp(pinButtonUp);
  pullupButtonDown = ConfigESP->getPullUp(pinButtonDown);
  pullupButtonStop = ConfigESP->getPullUp(pinButtonStop);

  inversedButtonUp = ConfigESP->getInversed(pinButtonUp);
  inversedButtonDown = ConfigESP->getInversed(pinButtonDown);
  inversedButtonStop = ConfigESP->getInversed(pinButtonStop);

  actionButtonUp = ConfigESP->getAction(pinButtonUp);

  eventButtonUp = ConfigESP->getEvent(pinButtonUp);
  eventButtonStop = ConfigESP->getEvent(pinButtonStop);

  pinLedUp = ConfigESP->getGpio(nr, FUNCTION_LED);
  pinLedDown = ConfigESP->getGpio(nr + 1, FUNCTION_LED);

  levelLedUp = ConfigESP->getLevel(pinLedUp);
  levelLedDown = ConfigESP->getLevel(pinLedDown);

  highIsOn = ConfigESP->getLevel(pinRelayUp);

  switch (actionButtonUp) {
    case Supla::GUI::ActionRolleShutter::OPEN_OR_CLOSE:
      actionButtonUp = Supla::Action::OPEN_OR_STOP;
      actionButtonDown = Supla::Action::CLOSE_OR_STOP;
      break;
    case Supla::GUI::ActionRolleShutter::MOVE_UP_OR_MOVE_DOWN:
      actionButtonUp = Supla::Action::MOVE_UP_OR_STOP;
      actionButtonDown = Supla::Action::MOVE_DOWN_OR_STOP;
      break;
    case Supla::GUI::ActionRolleShutter::STEP_BY_STEP:
      actionButtonUp = Supla::Action::STEP_BY_STEP;
      break;
  }

  auto rollerShutterRelay = Supla::Control::GUI::RollerShutter(pinRelayUp, pinRelayDown, highIsOn, nr);

#ifdef SUPLA_CONDITIONS
  Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_ROLLER_SHUTTER, S_ROLLERSHUTTERS, rollerShutterRelay, nr);
#endif

  if (pinButtonUp != OFF_GPIO && actionButtonUp == Supla::Action::STEP_BY_STEP) {
    auto rollerShutterButtonOpen = Supla::Control::GUI::Button(pinButtonUp, pullupButtonUp, inversedButtonUp, nr);
#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(nr, rollerShutterButtonOpen, eventButtonUp, rollerShutterRelay);
#endif

    rollerShutterButtonOpen->addAction(actionButtonUp, rollerShutterRelay, eventButtonUp);
  }
  else if (pinButtonUp != OFF_GPIO && pinButtonDown != OFF_GPIO) {
    auto rollerShutterButtonOpen = Supla::Control::GUI::Button(pinButtonUp, pullupButtonUp, inversedButtonUp, nr);
    auto rollerShutterButtonClose = Supla::Control::GUI::Button(pinButtonDown, pullupButtonDown, inversedButtonDown, nr);

#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(nr, rollerShutterButtonOpen, eventButtonUp, rollerShutterRelay);
    addActionTriggerRelatedChannel(nr + 1, rollerShutterButtonClose, eventButtonUp, rollerShutterRelay);
#endif

    if (eventButtonUp == Supla::Event::ON_CHANGE) {
      rollerShutterButtonOpen->addAction(actionButtonUp, rollerShutterRelay, Supla::Event::ON_PRESS);
      if (pinButtonStop == OFF_GPIO)
        rollerShutterButtonOpen->addAction(Supla::Action::STOP, rollerShutterRelay, Supla::Event::ON_RELEASE);

      rollerShutterButtonClose->addAction(actionButtonDown, rollerShutterRelay, Supla::Event::ON_PRESS);
      if (pinButtonStop == OFF_GPIO)
        rollerShutterButtonClose->addAction(Supla::Action::STOP, rollerShutterRelay, Supla::Event::ON_RELEASE);
    }
    else {
      rollerShutterButtonOpen->addAction(actionButtonUp, rollerShutterRelay, eventButtonUp);
      rollerShutterButtonClose->addAction(actionButtonDown, rollerShutterRelay, eventButtonUp);
    }
  }

  if (pinButtonStop != OFF_GPIO) {
    auto RollerShutterButtonStop = Supla::Control::GUI::Button(pinButtonStop, pullupButtonStop, inversedButtonStop, nr);
    RollerShutterButtonStop->addAction(Supla::Action::STOP, rollerShutterRelay, eventButtonStop);
  }

  if (pinLedUp != OFF_GPIO) {
    new Supla::Control::PinStatusLedGUI(pinRelayUp, pinLedUp, !levelLedUp);
  }
  if (pinLedDown != OFF_GPIO) {
    new Supla::Control::PinStatusLedGUI(pinRelayDown, pinLedDown, !levelLedDown);
  }
  delay(0);
}
#endif

#ifdef SUPLA_IMPULSE_COUNTER
std::vector<Supla::Sensor::ImpulseCounter *> impulseCounter;

void addImpulseCounter(uint8_t nr) {
  uint8_t pin, pinLED;
  unsigned int debounceDelay;
  bool lowToHigh, inputPullup, levelLed;

  pin = ConfigESP->getGpio(nr, FUNCTION_IMPULSE_COUNTER);
  lowToHigh = ConfigESP->getLevel(pin);
  inputPullup = ConfigESP->getMemory(pin);
  debounceDelay = ConfigManager->get(KEY_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT)->getValueInt();

  pinLED = ConfigESP->getGpio(nr, FUNCTION_LED);
  levelLed = ConfigESP->getLevel(pinLED);

  impulseCounter.push_back(new Supla::Sensor::ImpulseCounter(pin, lowToHigh, inputPullup, debounceDelay));

  if (pinLED != OFF_GPIO) {
    auto led = new Supla::Control::InternalPinOutput(pinLED, levelLed);
    led->setDurationMs(100);
    impulseCounter[nr]->addAction(Supla::TURN_ON, led, Supla::ON_CHANGE);
  }

  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_RGBW
void addRGBWLeds(uint8_t nr) {
  int redPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_RED);
  int greenPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_GREEN);
  int bluePin = ConfigESP->getGpio(nr, FUNCTION_RGBW_BLUE);
  int brightnessPin = ConfigESP->getGpio(nr, FUNCTION_RGBW_BRIGHTNESS);

#ifdef ARDUINO_ARCH_ESP8266
  // https://forum.supla.org/viewtopic.php?p=116483#p116483
  analogWriteFreq(400);
#endif

  if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO && brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBWLeds(redPin, greenPin, bluePin, brightnessPin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(redPin));
    setRGBWButton(nr, rgbw);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
#endif
  }
  else if (redPin != OFF_GPIO && greenPin != OFF_GPIO && bluePin != OFF_GPIO) {
    auto rgbw = new Supla::Control::RGBLeds(redPin, greenPin, bluePin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(redPin));
    setRGBWButton(nr, rgbw);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
#endif
  }
  else if (brightnessPin != OFF_GPIO) {
    auto rgbw = new Supla::Control::DimmerLeds(brightnessPin);
    setRGBWDefaultState(rgbw, ConfigESP->getMemory(brightnessPin));
    setRGBWButton(nr, rgbw);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsExecutive(CONDITIONS::EXECUTIVE_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_RGBW, S_RGBW_RGB_DIMMER, rgbw, nr);
#endif
  }
}

void setRGBWButton(uint8_t nr, Supla::Control::RGBWBase *rgbw) {
  uint8_t nrButton = ConfigESP->getNumberButtonAdditional(BUTTON_RGBW, nr);
  int buttonPin = ConfigESP->getGpio(nrButton, FUNCTION_BUTTON);
  int pullupButton = ConfigESP->getPullUp(buttonPin);
  int inversedButton = ConfigESP->getInversed(buttonPin);

  if (buttonPin != OFF_GPIO) {
    auto button = Supla::Control::GUI::Button(buttonPin, pullupButton, inversedButton, nrButton);
    button->setMulticlickTime(200);
    button->setHoldTime(400);
    button->repeatOnHoldEvery(35);

    rgbw->setStep(1);
    // rgbw->setMinMaxIterationDelay(750);  // delay between dimming direction
    // change, 750 ms (default)
    // rgbw->setMinIterationBrightness(1);  // 1 is default value

    button->addAction(Supla::ITERATE_DIM_ALL, rgbw, Supla::ON_HOLD);
    button->addAction(Supla::TOGGLE, rgbw, Supla::ON_CLICK_1);

#ifdef SUPLA_ACTION_TRIGGER
    addActionTriggerRelatedChannel(nr, button, ConfigESP->getEvent(buttonPin), rgbw);
#endif
  }
}

void setRGBWDefaultState(Supla::Control::RGBWBase *rgbw, uint8_t memory) {
  switch (memory) {
    case MEMORY_OFF:
      rgbw->setDefaultStateOff();
      break;
    case MEMORY_ON:
      rgbw->setDefaultStateOn();
      break;
    case MEMORY_RESTORE:
      rgbw->setDefaultStateRestore();
      break;
  }
}
#endif

#if defined(SUPLA_RELAY) || defined(SUPLA_ROLLERSHUTTER) || defined(SUPLA_PUSHOVER)
std::vector<Supla::Control::Relay *> relay;
#endif

#ifdef SUPLA_DS18B20
std::vector<DS18B20 *> sensorDS;
#endif

#ifdef SUPLA_HLW8012
Supla::Sensor::HLW_8012 *counterHLW8012 = nullptr;

void addHLW8012(int8_t pinCF, int8_t pinCF1, int8_t pinSEL) {
  if (counterHLW8012 == NULL && pinCF != OFF_GPIO && pinCF1 != OFF_GPIO && pinSEL != OFF_GPIO) {
    counterHLW8012 = new Supla::Sensor::HLW_8012(pinCF, pinCF1, pinSEL);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_HLW8012, S_HLW8012, counterHLW8012);
#endif
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_CSE7766
Supla::Sensor::CSE_7766 *counterCSE7766 = nullptr;

void addCSE7766(int8_t pinRX) {
  if (counterCSE7766 == NULL && pinRX != OFF_GPIO) {
    counterCSE7766 = new Supla::Sensor::CSE_7766(ConfigESP->getHardwareSerial(pinRX));

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_CSE7766, S_CSE7766, counterCSE7766);
#endif
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_ADE7953
Supla::Sensor::ADE7953 *couterADE7953;

void addADE7953(int8_t pinIRQ) {
  if (couterADE7953 == NULL && pinIRQ != OFF_GPIO) {
    couterADE7953 = new Supla::Sensor::ADE7953(pinIRQ);

#ifdef SUPLA_CONDITIONS
    Supla::GUI::Conditions::addConditionsSensor(SENSOR_ADE7953, S_ADE7953, couterADE7953);
#endif
  }
  eeprom.setStateSavePeriod(TIME_SAVE_PERIOD_IMPULSE_COUNTER_SEK * 1000);
}
#endif

#ifdef SUPLA_MPX_5XXX
Supla::Sensor::MPX_5XXX *mpx = nullptr;
#endif

#ifdef SUPLA_ANALOG_READING_MAP
Supla::Sensor::AnalogRedingMap **analog = nullptr;
#endif

#ifdef SUPLA_MODBUS_SDM
Supla::Sensor::SDM630 *smd;
#endif

#ifdef SUPLA_MODBUS_SDM_ONE_PHASE
Supla::Sensor::SDM120 *smd120;
#endif
}  // namespace GUI
}  // namespace Supla

SuplaConfigManager *ConfigManager = nullptr;
SuplaConfigESP *ConfigESP = nullptr;
SuplaWebServer *WebServer = nullptr;

#ifdef GUI_SENSOR_I2C_EXPENDER
Supla::Control::ConfigExpander *Expander;
#endif

#ifdef SUPLA_WT32_ETH01_LAN8720
Supla::WT32_ETH01 *eth = nullptr;
#elif defined(SUPLA_ETH01_LAN8720)
Supla::GUI_ETH01 *eth = nullptr;
#else
Supla::GUIESPWifi *wifi = nullptr;
#endif