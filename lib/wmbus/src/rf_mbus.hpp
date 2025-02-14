/***********************************************************************************
    Filename: rf_mbus.hpp
***********************************************************************************/

#ifndef _RF_MBUS_H
#define _RF_MBUS_H

#include <stdint.h>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------
//  Constants 
//----------------------------------------------------------------------------------
// CC1101 state machine
#define MARCSTATE_SLEEP            0x00
#define MARCSTATE_IDLE             0x01
#define MARCSTATE_XOFF             0x02
#define MARCSTATE_VCOON_MC         0x03
#define MARCSTATE_REGON_MC         0x04
#define MARCSTATE_MANCAL           0x05
#define MARCSTATE_VCOON            0x06
#define MARCSTATE_REGON            0x07
#define MARCSTATE_STARTCAL         0x08
#define MARCSTATE_BWBOOST          0x09
#define MARCSTATE_FS_LOCK          0x0A
#define MARCSTATE_IFADCON          0x0B
#define MARCSTATE_ENDCAL           0x0C
#define MARCSTATE_RX               0x0D
#define MARCSTATE_RX_END           0x0E
#define MARCSTATE_RX_RST           0x0F
#define MARCSTATE_TXRX_SWITCH      0x10
#define MARCSTATE_RXFIFO_OVERFLOW  0x11
#define MARCSTATE_FSTXON           0x12
#define MARCSTATE_TX               0x13
#define MARCSTATE_TX_END           0x14
#define MARCSTATE_RXTX_SWITCH      0x15
#define MARCSTATE_TXFIFO_UNDERFLOW 0x16

#define RX_FIFO_START_THRESHOLD    0
#define RX_FIFO_THRESHOLD          10
#define RX_AVAILABLE_FIFO          44 

#define FIXED_PACKET_LENGTH        0x00
#define INFINITE_PACKET_LENGTH     0x02

enum WmBusFrameType : uint8_t {
  WMBUS_FRAME_UNKNOWN = 0,
  WMBUS_FRAMEA = 1,
  WMBUS_FRAMEB = 2,
};

enum WmBusFrameMode : uint8_t {
  WMBUS_UNKNOWN_MODE = 0,
  WMBUS_T1_MODE = 1,
  WMBUS_C1_MODE = 2,
};

typedef struct RXinfoDescr {
  uint8_t  lengthField;         // The L-field in the WMBUS packet
  uint16_t length;              // Total number of bytes to to be read from the RX FIFO
  uint16_t bytesLeft;           // Bytes left to to be read from the RX FIFO
  uint8_t *pByteIndex;          // Pointer to current position in the byte array
  bool start;                   // Start of Packet
  bool complete;                // Packet received complete
  uint8_t state;
  WmBusFrameMode framemode;
  WmBusFrameType frametype;
} RXinfoDescr;

typedef struct WMbusFrame {
  std::vector<unsigned char> frame{};
  WmBusFrameMode framemode;
  int8_t rssi;
  uint8_t lqi;
} WMbusFrame;

//----------------------------------------------------------------------------------
//  Function declarations
//----------------------------------------------------------------------------------
const std::string mode_to_string(WmBusFrameMode mode);

uint16_t verifyCrcBytesCmodeA_local(uint8_t* pByte, uint8_t* pPacket, uint16_t packetSize);

class rf_mbus {
  public:
    rf_mbus();
    bool init(uint8_t mosi, uint8_t miso, uint8_t clk, uint8_t cs,
      uint8_t gdo0, uint8_t gdo2);
          bool task();
    WMbusFrame get_frame();
    uint8_t MBpacket[291];


  private:
    uint8_t start(bool force = true);

    uint8_t gdo0{0};
    uint8_t gdo2{0};
    
    uint8_t MBbytes[584];
   

    WMbusFrame returnFrame;

    RXinfoDescr RXinfo;

    uint32_t sync_time_{0};
    uint8_t extra_time_{20};
    uint8_t max_wait_time_ = extra_time_;

};

#endif
