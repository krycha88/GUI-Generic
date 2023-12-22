#include "utils.hpp"
#include "crc.hpp"
#include "rf_mbus.hpp"
#include "mbus_packet.hpp"
#include "3outof6.hpp"
#include "tmode_rf_settings.hpp"

#include <ELECHOUSE_CC1101_SRC_DRV.h>

const std::string mode_to_string(WmBusFrameMode mode) {
  switch (mode) {
    case WMBUS_T1_MODE:
      return "T1";
    case WMBUS_C1_MODE:
      return "C1";
    default:
      return "unknown";
  }
}

rf_mbus::rf_mbus() {};

uint8_t rf_mbus::start(bool force) {
  // waiting to long for next part of data?
  bool reinit_needed = ((millis() - sync_time_) > max_wait_time_) ? true: false;

  if (!force) {
    if (!reinit_needed) {
      // already in RX?
      if (ELECHOUSE_cc1101.SpiReadStatus(CC1101_MARCSTATE) == MARCSTATE_RX) {
        return 0;
      }
    }
  }

  // init RX here, each time we're idle
  RXinfo.state = 0;
  sync_time_ = millis();
  max_wait_time_ = extra_time_;

  int retries = 10;
  ELECHOUSE_cc1101.SpiStrobe(CC1101_SIDLE);
  while((ELECHOUSE_cc1101.SpiReadStatus(CC1101_MARCSTATE) != MARCSTATE_IDLE && retries-- > 0)) {
    delay(1);
  };
  ELECHOUSE_cc1101.SpiStrobe(CC1101_SFTX);  //flush TXfifo
  ELECHOUSE_cc1101.SpiStrobe(CC1101_SFRX);  //flush RXfifo

  // Initialize RX info variable
  RXinfo.lengthField = 0;              // Length Field in the wireless MBUS packet
  RXinfo.length      = 0;              // Total length of bytes to receive packet
  RXinfo.bytesLeft   = 0;              // Bytes left to to be read from the RX FIFO
  RXinfo.pByteIndex  = this->MBbytes;  // Pointer to current position in the byte array
  RXinfo.complete    = false;          // Packet Received
  RXinfo.framemode   = WMBUS_UNKNOWN_MODE;
  RXinfo.frametype   = WMBUS_FRAME_UNKNOWN;

  memset(this->MBbytes, 0, sizeof(this->MBbytes));
  memset(this->MBpacket, 0, sizeof(this->MBpacket));
  this->returnFrame.frame.clear();
  this->returnFrame.rssi = 0;
  this->returnFrame.lqi = 0;
  this->returnFrame.framemode = WMBUS_UNKNOWN_MODE;

  // Set RX FIFO threshold to 4 bytes
  ELECHOUSE_cc1101.SpiWriteReg(CC1101_FIFOTHR, RX_FIFO_START_THRESHOLD);
  // Set infinite length 
  ELECHOUSE_cc1101.SpiWriteReg(CC1101_PKTCTRL0, INFINITE_PACKET_LENGTH);

  ELECHOUSE_cc1101.SpiStrobe(CC1101_SRX);
  while((ELECHOUSE_cc1101.SpiReadStatus(CC1101_MARCSTATE) != MARCSTATE_RX && retries-- > 0)) {
    delay(1);
  };

  RXinfo.state = 1;

  return 1; // this will indicate we just have re-started RX
}

WMbusFrame rf_mbus::get_frame() {
  uint8_t len_without_crc = crcRemove(this->MBpacket, packetSize(this->MBpacket[0]));
  std::vector<unsigned char> frame(this->MBpacket, this->MBpacket + len_without_crc);
  this->returnFrame.frame = frame;
  return this->returnFrame;
}

uint16_t verifyCrcBytesCmodeA_local(uint8_t* pByte, uint8_t* pPacket, uint16_t packetSize)
{
  uint16_t crc = 0;
  uint16_t i = 0;

  bool crcNotOk = false;

  Serial.print("   ");
  while (i < 10) {
    Serial.printf("%02X", pByte[i]);
    crc = crcCalc(crc, pByte[i]);
    pPacket[i] = pByte[i];
    ++i;
  }
  Serial.printf(" %04X [%02X%02X] ", crc, pByte[i], pByte[i + 1]);

  if ((~crc) != (pByte[i] << 8 | pByte[i + 1])) {
    crcNotOk = true;
  }

  pPacket[i] = pByte[i];
  ++i;
  pPacket[i] = pByte[i];
  ++i;
  crc = 0;

  int cycles = (packetSize - 12) / 18;
  int myRun = 2;
  Serial.print("   ");
  while (cycles > 0) {
    for (int j = 0; j < 16; ++j) {
      Serial.printf("%02X", pByte[i]);
      crc = crcCalc(crc, pByte[i]);
      pPacket[i] = pByte[i];
      ++i;
    }
    Serial.printf(" %04X [%02X%02X] ", crc, pByte[i], pByte[i + 1]);

    myRun++;
    if ((~crc) != (pByte[i] << 8 | pByte[i + 1])) {
      crcNotOk = true;
    }

    pPacket[i] = pByte[i];
    ++i;
    pPacket[i] = pByte[i];
    ++i;
    crc = 0;

    --cycles;
  }

  if (i == packetSize) {
    return (PACKET_OK);
  }

  Serial.print("   ");
  while (i < packetSize - 2) {
    Serial.printf("%02X", pByte[i]);
    crc = crcCalc(crc, pByte[i]);
    pPacket[i] = pByte[i];
    ++i;
  }

  Serial.printf(" %04X [%02X%02X] ", crc, pByte[i], pByte[i + 1]);

  if ((~crc) != (pByte[i] << 8 | pByte[i + 1])) {
    crcNotOk = true;
  }

  pPacket[i] = pByte[i];
  ++i;
  pPacket[i] = pByte[i];
  ++i;

  Serial.println("");
  if (crcNotOk) {
    return (PACKET_CRC_ERROR);
  }
  else {
    return (PACKET_OK);
  }
}

