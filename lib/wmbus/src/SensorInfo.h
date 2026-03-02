#ifndef _SensorInfo_h
#define _SensorInfo_h
#include <Arduino.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <supla/channel_element.h>
#include "SensorBase.h"
namespace Supla
{
    namespace Sensor
    {
        class SensorInfo : public SensorBase, public ChannelElement
        {
        public:
            SensorInfo(std::string meter_id, std::string type, std::string property_to_send, std::string keyString)
                : SensorBase(meter_id, type, property_to_send, keyString)
            {
                channel.setType(SUPLA_CHANNELTYPE_IMPULSE_COUNTER);
                channel.setDefault(SUPLA_CHANNELFNC_IC_WATER_METER);
            };

            void setNewValue(uint64_t value)
            {
                Serial.print("wMBus-lib: Setting new value for sensor with id: ");
                Serial.print(this->get_meter_id().c_str());
                Serial.print(" value: ");
                Serial.println(value);
                channel.setNewValue(value);
            };
        };
    };
};
#endif