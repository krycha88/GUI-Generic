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

#include "boneIO.h"

#ifdef SUPLA_BONEIO
namespace Supla {

boneIO::boneIO() {
  Wire1.begin(GPIO_NUM_14, GPIO_NUM_15);
  Wire1.setClock(100000);
  // Serial.println("Scanner I2C Wire1");
  // I2CScanner(&Wire1);

  Wire.begin(GPIO_NUM_17, GPIO_NUM_33);
  Wire.setClock(100000);
  // Serial.println("Scanner I2C Wire");
  // I2CScanner(&Wire);

  // Turn off the buzzer connected to GPIO_NUM_2
  pinMode(GPIO_NUM_2, OUTPUT);
  noTone(GPIO_NUM_2);

#ifdef USE_MCP_OUTPUT
  auto *EX_OUTPUT_1 = new Supla::Control::ExpanderMCP23017(&Wire, 0x21);
  auto *EX_OUTPUT_2 = new Supla::Control::ExpanderMCP23017(&Wire, 0x20);
#else
  auto *EX_OUTPUT_1 = new Supla::Control::ExpanderPCF8575(&Wire, 0x21);
  auto *EX_OUTPUT_2 = new Supla::Control::ExpanderPCF8575(&Wire, 0x20);
#endif

  auto *EX_INPUT_1 = new Supla::Control::ExpanderPCF8575(&Wire1, 0x20);
  auto *EX_INPUT_2 = new Supla::Control::ExpanderPCF8575(&Wire1, 0x21);
  auto *EX_INPUT_3 = new Supla::Control::ExpanderPCF8574(&Wire1, 0x22);

#ifdef BONEIO_BOARD_OUTPUT_24
  DevicePin deviceRelay[24] = {{EX_OUTPUT_1, P10}, {EX_OUTPUT_1, P11}, {EX_OUTPUT_1, P12}, {EX_OUTPUT_1, P13}, {EX_OUTPUT_1, P14},
                               {EX_OUTPUT_1, P15}, {EX_OUTPUT_1, P16}, {EX_OUTPUT_1, P17}, {EX_OUTPUT_2, P10}, {EX_OUTPUT_2, P11},
                               {EX_OUTPUT_2, P12}, {EX_OUTPUT_2, P13}, {EX_OUTPUT_1, P07}, {EX_OUTPUT_1, P06}, {EX_OUTPUT_1, P05},
                               {EX_OUTPUT_1, P04}, {EX_OUTPUT_1, P03}, {EX_OUTPUT_1, P02}, {EX_OUTPUT_1, P01}, {EX_OUTPUT_1, P00},
                               {EX_OUTPUT_2, P14}, {EX_OUTPUT_2, P15}, {EX_OUTPUT_2, P16}, {EX_OUTPUT_2, P17}};

  DevicePin deviceButton[24] = {{EX_INPUT_1, P00}, {EX_INPUT_1, P01}, {EX_INPUT_1, P02}, {EX_INPUT_1, P03}, {EX_INPUT_1, P04}, {EX_INPUT_1, P05},
                                {EX_INPUT_1, P06}, {EX_INPUT_1, P10}, {EX_INPUT_1, P11}, {EX_INPUT_1, P12}, {EX_INPUT_1, P13}, {EX_INPUT_1, P14},
                                {EX_INPUT_1, P15}, {EX_INPUT_1, P16}, {EX_INPUT_2, P00}, {EX_INPUT_2, P01}, {EX_INPUT_2, P02}, {EX_INPUT_2, P03},
                                {EX_INPUT_2, P04}, {EX_INPUT_2, P05}, {EX_INPUT_2, P06}, {EX_INPUT_2, P10}, {EX_INPUT_2, P11}, {EX_INPUT_2, P12}};

  DevicePin deviceBinary[11] = {{EX_INPUT_2, P13}, {EX_INPUT_2, P14}, {EX_INPUT_2, P15}, {EX_INPUT_2, P16}, {EX_INPUT_3, P00}, {EX_INPUT_3, P01},
                                {EX_INPUT_3, P02}, {EX_INPUT_3, P03}, {EX_INPUT_3, P04}, {EX_INPUT_3, P05}, {EX_INPUT_3, P06}};
#else
  DevicePin deviceRelay[32] = {{EX_OUTPUT_1, P17}, {EX_OUTPUT_1, P16}, {EX_OUTPUT_1, P15}, {EX_OUTPUT_1, P14}, {EX_OUTPUT_1, P13}, {EX_OUTPUT_1, P12},
                               {EX_OUTPUT_1, P11}, {EX_OUTPUT_1, P10}, {EX_OUTPUT_2, P17}, {EX_OUTPUT_2, P16}, {EX_OUTPUT_2, P15}, {EX_OUTPUT_2, P14},
                               {EX_OUTPUT_2, P13}, {EX_OUTPUT_2, P12}, {EX_OUTPUT_2, P11}, {EX_OUTPUT_2, P10}, {EX_OUTPUT_1, P00}, {EX_OUTPUT_1, P01},
                               {EX_OUTPUT_1, P02}, {EX_OUTPUT_1, P03}, {EX_OUTPUT_1, P04}, {EX_OUTPUT_1, P05}, {EX_OUTPUT_1, P06}, {EX_OUTPUT_1, P07},
                               {EX_OUTPUT_2, P00}, {EX_OUTPUT_2, P01}, {EX_OUTPUT_2, P02}, {EX_OUTPUT_2, P03}, {EX_OUTPUT_2, P04}, {EX_OUTPUT_2, P05},
                               {EX_OUTPUT_2, P06}, {EX_OUTPUT_2, P07}};

  DevicePin deviceButton[32] = {{EX_INPUT_1, P00}, {EX_INPUT_1, P01}, {EX_INPUT_1, P02}, {EX_INPUT_1, P03}, {EX_INPUT_1, P04}, {EX_INPUT_1, P05},
                                {EX_INPUT_1, P06}, {EX_INPUT_1, P10}, {EX_INPUT_1, P11}, {EX_INPUT_1, P12}, {EX_INPUT_1, P13}, {EX_INPUT_1, P14},
                                {EX_INPUT_1, P15}, {EX_INPUT_1, P16}, {EX_INPUT_2, P00}, {EX_INPUT_2, P01}, {EX_INPUT_2, P02}, {EX_INPUT_2, P03},
                                {EX_INPUT_2, P04}, {EX_INPUT_2, P05}, {EX_INPUT_2, P06}, {EX_INPUT_2, P10}, {EX_INPUT_2, P11}, {EX_INPUT_2, P12},
                                {EX_INPUT_2, P13}, {EX_INPUT_2, P14}, {EX_INPUT_2, P15}, {EX_INPUT_2, P16}, {EX_INPUT_3, P00}, {EX_INPUT_3, P01},
                                {EX_INPUT_3, P02}, {EX_INPUT_3, P03}};

  DevicePin deviceBinary[3] = {{EX_INPUT_3, P04}, {EX_INPUT_3, P05}, {EX_INPUT_3, P06}};
#endif

  auto buttonCfg = new Supla::Control::Button(EX_INPUT_3, P07, false, true);
  buttonCfg->disableActionsInConfigMode();
  buttonCfg->configureAsConfigButton(&SuplaDevice);

  auto ssd1306 = new Supla::Display::SSD1306(0x3c, GPIO_NUM_17, GPIO_NUM_33, GEOMETRY_128_64, I2C_ONE, 100000);
  ssd1306->addButton(buttonCfg);

  auto PCT2075 = new Supla::Sensor::LM75(0x48, &Wire);

  size_t deviceRelaySize = sizeof(deviceRelay) / sizeof(deviceRelay[0]);
  size_t deviceBinarySize = sizeof(deviceBinary) / sizeof(deviceBinary[0]);

#ifdef BONEIO_RELAY_CHANNEL
  RelayButtonPair RelayButtonPair[deviceRelaySize];

  for (int i = 0; i < deviceRelaySize; i++) {
    RelayButtonPair[i] = createRelayChannel(deviceRelay[i], deviceButton[i], ConfigESP->getLevel(BONEIO_RELAY_CONFIG));

    PCT2075->addAction(Supla::TURN_OFF, RelayButtonPair[i].relay, OnGreater(80.0));
  }

  for (int i = 0; i < deviceBinarySize; i++) {
    new Supla::Sensor::Binary(deviceBinary[i].io, deviceBinary[i].pin, false, true);
  }

  for (int i = 0; i < deviceRelaySize; i++) {
    auto at = new Supla::Control::ActionTrigger();
    at->setRelatedChannel(RelayButtonPair[i].relay);
    at->attach(RelayButtonPair[i].button);
  }
#endif

#ifdef BONEIO_ROLLER_SHUTTER_CHANNEL
  RollerShutterButtonPair rollerShutterButtonPairs[32];

  for (int i = 0; i < deviceRelaySize / 2; i++) {
    int shutterRelayUpIndex = i * 2;
    int shutterRelayDownIndex = i * 2 + 1;
    int openButtonIndex = i * 2;
    int closeButtonIndex = i * 2 + 1;

    DevicePin shutterRelayUp = deviceRelay[shutterRelayUpIndex];
    DevicePin shutterRelayDown = deviceRelay[shutterRelayDownIndex];
    DevicePin openButton = deviceButton[openButtonIndex];
    DevicePin closeButton = deviceButton[closeButtonIndex];

    rollerShutterButtonPairs[i] = createRollerShutterChannel(shutterRelayUp, shutterRelayDown, openButton, closeButton);
    PCT2075->addAction(Supla::STOP, rollerShutterButtonPairs[i].shutter, OnGreater(80.0));
  }

  for (int i = 0; i < deviceBinarySize; i++) {
    new Supla::Sensor::Binary(deviceBinary->io, deviceBinary->pin, false, true);
  }

  for (int i = 0; i < deviceRelaySize / 2; i++) {
    auto atUP = new Supla::Control::ActionTrigger();
    atUP->setRelatedChannel(rollerShutterButtonPairs[i].shutter);
    atUP->attach(rollerShutterButtonPairs[i].openButton);

    auto atDown = new Supla::Control::ActionTrigger();
    atDown->setRelatedChannel(rollerShutterButtonPairs[i].shutter);
    atDown->attach(rollerShutterButtonPairs[i].closeButton);
  }
#endif
}

RollerShutterButtonPair
boneIO::createRollerShutterChannel(DevicePin shutterRelayUpPin, DevicePin shutterRelayDownPin, DevicePin openButtonPin, DevicePin closeButtonPin) {
  auto shutter = new Supla::Control::RollerShutter(shutterRelayUpPin.io, shutterRelayUpPin.pin, shutterRelayDownPin.pin, false);
  auto openButton = new Supla::Control::Button(openButtonPin.io, openButtonPin.pin, false, true);
  auto closeButton = new Supla::Control::Button(closeButtonPin.io, closeButtonPin.pin, false, true);

  openButton->addAction(Supla::OPEN_OR_STOP, shutter, Supla::ON_PRESS);
  openButton->dontUseOnLoadConfig();
  closeButton->addAction(Supla::CLOSE_OR_STOP, shutter, Supla::ON_PRESS);
  closeButton->dontUseOnLoadConfig();

  return {shutter, openButton, closeButton};
}

RelayButtonPair boneIO::createRelayChannel(DevicePin relayPin, DevicePin buttonPin, bool highIsOn) {
  auto relay = new Supla::Control::Relay(relayPin.io, relayPin.pin, highIsOn);
  relay->getChannel()->setDefault(SUPLA_CHANNELFNC_POWERSWITCH);

  switch (ConfigESP->getMemory(BONEIO_RELAY_CONFIG)) {
    case MEMORY_OFF:
      relay->setDefaultStateOff();
      break;
    case MEMORY_ON:
      relay->setDefaultStateOn();
      break;
    case MEMORY_RESTORE:
      relay->setDefaultStateRestore();
      break;
  }

  auto button = new Supla::Control::Button(buttonPin.io, buttonPin.pin, false, true);
  button->dontUseOnLoadConfig();
  button->addAction(Supla::TOGGLE, relay, Supla::ON_PRESS);
  return {relay, button};
}

};  // namespace Supla

#endif