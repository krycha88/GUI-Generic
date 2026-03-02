
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

#include "distance_hygro_meter.h"

#include <supla/events.h>

Supla::Sensor::DistanceHumidityMeter::DistanceHumidityMeter() {
  humidityChannel.setType(SUPLA_CHANNELTYPE_HUMIDITYSENSOR);
  humidityChannel.setDefaultFunction(SUPLA_CHANNELFNC_HUMIDITY);
  // humidityChannel.setNewValue(HUMIDITY_NOT_AVAILABLE);
}

double Supla::Sensor::DistanceHumidityMeter::getHumi() {
  return HUMIDITY_NOT_AVAILABLE;
}

void Supla::Sensor::DistanceHumidityMeter::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    humidityChannel.setNewValue(0, getHumi());
  }
  Distance::iterateAlways();
}

bool Supla::Sensor::DistanceHumidityMeter::iterateConnected() {
  bool response = true;
  if (humidityChannel.isUpdateReady() &&
      millis() - lastCommunicationTimeMs > 100) {
    lastCommunicationTimeMs = millis();
    humidityChannel.sendUpdate();
    response = false;
  }

  if (!Element::iterateConnected()) {
    response = false;
  }
  return response;
}

Supla::Element &Supla::Sensor::DistanceHumidityMeter::disableChannelState() {
  humidityChannel.unsetFlag(SUPLA_CHANNEL_FLAG_CHANNELSTATE);
  return Distance::disableChannelState();
}

Supla::Channel *Supla::Sensor::DistanceHumidityMeter::getSecondaryChannel() {
  return &humidityChannel;
}

void Supla::Sensor::DistanceHumidityMeter::addAction(int action,
                                                     ActionHandler &client,
                                                     int event) {
  // delegate secondary channel event registration to secondary channel
  switch (event) {
    case Supla::ON_SECONDARY_CHANNEL_CHANGE: {
      getSecondaryChannel()->addAction(action, client, event);
      return;
    }
  }
  // delegate all other events to primary channel
  channel.addAction(action, client, event);
}

void Supla::Sensor::DistanceHumidityMeter::addAction(int action,
                                                     ActionHandler *client,
                                                     int event) {
  addAction(action, *client, event);
}