bool rf_mbus::init(uint8_t mosi, uint8_t miso, uint8_t clk, uint8_t cs, uint8_t gdo0, uint8_t gdo2) {
  bool retVal = false;
  Serial.println("");
  this->gdo0 = gdo0;
  this->gdo2 = gdo2;
  pinMode(this->gdo0, INPUT);
  pinMode(this->gdo2, INPUT);
  ELECHOUSE_cc1101.setSpiPin(clk, miso, mosi, cs);

  ELECHOUSE_cc1101.Init();

  for (uint8_t i = 0; i < TMODE_RF_SETTINGS_LEN; i++) {
    ELECHOUSE_cc1101.SpiWriteReg(TMODE_RF_SETTINGS_BYTES[i << 1],
                                 TMODE_RF_SETTINGS_BYTES[(i << 1) + 1]);
  }

  ELECHOUSE_cc1101.SpiStrobe(CC1101_SCAL);

  byte cc1101Version = ELECHOUSE_cc1101.SpiReadStatus(CC1101_VERSION);

  if ((cc1101Version != 0) && (cc1101Version != 255)) {
    retVal = true;
    Serial.print("wMBus-lib: CC1101 version '");
    Serial.print(cc1101Version);
    Serial.println("'");
    ELECHOUSE_cc1101.SetRx();
    Serial.println("wMBus-lib: CC1101 initialized");
    memset(&RXinfo, 0, sizeof(RXinfo));
    delay(4);
  }
  else {
    Serial.println("wMBus-lib: CC1101 initialization FAILED!");
  }

  return retVal;
}

