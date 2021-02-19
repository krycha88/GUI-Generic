#include <Wire.h>
#include <FunctionalInterrupt.h>
#include "S_MCP23017.h"

static const uint8_t MCP23017_BASEADDRESS = 0x20;

static const uint8_t MCP23017_IODIRA = 0x00;
static const uint8_t MCP23017_IODIRB = 0x01;
static const uint8_t MCP23017_IPOLA = 0x02;
static const uint8_t MCP23017_IPOLB = 0x03;
static const uint8_t MCP23017_GPINTENA = 0x04;
static const uint8_t MCP23017_GPINTENB = 0x05;
static const uint8_t MCP23017_DEFVALA = 0x06;
static const uint8_t MCP23017_DEFVALB = 0x07;
static const uint8_t MCP23017_INTCONA = 0x08;
static const uint8_t MCP23017_INTCONB = 0x09;
static const uint8_t MCP23017_IOCONA = 0x0A;
static const uint8_t MCP23017_IOCONB = 0x0B;
static const uint8_t MCP23017_GPPUA = 0x0C;
static const uint8_t MCP23017_GPPUB = 0x0D;
static const uint8_t MCP23017_INTFA = 0x0E;
static const uint8_t MCP23017_INTFB = 0x0F;
static const uint8_t MCP23017_INTCAPA = 0x10;
static const uint8_t MCP23017_INTCAPB = 0x11;
static const uint8_t MCP23017_GPIOA = 0x12;
static const uint8_t MCP23017_GPIOB = 0x13;
static const uint8_t MCP23017_OLATA = 0x14;
static const uint8_t MCP23017_OLATB = 0x15;

#ifdef ESP8266
void MCP23017::init(uint8_t sda, uint8_t scl, bool fast) {
  Wire.begin(sda, scl);
  if (fast)
    Wire.setClock(400000);
}
#else
void MCP23017::init(bool fast) {
  Wire.begin();
  if (fast)
    Wire.setClock(400000);
}
#endif

bool MCP23017::begin(uint8_t address) {
  _address = MCP23017_BASEADDRESS | (address & 0x07);
  return (writeReg16(MCP23017_IOCONA, 0x4242) && writeReg16(MCP23017_IODIRA, 0xFFFF)); // INT MIRROR & INT POL HIGH, ALL INPUTS
}

void MCP23017::pinMode(uint8_t pin, uint8_t mode) {
  if (pin < 16) {
    if (mode == OUTPUT) {
      updateReg(pin < 8 ? MCP23017_IODIRA : MCP23017_IODIRB, ~(uint8_t)(1 << (pin % 8)), 0x00);
    } else if ((mode == INPUT) || (mode == INPUT_PULLUP)) {
      updateReg(pin < 8 ? MCP23017_IODIRA : MCP23017_IODIRB, 0xFF, 1 << (pin % 8));
      if (mode == INPUT_PULLUP) {
        updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, 0xFF, 1 << (pin % 8));
        updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, 0xFF, 1 << (pin % 8));
      } else {
        updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, ~(uint8_t)(1 << (pin % 8)), 0x00);
        updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, ~(uint8_t)(1 << (pin % 8)), 0x00);
      }
    }
  }
}

void MCP23017::setPullup(uint8_t pin, bool pullup, bool inverse) {
  if (pin < 16) {
    if (pullup)
      updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPPUA : MCP23017_GPPUB, ~(uint8_t)(1 << (pin % 8)), 0x00);
    if (inverse)
      updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_IPOLA : MCP23017_IPOLB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

bool MCP23017::digitalRead(uint8_t pin) {
  unsigned long now = millis();
  if (now > get_ba){
    ba = readReg16(MCP23017_GPIOA);  // --------- reads "readReg16" only once every 20 ms  ----
    get_ba = now + 20;  
  }
  if (pin < 16) {
    return ((ba >> (pin % 16)) & 0x01);
  }
}

void MCP23017::digitalWrite(uint8_t pin, bool value) {
  if (pin < 16) {
    if (value)
      updateReg(pin < 8 ? MCP23017_GPIOA : MCP23017_GPIOB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPIOA : MCP23017_GPIOB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

uint16_t MCP23017::digitalReads() {
  return readReg16(MCP23017_GPIOA);
}

void MCP23017::digitalWrites(uint16_t values) {
  writeReg16(MCP23017_GPIOA, values);
}

void MCP23017::attachInterrupt(uint8_t pin, callback_t callback) {
  _callback = callback;
  ::pinMode(pin, INPUT);
  ::attachInterrupt(digitalPinToInterrupt(pin), std::bind(&MCP23017::_interrupt, this), RISING);
}

void MCP23017::detachInterrupt(uint8_t pin) {
  ::detachInterrupt(digitalPinToInterrupt(pin));
  _callback = NULL;
}

void MCP23017::setupInterrupt(uint8_t pin, bool enable) {
  if (pin < 16) {
    if (enable)
      updateReg(pin < 8 ? MCP23017_GPINTENA : MCP23017_GPINTENB, 0xFF, 1 << (pin % 8));
    else
      updateReg(pin < 8 ? MCP23017_GPINTENA : MCP23017_GPINTENB, ~(uint8_t)(1 << (pin % 8)), 0x00);
  }
}

void MCP23017::setupInterrupts(uint16_t pins, bool enable) {
  if (enable)
    updateReg16(MCP23017_GPINTENA, 0xFFFF, pins);
  else
    updateReg16(MCP23017_GPINTENA, ~pins, 0x00);
}

bool MCP23017::writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(value);
  return (Wire.endTransmission() == 0);
}

bool MCP23017::writeReg16(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(value & 0xFF);
  Wire.write(value >> 8);
  return (Wire.endTransmission() == 0);
}

uint8_t MCP23017::readReg(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
    return 0; // Error!
  Wire.requestFrom(_address, (uint8_t)1);
  return Wire.read();
}

uint16_t MCP23017::readReg16(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
    return 0; // Error!
  Wire.requestFrom(_address, (uint8_t)2);
  uint8_t a = Wire.read();
  return ((Wire.read() << 8) | a);
}

bool MCP23017::updateReg(uint8_t reg, uint8_t andMask, uint8_t orMask) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
    return false; // Error!
  Wire.requestFrom(_address, (uint8_t)1);
  uint8_t a = (Wire.read() & andMask) | orMask;
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(a);
  return (Wire.endTransmission() == 0);
}

bool MCP23017::updateReg16(uint8_t reg, uint16_t andMask, uint16_t orMask) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
    return false; // Error!
  Wire.requestFrom(_address, (uint8_t)2);
  uint16_t ab = Wire.read();
  ab |= (Wire.read() << 8);
  ab &= andMask;
  ab |= orMask;
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(ab & 0xFF);
  Wire.write(ab >> 8);
  return (Wire.endTransmission() == 0);
}

void IRAM_ATTR MCP23017::_interrupt() {
  uint16_t pins, values;

  pins = readReg16(MCP23017_INTFA);
  values = readReg16(MCP23017_INTCAPA);
  if (_callback)
    _callback(pins, values);
}
