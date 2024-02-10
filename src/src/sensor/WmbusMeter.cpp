#include "WmbusMeter.h"
#include <string>
namespace Supla
{
  namespace Sensor
  {   
    WmbusMeter::WmbusMeter(uint8_t mosi, uint8_t miso, uint8_t clk, uint8_t cs, uint8_t gdo0, uint8_t gdo2)
    {
      Serial.print("wMBus-lib: Initializing with GPIO: ");
      Serial.print(mosi);
      Serial.print(",");
      Serial.print(miso);
      Serial.print(",");
      Serial.print(clk);
      Serial.print(",");
      Serial.print(cs);
      Serial.print(",");
      Serial.print(gdo0);
      Serial.print(",");
      Serial.print(gdo2);
      Serial.println(" GPIO");
      bool isInitialized = receiver.init(mosi, miso, clk, cs, gdo0, gdo2);
      if (isInitialized)
      {
        Serial.println("wMBus-lib: Receiver started.");
      }
    };

    void WmbusMeter::add_driver(Driver *driver)
    {
      this->drivers_[driver->get_name()] = driver;
    }

    void WmbusMeter::add_sensor(SensorBase *sensor)
    {
      this->sensors_[sensor->get_meter_id()] = sensor;
      Serial.println("------");
      Serial.print("Type:");
      Serial.println(sensor->get_type().c_str());
      Serial.print("Id:");
      Serial.println(sensor->get_meter_id().c_str());
      Serial.print("Key:");
      Serial.println(sensor->get_key_string().c_str());
      Serial.print("Property:");
      Serial.println(sensor->get_property_to_send().c_str());
    }

    bool WmbusMeter::decrypt_telegram(std::vector<unsigned char> &telegram, std::vector<unsigned char> key)
    {
      bool ret_val = false;
      std::vector<unsigned char>::iterator pos;
      // CI
      pos = telegram.begin() + 10;
      // data offset
      int offset{0};

      unsigned char iv[16];
      int i = 0;

      if ((*pos == 0x67) || (*pos == 0x6E) || (*pos == 0x74) ||
          (*pos == 0x7A) || (*pos == 0x7D) || (*pos == 0x7F) || (*pos == 0x9E))
      {
        offset = 15;

        // dll-mfct + dll-id + dll-version + dll-type
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[2 + j];
        }
        // tpl-acc
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[11];
        }
      }
      else if ((*pos == 0x68) || (*pos == 0x6F) || (*pos == 0x72) ||
               (*pos == 0x75) || (*pos == 0x7C) || (*pos == 0x7E) || (*pos == 0x9F))
      {
        offset = 23;

        // tpl-mfct
        for (int j = 0; j < 2; ++j)
        {
          iv[i++] = telegram[15 + j];
        }
        // tpl-id
        for (int j = 0; j < 4; ++j)
        {
          iv[i++] = telegram[11 + j];
        }
        // tpl-version + tpl-type
        for (int j = 0; j < 2; ++j)
        {
          iv[i++] = telegram[17 + j];
        }
        // tpl-acc
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[19];
        }
      }
      else
      {
        Serial.println("wMBus-lib: CI unknown");
      }

      pos = telegram.begin() + offset;
      int num_encrypted_bytes = 0;
      int num_not_encrypted_at_end = 0;

      if (decrypt_TPL_AES_CBC_IV(telegram, pos, key, iv,
                                 &num_encrypted_bytes, &num_not_encrypted_at_end))
      {
        uint32_t decrypt_check = 0x2F2F;
        uint32_t dc = (((uint32_t)telegram[offset] << 8) | ((uint32_t)telegram[offset + 1]));
        if (dc == decrypt_check)
        {
          ret_val = true;
        }
      }
      return ret_val;
    }

    float WmbusMeter::parse_frame(std::vector<unsigned char> &frame)
    {
      Serial.println("wMBus-lib: Formatting as string.");

      std::string telegram = format_hex_pretty(frame);

      Serial.println("wMBus-lib: Removing helping characters.");

      telegram.erase(std::remove(telegram.begin(), telegram.end(), '.'), telegram.end());

      Serial.println("wMBus-lib: Getting meter id.");
      std::string meterIdString = telegram.substr(8, 8);
      char s[9]= {0,0,0,0,0,0,0,0,0};
      s[0]=meterIdString[6];
      s[1]=meterIdString[7];
      s[2]=meterIdString[4];
      s[3]=meterIdString[5];
      s[4]=meterIdString[2];
      s[5]=meterIdString[3];
      s[6]=meterIdString[0];
      s[7]=meterIdString[1];
      std::string meterIdRealString = s;

      // either we have sensors defined or we have just one without ID (like Izar)
      if(sensors_.count(meterIdRealString) > 0 || (sensors_.size() == 1 && sensors_.begin()->second->get_meter_id() == "" ))
      {
        Serial.println("wMBus-lib: Getting sensor config.");
        auto sensor = (sensors_.size() == 1 && sensors_.begin()->second->get_meter_id() == "" )? sensors_.begin()->second : sensors_[meterIdRealString];
        bool isOk = true;
        float readValue = 0.0;
        if (sensor->get_key().size() >0)
        {
          Serial.println("wMBus-lib: Key provided, decrypting frame.");
          if (!decrypt_telegram(frame, sensor->get_key()))
          {
            isOk = false;
          }
        }
        if (isOk)
        {
          if(this->drivers_.count(sensor->get_type()))
          {
            Serial.println("wMBus-lib: Getting driver.");
            auto driver = this->drivers_[sensor->get_type()];
            auto mapValues = driver->get_values(frame);
            readValue = mapValues[sensor->get_property_to_send()];
            Serial.print("Meter id as string: ");
            Serial.println(meterIdRealString.c_str());
            Serial.print("Meter id as number: ");
            Serial.println(meterIdString.c_str());
            Serial.print(readValue);
            Serial.println("m3");
            if(fabs(readValue - lastReadValue) > 0.01f)
            {
              sensor->setNewValue((unsigned _supla_int64_t)(readValue * 1000));
              sensor->iterateAlways();
              lastReadValue = readValue;
            }
            else
            {
              Serial.println("Value is lower than previous one, ignoring.");
            }
          }
          else
          {
            Serial.print("wMBus-lib: Driver for sensor: ");
            Serial.print(sensor->get_type().c_str());
            Serial.println(" does not exist.");
          }
        }
        else
        {
          Serial.println("wMBus-lib: Failed to decrypt telegram.");
        }
        return readValue;
      }
      else
      {
        Serial.print("wMBus-lib: Config for meter: ");
        Serial.print(meterIdRealString.c_str());
        Serial.println(" does not exist.");
      }
      return 0.0;
    }

    void WmbusMeter::onFastTimer()
    {
      if (receiver.task())
      {
        Serial.println("wMBus-lib: Found telegram.");
        dumpHex(receiver.MBpacket, packetLength);
        WMbusFrame mbus_data = receiver.get_frame();
        std::vector<unsigned char> frame = mbus_data.frame;
        Serial.println("----------------");
        Serial.println("wMBus-lib: Parsing frame.");
        parse_frame(frame);
        Serial.println("----------------");
      }
    }

  };
};

Supla::Sensor::WmbusMeter *meter;