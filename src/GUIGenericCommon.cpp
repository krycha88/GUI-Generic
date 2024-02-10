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

#include "GUIGenericCommon.h"
#include "SuplaDeviceGUI.h"
#include <supla/tools.h>

uint8_t *HexToBytes(String _value) {
  int size = 16;
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (size));

  for (int i = 0; i < size; i++) {
    sscanf(&_value[i * 2], "%2hhx", &buffer[i]);
  }
  return buffer;
}

uint8_t getCountSensorChannels() {
  uint8_t maxFrame = 0;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel()) {
      auto channel = element->getChannel();

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
        maxFrame += 1;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
        maxFrame += 2;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYSENSOR) {
        maxFrame += 1;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_DISTANCESENSOR) {
        maxFrame += 1;
      }

      if (channel->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
        maxFrame += 3;
      }
      if (channel->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR) {
        maxFrame += 1;
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

uint8_t getCountActiveThermostat() {
  uint8_t activeThermostatOled = 0;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel() && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HVAC) {
      activeThermostatOled++;
    }
  }
  return activeThermostatOled;
}

int getCountChannels() {
  int count = 0;
  for (auto element = Supla::Element::begin(); element != nullptr; element = element->next()) {
    if (element->getChannel())
      count++;
  }
  return count;
}

void printFreeMemory(const char *location) {
  size_t freeMemory = ESP.getFreeHeap();

  SUPLA_LOG_DEBUG("Free memory at %s: %u bytes", location, freeMemory);
}

const String getAPName() {
  uint8_t mac[6] = {};
  char macStr[12 + 6] = {};
  if (Supla::Network::GetMacAddr(mac)) {
    generateHexString(mac, macStr, 6);
  }

  String cstr = "SUPLA-GUI-Generic-";
  cstr.reserve(32);
  cstr += macStr;

  return cstr.c_str();
}