
#ifndef S_mcp_23017_h
#define S_mcp_23017_h

#include <Arduino.h>
#include <supla/control/MCP23017/S_MCP23017.h>
#include <supla/io.h>


	MCP23017 mcp1;
    MCP23017 mcp2;
    MCP23017 mcp3;
    MCP23017 mcp4;


class CustomControl : public Supla::Io {
  public:
    void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
      if (pin < 100) {
        return ::digitalWrite(pin,val);   
      }     
      if ((pin > 99) && (pin < 116)){
        mcp1.digitalWrite(pin - 100, val);
         return;
      }
      if ((pin > 115) && (pin < 132)){
        mcp2.digitalWrite(pin - 116, val);
      }
      if ((pin > 131) && (pin < 148)){
        mcp3.digitalWrite(pin - 132, val);
         return;
      }
      if ((pin > 147) && (pin < 164)){
        mcp4.digitalWrite(pin - 148, val);
         return;
      } 
   }   
   int customDigitalRead(int channelNumber, uint8_t pin) {
      if (pin < 100){
        return ::digitalRead(pin);  
      }      
      if ((pin > 99)&& (pin < 116)){
        return mcp1.digitalRead(pin - 100);    
      }
      if ((pin > 115)&& (pin < 132)){
        return mcp2.digitalRead(pin - 116);
      }
      if ((pin > 131)&& (pin < 148)){
        return mcp3.digitalRead(pin - 132);    
      }
      if ((pin > 147)&& (pin < 164)){
        return mcp4.digitalRead(pin - 148);    
      }     
    }
   void customPinMode(int channelNumber, uint8_t pin, uint8_t mode) {
      (void)(channelNumber);
      if (pin < 100){
        return ::pinMode(pin, mode);  
      }       
      if ((pin > 99)&& (pin < 116)){
        mcp1.pinMode(pin - 100, mode);
      }
      if ((pin > 115)&& (pin < 132)){
        mcp2.pinMode(pin - 116, mode);
      }
      if ((pin > 131)&& (pin < 148)){
        mcp3.pinMode(pin - 132, mode);
      }
      if ((pin > 147)&& (pin < 164)){
        mcp4.pinMode(pin - 148, mode);
      }     
    } 
	      
}CustomControl; 


#endif

