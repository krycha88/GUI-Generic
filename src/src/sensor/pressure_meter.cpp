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

#include "pressure_meter.h"

#include <Arduino.h>
#include <supla/events.h>

Supla::Sensor::PressMeter::PressMeter() {
  pressureChannel.setType(SUPLA_CHANNELTYPE_PRESSURESENSOR);
  pressureChannel.setDefaultFunction(SUPLA_CHANNELFNC_PRESSURESENSOR);
}

double Supla::Sensor::PressMeter::getPressure() {
  return PRESSURE_NOT_AVAILABLE;
}

void Supla::Sensor::PressMeter::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    pressureChannel.setNewValue(getPressure());
  }
}

bool Supla::Sensor::PressMeter::iterateConnected() {
  bool response = true;
  if (pressureChannel.isUpdateReady() &&
      millis() - lastCommunicationTimeMs > 100) {
    lastCommunicationTimeMs = millis();
    pressureChannel.sendUpdate();
    response = false;
  }

  if (!Element::iterateConnected()) {
    response = false;
  }
  return response;
}

Supla::Channel *Supla::Sensor::PressMeter::getSecondaryChannel() {
  return &pressureChannel;
}

void Supla::Sensor::PressMeter::addAction(int action,
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
  // channel.addAction(action, client, event);
}

void Supla::Sensor::PressMeter::addAction(int action,
                                          ActionHandler *client,
                                          int event) {
  addAction(action, *client, event);
}
