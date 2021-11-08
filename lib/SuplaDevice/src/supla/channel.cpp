/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include <string.h>

#include "supla/channel.h"
#include "supla-common/log.h"
#include "supla-common/srpc.h"
#include "tools.h"
#include "events.h"
#include "correction.h"

namespace Supla {

unsigned long Channel::lastCommunicationTimeMs = 0;
TDS_SuplaRegisterDevice_E Channel::reg_dev;

Channel::Channel() : valueChanged(false), channelConfig(false),
  channelNumber(-1), validityTimeSec(0) {
  if (reg_dev.channel_count < SUPLA_CHANNELMAXCOUNT) {
    channelNumber = reg_dev.channel_count;

    memset(&reg_dev.channels[channelNumber], 0,
        sizeof(reg_dev.channels[channelNumber]));
    reg_dev.channels[channelNumber].Number = channelNumber;

    reg_dev.channel_count++;
  } else {
// TODO: add status CHANNEL_LIMIT_EXCEEDED
  }

  setFlag(SUPLA_CHANNEL_FLAG_CHANNELSTATE);
}

Channel::~Channel() {
  reg_dev.channel_count--;
}

void Channel::setNewValue(double dbl) {
  // Apply channel value correction
  dbl += Correction::get(getChannelNumber());

  char newValue[SUPLA_CHANNELVALUE_SIZE];
  if (sizeof(double) == 8) {
    memcpy(newValue, &dbl, 8);
  } else if (sizeof(double) == 4) {
    float2DoublePacked(dbl, (uint8_t *)(newValue));
  }
  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(LOG_DEBUG, "Channel(%d) value changed to %d.%d", channelNumber, static_cast<int>(dbl), static_cast<int>(dbl*100)%100);
  }
}

void Channel::setNewValue(double temp, double humi) {
  // Apply channel value corrections
  temp += Correction::get(getChannelNumber());
  humi += Correction::get(getChannelNumber(), true);

  char newValue[SUPLA_CHANNELVALUE_SIZE];
  _supla_int_t t = temp * 1000.00;
  _supla_int_t h = humi * 1000.00;

  memcpy(newValue, &t, 4);
  memcpy(&(newValue[4]), &h, 4);

  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(LOG_DEBUG,
              "Channel(%d) value changed to temp(%f), humi(%f)",
              channelNumber,
              temp,
              humi);
  }
}

void Channel::setNewValue(unsigned _supla_int64_t value) {
  char newValue[SUPLA_CHANNELVALUE_SIZE];

  memset(newValue, 0, SUPLA_CHANNELVALUE_SIZE);

  memcpy(newValue, &value, sizeof(value));
  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(
        LOG_DEBUG, "Channel(%d) value changed to %d", channelNumber, static_cast<int>(value));
  }
}

void Channel::setNewValue(_supla_int_t value) {
  char newValue[SUPLA_CHANNELVALUE_SIZE];

  memset(newValue, 0, SUPLA_CHANNELVALUE_SIZE);

  memcpy(newValue, &value, sizeof(value));
  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(
        LOG_DEBUG, "Channel(%d) value changed to %d", channelNumber, value);
  }
}

void Channel::setNewValue(bool value) {
  char newValue[SUPLA_CHANNELVALUE_SIZE];

  memset(newValue, 0, SUPLA_CHANNELVALUE_SIZE);

  newValue[0] = value;
  if (setNewValue(newValue)) {
    if (value) {
      runAction(Supla::ON_TURN_ON);
    } else {
      runAction(Supla::ON_TURN_OFF);
    }
    runAction(Supla::ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);

    supla_log(
        LOG_DEBUG, "Channel(%d) value changed to %d", channelNumber, value);
  }
}

void Channel::setNewValue(TElectricityMeter_ExtendedValue_V2 &emValue) {
  // Prepare standard channel value
  if (sizeof(TElectricityMeter_Value) <= SUPLA_CHANNELVALUE_SIZE) {
    TElectricityMeter_Measurement *m = nullptr;
    TElectricityMeter_Value v;
    memset(&v, 0, sizeof(TElectricityMeter_Value));

    unsigned _supla_int64_t fae_sum = emValue.total_forward_active_energy[0] +
                                      emValue.total_forward_active_energy[1] +
                                      emValue.total_forward_active_energy[2];

    v.total_forward_active_energy = fae_sum / 1000;

    if (emValue.m_count && emValue.measured_values & EM_VAR_VOLTAGE) {
      m = &emValue.m[emValue.m_count - 1];

      if (m->voltage[0] > 0) {
        v.flags |= EM_VALUE_FLAG_PHASE1_ON;
      }

      if (m->voltage[1] > 0) {
        v.flags |= EM_VALUE_FLAG_PHASE2_ON;
      }

      if (m->voltage[2] > 0) {
        v.flags |= EM_VALUE_FLAG_PHASE3_ON;
      }
    }

    memcpy(reg_dev.channels[channelNumber].value,
           &v,
           sizeof(TElectricityMeter_Value));
    setUpdateReady();
  }
}

bool Channel::setNewValue(char *newValue) {
  if (memcmp(newValue, reg_dev.channels[channelNumber].value, 8) != 0) {
    memcpy(reg_dev.channels[channelNumber].value, newValue, 8);
    setUpdateReady();
    return true;
  }
  return false;
}

void Channel::setType(_supla_int_t type) {
  if (channelNumber >= 0) {
    reg_dev.channels[channelNumber].Type = type;
  }
}

void Channel::setDefault(_supla_int_t value) {
  if (channelNumber >= 0) {
    reg_dev.channels[channelNumber].Default = value;
  }
}

