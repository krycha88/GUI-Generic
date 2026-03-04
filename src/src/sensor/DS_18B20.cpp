#ifdef SUPLA_DS18B20

#include "DS_18B20.h"
#include "../../SuplaDeviceGUI.h"

OneWire DS18B20::sharedOneWire;
DallasTemperature DS18B20::sharedSensors(&sharedOneWire);
unsigned long DS18B20::lastConversionTime = 0;

void DS18B20::initSharedResources(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);

  sharedOneWire.begin(pin);
  sharedSensors.setOneWire(&sharedOneWire);
  sharedSensors.begin();
  sharedSensors.setResolution(10);

  waitForAndRequestTemperatures();
}

void DS18B20::onInit() {
  channel.setNewValue(getValue());
}

DS18B20::DS18B20(uint8_t *deviceAddress) : lastValidValue(TEMPERATURE_NOT_AVAILABLE), retryCounter(0), lastUpdateTime(0) {
  unsigned long currentTime = millis();
  channel.setNewValue(getValue());

  lastConversionTime = currentTime;
  lastUpdateTime = currentTime;

  if (deviceAddress == nullptr) {
    address[0] = 0;
  }
  else {
    memcpy(address, deviceAddress, 8);
  }
}

void DS18B20::iterateAlways() {
  unsigned long currentTime = millis();
  unsigned long timeSinceLastConversion = currentTime - lastConversionTime;

  static bool requestDone = false;

  if (timeSinceLastConversion >= conversionInterval && !requestDone) {
    sharedSensors.requestTemperatures();
    lastConversionTime = currentTime;
    requestDone = true;
  }

  if (timeSinceLastConversion < conversionInterval) {
    requestDone = false;
  }

  unsigned long timeSinceLastOperation = currentTime - lastUpdateTime;

  if (timeSinceLastOperation >= conversionInterval + 2000) {
    float temp = getValue();
    channel.setNewValue(temp);
    lastUpdateTime = currentTime;
  }

  yield();
}

double DS18B20::getValue() {
  double value = TEMPERATURE_NOT_AVAILABLE;

  if (address[0] == 0 && ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() == 1) {
    value = sharedSensors.getTempCByIndex(0);
  }
  else {
    value = sharedSensors.getTempC(address);
  }

  if (value == DEVICE_DISCONNECTED_C || value == 85.0) {
    value = TEMPERATURE_NOT_AVAILABLE;
  }

  if (value == TEMPERATURE_NOT_AVAILABLE) {
    retryCounter++;
    if (retryCounter > 3) {
      lastConversionTime = 0;
      lastUpdateTime = millis();
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

void DS18B20::waitForAndRequestTemperatures() {
  sharedSensors.setWaitForConversion(false);
  sharedSensors.requestTemperatures();
}

void DS18B20::restartOneWire() {
  if (ConfigManager->get(KEY_MULTI_MAX_DS18B20)->getValueInt() == 1) {
    supla_log(LOG_DEBUG, "Restarting OneWire...");
    sharedOneWire.reset();
    waitForAndRequestTemperatures();
    channel.setNewValue(getValue());
  }
}

void DS18B20::setDeviceAddress(uint8_t *deviceAddress) {
  if (deviceAddress == nullptr) {
    supla_log(LOG_DEBUG, "Device address not provided. Using device from index 0");
  }
  else {
    memcpy(address, deviceAddress, 8);
  }
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
#endif