#ifdef SUPLA_DS18B20
#include "DS_18B20.h"
#include "../../SuplaDeviceGUI.h"

namespace Supla {
namespace Sensor {

OneWireBus::OneWireBus(uint8_t pinNumber) : pin(pinNumber), nextBus(nullptr), lastReadTime(0), oneWire(pinNumber) {
  SUPLA_LOG_DEBUG("Initializing OneWire bus at pin %d", pinNumber);
  sensors.setOneWire(&oneWire);
  sensors.begin();

  if (sensors.isParasitePowerMode()) {
    SUPLA_LOG_DEBUG("OneWire(pin %d) Parasite power is ON", pinNumber);
  }
  else {
    SUPLA_LOG_DEBUG("OneWire(pin %d) Parasite power is OFF", pinNumber);
  }

  SUPLA_LOG_DEBUG("OneWire(pin %d) Found %d devices:", pinNumber, sensors.getDeviceCount());

  DeviceAddress address;
  char strAddr[64];

  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    if (!sensors.getAddress(address, i)) {
      SUPLA_LOG_DEBUG("Unable to find address for Device %d", i);
    }
    else {
      snprintf(strAddr, sizeof(strAddr), "{0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}", address[0], address[1], address[2],
               address[3], address[4], address[5], address[6], address[7]);

      SUPLA_LOG_DEBUG("Index %d - address %s", i, strAddr);
      sensors.setResolution(address, 12);
    }
    delay(0);
  }

  sensors.setWaitForConversion(true);
  sensors.requestTemperatures();
  sensors.setWaitForConversion(false);
}

int8_t OneWireBus::getIndex(uint8_t *deviceAddress) {
  DeviceAddress address;

  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    if (sensors.getAddress(address, i)) {
      bool found = true;
      for (int j = 0; j < 8; j++) {
        if (deviceAddress[j] != address[j]) {
          found = false;
        }
      }

      if (found) {
        return i;
      }
    }
  }

  return -1;
}

// ---------------- DS18B20 ----------------

OneWireBus *DS18B20::oneWireBus = nullptr;

DS18B20::DS18B20(uint8_t pin, uint8_t *deviceAddress) {
  OneWireBus *bus = oneWireBus;
  OneWireBus *prevBus = nullptr;

  address[0] = 0;
  lastValidValue = TEMPERATURE_NOT_AVAILABLE;
  retryCounter = 0;
  lastReadTime = 0;

  if (bus) {
    while (bus) {
      if (bus->pin == pin) {
        myBus = bus;
        break;
      }
      prevBus = bus;
      bus = bus->nextBus;
    }
  }

  if (!bus) {
    SUPLA_LOG_DEBUG("Creating OneWire bus for pin: %d", pin);
    myBus = new OneWireBus(pin);

    if (prevBus) {
      prevBus->nextBus = myBus;
    }
    else {
      oneWireBus = myBus;
    }
  }

  if (deviceAddress == nullptr) {
    SUPLA_LOG_DEBUG("Device address not provided. Using device from index 0");
  }
  else {
    memcpy(address, deviceAddress, 8);
  }
}

void DS18B20::iterateAlways() {
  if (millis() - myBus->lastReadTime > 10000) {
    myBus->sensors.requestTemperatures();
    myBus->lastReadTime = millis();
  }

  if (millis() - myBus->lastReadTime > 5000 && (lastReadTime != myBus->lastReadTime)) {
    channel.setNewValue(getValue());
    lastReadTime = myBus->lastReadTime;
  }
}

double DS18B20::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;

  if (address[0] == 0) {
    value = myBus->sensors.getTempCByIndex(0);
  }
  else {
    value = myBus->sensors.getTempC(address);
  }

  if (value == DEVICE_DISCONNECTED_C || value == 85.0) {
    value = TEMPERATURE_NOT_AVAILABLE;
  }

  if (value == TEMPERATURE_NOT_AVAILABLE) {
    retryCounter++;
    if (retryCounter > 3) {
      retryCounter = 0;
    }
    else {
      value = lastValidValue;
    }
  }
  else {
    retryCounter = 0;
  }

  lastValidValue = value;
  return value;
}

DallasTemperature &DS18B20::getHwSensors() {
  return myBus->sensors;
}

void DS18B20::findAndSaveDS18B20Addresses() {
  uint8_t pin = ConfigESP->getGpio(FUNCTION_DS18B20);
  uint8_t maxDevices = ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt();
  OneWire ow(pin);
  DallasTemperature sensors(&ow);

  sensors.begin();

  Serial.print("Szukanie urządzeń DS18B20...");

  int deviceCount = 0;

  for (int i = 0; i < maxDevices; ++i) {
    DeviceAddress devAddr;

    if (sensors.getAddress(devAddr, i)) {
      deviceCount++;

      char devAddrStr[17];
      for (uint8_t j = 0; j < 8; j++) {
        sprintf(devAddrStr + j * 2, "%02X", devAddr[j]);
      }
      devAddrStr[16] = '\0';

      ConfigManager->setElement(KEY_ADDR_DS18B20, i, devAddrStr);

      Serial.print("Znaleziono urządzenie na adresie: ");
      Serial.print(devAddrStr);
      Serial.println();
    }
    else {
      break;
    }
  }

  Serial.print("Znaleziono łącznie ");
  Serial.print(deviceCount);
  Serial.println(" urządzeń DS18B20.");

  ConfigManager->save();
}
void DS18B20::setDeviceAddress(uint8_t *deviceAddress) {
  if (deviceAddress == nullptr) {
    supla_log(LOG_DEBUG, "Device address not provided. Using device from index 0");
  }
  else {
    memcpy(address, deviceAddress, 8);
  }
}

};  // namespace Sensor
};  // namespace Supla
#endif