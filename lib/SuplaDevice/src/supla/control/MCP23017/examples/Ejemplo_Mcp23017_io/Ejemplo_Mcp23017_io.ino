#define supla_lib_config_h_  // silences unnecessary debug messages "should be disabled by default"
#include <SuplaDevice.h>
#include <supla/control/button.h>
#include <supla/control/relay.h>
#include <supla_mcp23017.h>

//
// ESP8266 based board:
#include <supla/network/esp_wifi.h>
Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
//

void setup() {

  Serial.begin(115200);

  mcp1.init(4, 5); // init(uint8_t sda, uint8_t scl, bool fast)  =  Wire.begin

  if (! mcp1.begin(0))Serial.println("MCP23017 1 not found!"); // begin(uint8_t address)  "Pin 100 - 115"  
  if (! mcp2.begin(1))Serial.println("MCP23017 2 not found!"); // begin(uint8_t address)  "Pin 116 - 131"
  if (! mcp3.begin(2))Serial.println("MCP23017 3 not found!"); // begin(uint8_t address)  "Pin 132 - 147"
  if (! mcp4.begin(3))Serial.println("MCP23017 4 not found!"); // begin(uint8_t address)  "Pin 148 - 163"

  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */
    
    auto relay_1 = new Supla::Control::Relay(100, true);
    auto relay_2 = new Supla::Control::Relay(101, true);
    auto relay_3 = new Supla::Control::Relay(102, true);
    auto relay_4 = new Supla::Control::Relay(103, true);
    auto relay_5 = new Supla::Control::Relay(104, true);
    auto relay_6 = new Supla::Control::Relay(105, true);
    auto relay_7 = new Supla::Control::Relay(106, true);
    auto relay_8 = new Supla::Control::Relay(107, true);
    auto relay_9 = new Supla::Control::Relay(108, true);
    auto relay_10 = new Supla::Control::Relay(109, true);
    auto relay_11 = new Supla::Control::Relay(110, true);
    auto relay_12 = new Supla::Control::Relay(111, true);
    auto relay_13 = new Supla::Control::Relay(112, true);
    auto relay_14 = new Supla::Control::Relay(113, true);
    auto relay_15 = new Supla::Control::Relay(114, true);
    auto relay_16 = new Supla::Control::Relay(115, true);

    auto relay_17 = new Supla::Control::Relay(148, true);
    auto relay_18 = new Supla::Control::Relay(149, true);
    auto relay_19 = new Supla::Control::Relay(150, true);
    auto relay_20 = new Supla::Control::Relay(151, true);
    auto relay_21 = new Supla::Control::Relay(152, true);
    auto relay_22 = new Supla::Control::Relay(153, true);
    auto relay_23 = new Supla::Control::Relay(154, true);
    auto relay_24 = new Supla::Control::Relay(155, true);
    auto relay_25 = new Supla::Control::Relay(156, true);
    auto relay_26 = new Supla::Control::Relay(157, true);
    auto relay_27 = new Supla::Control::Relay(158, true);
    auto relay_28 = new Supla::Control::Relay(159, true);
    auto relay_29 = new Supla::Control::Relay(160, true);
    auto relay_30 = new Supla::Control::Relay(161, true);
    auto relay_31 = new Supla::Control::Relay(162, true);
    auto relay_32 = new Supla::Control::Relay(163, true);

    auto button_1 = new Supla::Control::Button(116, true, true); 
    auto button_2 = new Supla::Control::Button(117, true, true); 
    auto button_3 = new Supla::Control::Button(118, true, true); 
    auto button_4 = new Supla::Control::Button(119, true, true); 
    auto button_5 = new Supla::Control::Button(120, true, true); 
    auto button_6 = new Supla::Control::Button(121, true, true); 
    auto button_7 = new Supla::Control::Button(122, true, true); 
    auto button_8 = new Supla::Control::Button(123, true, true);
    auto button_9 = new Supla::Control::Button(124, true, true); 
    auto button_10 = new Supla::Control::Button(125, true, true); 
    auto button_11 = new Supla::Control::Button(126, true, true); 
    auto button_12 = new Supla::Control::Button(127, true, true); 
    auto button_13 = new Supla::Control::Button(128, true, true); 
    auto button_14 = new Supla::Control::Button(129, true, true); 
    auto button_15 = new Supla::Control::Button(130, true, true); 
    auto button_16 = new Supla::Control::Button(131, true, true); 

    auto button_17 = new Supla::Control::Button(132, true, true); 
    auto button_18 = new Supla::Control::Button(133, true, true); 
    auto button_19 = new Supla::Control::Button(134, true, true); 
    auto button_20 = new Supla::Control::Button(135, true, true); 
    auto button_21 = new Supla::Control::Button(136, true, true); 
    auto button_22 = new Supla::Control::Button(137, true, true); 
    auto button_23 = new Supla::Control::Button(138, true, true); 
    auto button_24 = new Supla::Control::Button(139, true, true);
    auto button_25 = new Supla::Control::Button(140, true, true); 
    auto button_26 = new Supla::Control::Button(141, true, true); 
    auto button_27 = new Supla::Control::Button(142, true, true); 
    auto button_28 = new Supla::Control::Button(143, true, true); 
    auto button_29 = new Supla::Control::Button(144, true, true); 
    auto button_30 = new Supla::Control::Button(145, true, true); 
    auto button_31 = new Supla::Control::Button(146, true, true); 
    auto button_32 = new Supla::Control::Button(147, true, true);
      
    button_1->addAction(Supla::TOGGLE, relay_1, Supla::ON_PRESS);
    button_1->setSwNoiseFilterDelay(50);
    button_2->addAction(Supla::TOGGLE, relay_2, Supla::ON_PRESS);
    button_2->setSwNoiseFilterDelay(50);
    button_3->addAction(Supla::TOGGLE, relay_3, Supla::ON_PRESS);
    button_3->setSwNoiseFilterDelay(50);
    button_4->addAction(Supla::TOGGLE, relay_4, Supla::ON_PRESS);
    button_4->setSwNoiseFilterDelay(50);
    button_5->addAction(Supla::TOGGLE, relay_5, Supla::ON_PRESS);
    button_5->setSwNoiseFilterDelay(50);
    button_6->addAction(Supla::TOGGLE, relay_6, Supla::ON_PRESS);
    button_6->setSwNoiseFilterDelay(50);
    button_7->addAction(Supla::TOGGLE, relay_7, Supla::ON_PRESS);
    button_7->setSwNoiseFilterDelay(50);
    button_8->addAction(Supla::TOGGLE, relay_8, Supla::ON_PRESS);
    button_8->setSwNoiseFilterDelay(50);
    button_9->addAction(Supla::TOGGLE, relay_9, Supla::ON_PRESS);
    button_9->setSwNoiseFilterDelay(50);
    button_10->addAction(Supla::TOGGLE, relay_10, Supla::ON_PRESS);
    button_10->setSwNoiseFilterDelay(50);
    button_11->addAction(Supla::TOGGLE, relay_11, Supla::ON_PRESS);
    button_11->setSwNoiseFilterDelay(50);
    button_12->addAction(Supla::TOGGLE, relay_12, Supla::ON_PRESS);
    button_12->setSwNoiseFilterDelay(50);
    button_13->addAction(Supla::TOGGLE, relay_13, Supla::ON_PRESS);
    button_13->setSwNoiseFilterDelay(50);
    button_14->addAction(Supla::TOGGLE, relay_14, Supla::ON_PRESS);
    button_14->setSwNoiseFilterDelay(50);
    button_15->addAction(Supla::TOGGLE, relay_15, Supla::ON_PRESS);
    button_15->setSwNoiseFilterDelay(50);
    button_16->addAction(Supla::TOGGLE, relay_16, Supla::ON_PRESS);
    button_16->setSwNoiseFilterDelay(50);

    button_17->addAction(Supla::TOGGLE, relay_17, Supla::ON_PRESS);
    button_17->setSwNoiseFilterDelay(50);
    button_18->addAction(Supla::TOGGLE, relay_18, Supla::ON_PRESS);
    button_18->setSwNoiseFilterDelay(50);
    button_19->addAction(Supla::TOGGLE, relay_19, Supla::ON_PRESS);
    button_19->setSwNoiseFilterDelay(50);
    button_20->addAction(Supla::TOGGLE, relay_20, Supla::ON_PRESS);
    button_20->setSwNoiseFilterDelay(50);
    button_21->addAction(Supla::TOGGLE, relay_21, Supla::ON_PRESS);
    button_21->setSwNoiseFilterDelay(50);
    button_22->addAction(Supla::TOGGLE, relay_22, Supla::ON_PRESS);
    button_22->setSwNoiseFilterDelay(50);
    button_23->addAction(Supla::TOGGLE, relay_23, Supla::ON_PRESS);
    button_23->setSwNoiseFilterDelay(50);
    button_24->addAction(Supla::TOGGLE, relay_24, Supla::ON_PRESS);
    button_24->setSwNoiseFilterDelay(50);
    button_25->addAction(Supla::TOGGLE, relay_25, Supla::ON_PRESS);
    button_25->setSwNoiseFilterDelay(50);
    button_26->addAction(Supla::TOGGLE, relay_26, Supla::ON_PRESS);
    button_26->setSwNoiseFilterDelay(50);
    button_27->addAction(Supla::TOGGLE, relay_27, Supla::ON_PRESS);
    button_27->setSwNoiseFilterDelay(50);
    button_28->addAction(Supla::TOGGLE, relay_28, Supla::ON_PRESS);
    button_28->setSwNoiseFilterDelay(50);
    button_29->addAction(Supla::TOGGLE, relay_29, Supla::ON_PRESS);
    button_29->setSwNoiseFilterDelay(50);
    button_30->addAction(Supla::TOGGLE, relay_30, Supla::ON_PRESS);
    button_30->setSwNoiseFilterDelay(50);
    button_31->addAction(Supla::TOGGLE, relay_31, Supla::ON_PRESS);
    button_31->setSwNoiseFilterDelay(50);
    button_32->addAction(Supla::TOGGLE, relay_32, Supla::ON_PRESS);
    button_32->setSwNoiseFilterDelay(50);
 
  /*
   * SuplaDevice Initialization.
   * Server address is available at https://cloud.supla.org 
   * If you do not have an account, you can create it at https://cloud.supla.org/account/create
   * SUPLA and SUPLA CLOUD are free of charge
   * 
   */
 
  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "email@address",   // Email address used to login to Supla Cloud
                    AUTHKEY);          // Authorization key
    
}

void loop() {
  SuplaDevice.iterate();
  delay(25);
}
