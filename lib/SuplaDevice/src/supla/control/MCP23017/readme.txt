Support for up to 4 x "MCP23017" additional 64 Gpios.


BASIC USE:

INSTANTIATE
#include <supla_mcp23017.h>

SETUP
  mcp1.init(uint8_t sda, uint8_t scl);  //  init(uint8_t sda, uint8_t scl) = Wire.begin();

  mcp1.begin(uint8_t mcp23017_address); // Pin 100 - 115
  mcp2.begin(uint8_t mcp23017_address); // Pin 116 - 131
  mcp3.begin(uint8_t mcp23017_address); // Pin 132 - 147
  mcp4.begin(uint8_t mcp23017_address); // Pin 148 - 163