bool rf_mbus::task() {
  uint8_t bytesDecoded[2];

  switch (RXinfo.state) {
    case 0:
      {
        start();
      }
      return false;

     // RX active, awaiting SYNC
    case 1:
      if (digitalRead(this->gdo2)) {
        RXinfo.state = 2;
        sync_time_ = millis();
      }
      break;

    // awaiting pkt len to read
    case 2:
      if (digitalRead(this->gdo0)) {
        // Read the 3 first bytes
        ELECHOUSE_cc1101.SpiReadBurstReg(CC1101_RXFIFO, RXinfo.pByteIndex, 3);

        // - Calculate the total number of bytes to receive -

        // In C-mode we allow receiving T-mode because they are similar. To not break any applications using T-mode,
        // we do not include results from C-mode in T-mode.

        // If T-mode preamble and sync is used, then the first data byte is either a valid 3outof6 byte or C-mode
        // signaling byte. (http://www.ti.com/lit/an/swra522d/swra522d.pdf#page=6)
        if (RXinfo.pByteIndex[0] == 0x54) {
          RXinfo.framemode = WMBUS_C1_MODE;
          // If we have determined that it is a C-mode frame, we have to determine if it is Type A or B.
          if (RXinfo.pByteIndex[1] == 0xCD) {
            RXinfo.frametype = WMBUS_FRAMEA;

            // Frame format A
            RXinfo.lengthField = RXinfo.pByteIndex[2];

            if (RXinfo.lengthField < 9) {
              RXinfo.state = 0;
              return false;
            }

            // Number of CRC bytes = 2 * ceil((L-9)/16) + 2
            // Preamble + L-field + payload + CRC bytes
            RXinfo.length = 2 + 1 + RXinfo.lengthField + 2 * (2 + (RXinfo.lengthField - 10)/16);
          } else if (RXinfo.pByteIndex[1] == 0x3D) {
            RXinfo.frametype = WMBUS_FRAMEB;
            // Frame format B
            RXinfo.lengthField = RXinfo.pByteIndex[2];

            if (RXinfo.lengthField < 12 || RXinfo.lengthField == 128) {
              RXinfo.state = 0;
              return false;
            }

            // preamble + L-field + payload
            RXinfo.length = 2 + 1 + RXinfo.lengthField;
          } else {
            // Unknown type, reset.
            RXinfo.state = 0;
            return false;
          }
        // T-Mode
        // Possible improvment: Check the return value from the deocding function,
        // and abort RX if coding error.
        } else if (decode3outof6(RXinfo.pByteIndex, bytesDecoded, 0) != DECODING_3OUTOF6_OK) {
          RXinfo.state = 0;
          return false;
        } else {
          RXinfo.framemode = WMBUS_T1_MODE;
          RXinfo.frametype = WMBUS_FRAMEA;
          RXinfo.lengthField = bytesDecoded[0];
          RXinfo.length = byteSize(packetSize(RXinfo.lengthField));
        }

        // check if incoming data will fit into buffer
        if (RXinfo.length>sizeof(this->MBbytes)) {
          RXinfo.state = 0;
          return false;
        }

        // we got the length: now start setup chip to receive this much data
        // - Length mode -
        ELECHOUSE_cc1101.SpiWriteReg(CC1101_PKTLEN, (uint8_t)(RXinfo.length));
        ELECHOUSE_cc1101.SpiWriteReg(CC1101_PKTCTRL0, FIXED_PACKET_LENGTH);

        RXinfo.pByteIndex += 3;
        RXinfo.bytesLeft   = RXinfo.length - 3;

        RXinfo.state = 3;
        max_wait_time_ += extra_time_;

        ELECHOUSE_cc1101.SpiWriteReg(CC1101_FIFOTHR, RX_FIFO_THRESHOLD);
      }
      break;

    // awaiting more data to be read
    case 3:
      if (digitalRead(this->gdo0)) {
        // Read out the RX FIFO
        // Do not empty the FIFO (See the CC110x or 2500 Errata Note)
        uint8_t bytesInFIFO = ELECHOUSE_cc1101.SpiReadStatus(CC1101_RXBYTES) & 0x7F;        
        ELECHOUSE_cc1101.SpiReadBurstReg(CC1101_RXFIFO, RXinfo.pByteIndex, bytesInFIFO - 1);

        RXinfo.bytesLeft  -= (bytesInFIFO - 1);
        RXinfo.pByteIndex += (bytesInFIFO - 1);

        max_wait_time_ += extra_time_;
      }
      break;
  }

  uint8_t overfl = ELECHOUSE_cc1101.SpiReadStatus(CC1101_RXBYTES) & 0x80;
  // END OF PAKET
  if ((!overfl) && (!digitalRead(gdo2)) && (RXinfo.state > 1)) {
    ELECHOUSE_cc1101.SpiReadBurstReg(CC1101_RXFIFO, RXinfo.pByteIndex, (uint8_t)RXinfo.bytesLeft);

    // decode
    uint16_t rxStatus = -1;
    uint16_t rxLength = 0;

    if (RXinfo.framemode == WMBUS_T1_MODE) {
      Serial.println("wMBus-lib: Processing T1 A frame");
      rxStatus = decodeRXBytesTmode(this->MBbytes, this->MBpacket, packetSize(RXinfo.lengthField));
      rxLength = packetSize(this->MBpacket[0]);
    } else if (RXinfo.framemode == WMBUS_C1_MODE) {
      if (RXinfo.frametype == WMBUS_FRAMEA) {
        Serial.println("wMBus-lib: Processing C1 A frame");
        Serial.print(" FullFrame: ");
        for (int ii=0; ii < RXinfo.length; ii++) {
          Serial.printf("%02X", (int)(this->MBbytes[ii]));
        }
        Serial.println("");

        // 2 + 1 + RXinfo.lengthField + 2 * (2 + (RXinfo.lengthField - 10)/16);
        rxLength = RXinfo.lengthField + 2 * (2 + (RXinfo.lengthField - 10)/16) + 1;

        rxStatus = verifyCrcBytesCmodeA_local(this->MBbytes + 2, this->MBpacket, rxLength);
        // small cheat
        rxStatus = PACKET_OK;
      } else if (RXinfo.frametype == WMBUS_FRAMEB) {
        Serial.println("wMBus-lib: Processing C1 B frame -- NOT supported yet");
        rxStatus = -1;
        Serial.print(" FullFrame: ");
        for (int ii=0; ii < RXinfo.length; ii++) {
          Serial.printf("%02X", (int)(this->MBbytes[ii]));
        }
        Serial.println("");
      }
    }

    if (rxStatus == PACKET_OK) {
      this->returnFrame.framemode = RXinfo.framemode;
      RXinfo.complete = true;
      this->returnFrame.rssi = (int8_t)ELECHOUSE_cc1101.getRssi();
      this->returnFrame.lqi = (uint8_t)ELECHOUSE_cc1101.getLqi();
    }
    else if (rxStatus == PACKET_CODING_ERROR) {
      Serial.println("wMBus-lib:  Error during decoding '3 out of 6'");
    }
    else if (rxStatus == PACKET_CRC_ERROR) {
      Serial.println("wMBus-lib:  Error during decoding 'CRC'");
    }
    else {
      Serial.println("wMBus-lib:  Error during decoding 'unknown'");
    }
    RXinfo.state = 0;
    return RXinfo.complete;
  }
  start(false);

  return RXinfo.complete;
}
