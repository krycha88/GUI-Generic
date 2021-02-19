#ifndef _S_MCP23017_H
#define _S_MCP23017_H

#include <Arduino.h>
#include <functional>

class MCP23017 {
public:
  typedef std::function<void(uint16_t pins, uint16_t values)> callback_t;

  MCP23017() : _callback(NULL) {}

#ifdef ESP8266
  static void init(uint8_t sda, uint8_t scl, bool fast = true);
  static void init(bool fast = true) {
    init(SDA, SCL, fast);
  }
#else
  static void init(bool fast = true);
#endif

  bool begin(uint8_t address = 0);

  void pinMode(uint8_t pin, uint8_t mode);
  void setPullup(uint8_t pin, bool pullup, bool inverse = false);
  bool digitalRead(uint8_t pin);
  void digitalWrite(uint8_t pin, bool value);
  uint16_t digitalReads();
  void digitalWrites(uint16_t values);

  void attachInterrupt(uint8_t pin, callback_t callback);
  void detachInterrupt(uint8_t pin);
  void setupInterrupt(uint8_t pin, bool enable = true);
  void setupInterrupts(uint16_t pins, bool enable = true);

protected:
  bool writeReg(uint8_t reg, uint8_t value);
  bool writeReg16(uint8_t reg, uint16_t value);
  uint8_t readReg(uint8_t reg);
  uint16_t readReg16(uint8_t reg);
  bool updateReg(uint8_t reg, uint8_t andMask, uint8_t orMask);
  bool updateReg16(uint8_t reg, uint16_t andMask, uint16_t orMask);

  void _interrupt();

  uint8_t _address;
  uint16_t ba = 0;
  unsigned long get_ba = 0;
  callback_t _callback;
};

#endif
