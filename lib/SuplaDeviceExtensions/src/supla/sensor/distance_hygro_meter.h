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

#ifndef _distance_hygro_meter_h
#define _distance_hygro_meter_h

#include <Arduino.h>

#include "supla/sensor/distance.h"

#define HUMIDITY_NOT_AVAILABLE -1

namespace Supla {
namespace Sensor {
class DistanceHumidityMeter : public Distance {
 public:
  DistanceHumidityMeter();
  virtual double getHumi();
  void iterateAlways();
  bool iterateConnected();
  Element &disableChannelState();
  Channel *getSecondaryChannel();

  // Override local action methods in order to delegate execution to Channel and
  // Secondary Channel
  void addAction(int action, ActionHandler &client, int event);
  void addAction(int action, ActionHandler *client, int event);

 protected:
  Channel humidityChannel;
  unsigned long lastCommunicationTimeMs;
};

};  // namespace Sensor
};  // namespace Supla

#endif