void Channel::setFlag(_supla_int_t flag) {
  if (channelNumber >= 0) {
    reg_dev.channels[channelNumber].Flags |= flag;
  }
}

void Channel::unsetFlag(_supla_int_t flag) {
  if (channelNumber >= 0) {
    reg_dev.channels[channelNumber].Flags &= ~flag;
  }
}

void Channel::setFuncList(_supla_int_t functions) {
  if (channelNumber >= 0) {
    reg_dev.channels[channelNumber].FuncList = functions;
  }
}

_supla_int_t Channel::getFuncList() {
  if (channelNumber >= 0) {
    return reg_dev.channels[channelNumber].FuncList;
  }
  return 0;
}

void Channel::setActionTriggerCaps(_supla_int_t caps) {
  supla_log(LOG_DEBUG, "Channel[%d] setting func list: %d", channelNumber, caps);
  setFuncList(caps);
}

_supla_int_t Channel::getActionTriggerCaps() {
  return getFuncList();
}

int Channel::getChannelNumber() {
  return channelNumber;
}

void Channel::clearUpdateReady() {
  valueChanged = false;
};

void Channel::sendUpdate(void *srpc) {
  if (valueChanged) {
    clearUpdateReady();
    srpc_ds_async_channel_value_changed_c(
        srpc, channelNumber, reg_dev.channels[channelNumber].value,
        0, validityTimeSec);

    // returns null for non-extended channels
    TSuplaChannelExtendedValue *extValue = getExtValue();
    if (extValue) {
      srpc_ds_async_channel_extendedvalue_changed(srpc, channelNumber, extValue);
    }
  }

  // send channel config request if needed
  if (channelConfig) {
    channelConfig = false;
    channelConfig = false;
    TDS_GetChannelConfigRequest request = {};
    request.ChannelNumber = getChannelNumber();
    srpc_ds_async_get_channel_config(srpc, &request);
  }
}

TSuplaChannelExtendedValue *Channel::getExtValue() {
  return nullptr;
}

void Channel::setUpdateReady() {
  valueChanged = true;
};

bool Channel::isUpdateReady() {
  return valueChanged || channelConfig;
};

bool Channel::isExtended() {
  return false;
}

void Channel::setNewValue(const TRollerShutterValue &value) {
  char newValue[SUPLA_CHANNELVALUE_SIZE] = {};
  memcpy(newValue, &value, sizeof(TRollerShutterValue));

  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(
        LOG_DEBUG, "Channel(%d) value changed to %d", channelNumber, value.position);
  }

}

void Channel::setNewValue(uint8_t red,
                   uint8_t green,
                   uint8_t blue,
                   uint8_t colorBrightness,
                   uint8_t brightness) {
  char newValue[SUPLA_CHANNELVALUE_SIZE];
  memset(newValue, 0, SUPLA_CHANNELVALUE_SIZE);
  newValue[0] = brightness;
  newValue[1] = colorBrightness;
  newValue[2] = blue;
  newValue[3] = green;
  newValue[4] = red;
  if (setNewValue(newValue)) {
    runAction(ON_CHANGE);
    runAction(ON_SECONDARY_CHANNEL_CHANGE);
    supla_log(LOG_DEBUG, "Channel(%d) value changed to RGB(%d, %d, %d), colBr(%d), bright(%d)", channelNumber, red, green, blue, colorBrightness, brightness);
  }
}

_supla_int_t Channel::getChannelType() {
  if (channelNumber >= 0) {
    return reg_dev.channels[channelNumber].Type;
  }
  return -1;
}

double Channel::getValueDouble() {
  double value;
  if (sizeof(double) == 8) {
    memcpy(&value, reg_dev.channels[channelNumber].value, 8);
  } else if (sizeof(double) == 4) {
    value = doublePacked2float((uint8_t *)(reg_dev.channels[channelNumber].value));
  }
  
  return value;
}

double Channel::getValueDoubleFirst() { 
  _supla_int_t value;
  memcpy(&value, reg_dev.channels[channelNumber].value, 4);

  return value / 1000.0;
}

double Channel::getValueDoubleSecond() {
  _supla_int_t value;
  memcpy(&value, &(reg_dev.channels[channelNumber].value[4]), 4);

  return value / 1000.0;
}

_supla_int_t Channel::getValueInt32() {
  _supla_int_t value;
  memcpy(&value, reg_dev.channels[channelNumber].value, sizeof(value));
  return value;
}
 
unsigned _supla_int64_t Channel::getValueInt64() {
  unsigned _supla_int64_t value;
  memcpy(&value, reg_dev.channels[channelNumber].value, sizeof(value));
  return value;
}

bool Channel::getValueBool() {
  return reg_dev.channels[channelNumber].value[0];
}

uint8_t Channel::getValueRed() {
  return reg_dev.channels[channelNumber].value[4];
}

uint8_t Channel::getValueGreen() {
  return reg_dev.channels[channelNumber].value[3];
}

uint8_t Channel::getValueBlue() {
  return reg_dev.channels[channelNumber].value[2];
}

uint8_t Channel::getValueColorBrightness() {
  return reg_dev.channels[channelNumber].value[1];
}

uint8_t Channel::getValueBrightness() {
  return reg_dev.channels[channelNumber].value[0];
}

void Channel::setValidityTimeSec(unsigned _supla_int_t timeSec) {
  validityTimeSec = timeSec;
  if (validityTimeSec > 0) unsetFlag(SUPLA_CHANNEL_FLAG_CHANNELSTATE);
}

void Channel::setCorrection(double correction, bool forSecondaryValue) {
  Correction::add(getChannelNumber(), correction, forSecondaryValue);
}

void Channel::requestChannelConfig() {
  channelConfig = true;
}

};  // namespace Supla
