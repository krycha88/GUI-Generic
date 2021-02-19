#include "GUIGenericCommon.h"
#include "SuplaDeviceGUI.h"

uint8_t *HexToBytes(String _value) {
  int size = 16;
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (size));

  for (int i = 0; i < size; i++) {
    sscanf(&_value[i * 2], "%2hhx", &buffer[i]);
  }
  return buffer;
}

int getCountSensorChannels() {
  int maxFrame = 0;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        maxFrame += 1;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        maxFrame += 2;
      }
    }

    if (element->getSecondaryChannel()) {
      auto channel = element->getSecondaryChannel();
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        maxFrame += 1;
      }
    }
  }

  return maxFrame;
}

uint32_t lowestRAM = 0;
uint32_t lowestFreeStack = 0;

void checkRAM() {
  uint32_t freeRAM = ESP.getFreeHeap();
  Serial.print(F("freeRAM: "));
  Serial.println(freeRAM);
  if (freeRAM <= lowestRAM) {
    lowestRAM = freeRAM;
  }
  uint32_t freeStack = ESP.getFreeContStack();
  Serial.print(F("freeStack: "));
  Serial.println(freeStack);
  if (freeStack <= lowestFreeStack) {
    lowestFreeStack = freeStack;
  }
}