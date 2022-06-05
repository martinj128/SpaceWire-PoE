#ifndef _SPACEWIRE_H_
#define _SPACEWIRE_H_

#include "EthernetSW.h"

#define EOP_HEADER (uint8_t)0x2A // char '*'
#define EEP_HEADER (uint8_t)0b10 
#define MULTIPLE_FRAMES_HEADER (uint8_t)0x26 //char '&'
#define END_OF_FRAME (uint8_t) 0x2B //char '+'
#define CCODE_HEADER (uint8_t)0x23  //char '#'
#define SW_NCHAR_BUFFER_SIZE 128
#define SW_CCODE_BUFFER_SIZE 2
#define EOP (uint8_t)0x27  // char '

#define MSB_BITMASK 0xFF00
#define LSB_BITMASK 0x00FF
#define MSB_FRAME_INDEX 12
#define LSB_FRAME_INDEX 13
#define PAYLOAD_START_INDEX 15
#define SW_No1_HEADER_INDEX 14

void receiveSWPackets(esp_eth_handle_t, uint8_t*, uint32_t, void *);
void sendAndBuildSWPacket(uint8_t*, uint16_t, uint8_t, uint8_t*);                          //data buffer to send, size of data, type of data (N-Char, Broadcast Code)
void sendSWPackets(spaceWirePoEPackets_t);  
uint8_t processSWPacketHeader(uint8_t*);
void readSWPacket(uint8_t*);
void readSWCCode(uint8_t*);
void readSWPacketMultipleFrames(uint8_t*);
char* convertToCharArray(uint8_t*, uint16_t);
unsigned char* convertToCharArray2(uint8_t*, uint16_t);

#endif