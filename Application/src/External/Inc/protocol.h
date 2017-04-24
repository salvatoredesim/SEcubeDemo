/*
 * protocol.h
 *
 *  Created on: 31 dic 2016
 *      Author: raidenfox
 */

#ifndef APPLICATION_USER_EXTERNAL_INC_PROTOCOL_H_
#define APPLICATION_USER_EXTERNAL_INC_PROTOCOL_H_

#include "stm32f4xx_hal.h"
#include "cp2102.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 300

typedef enum{
	IDLERX = 0x00,
	TYPE_RECEIVED = 0x01,
	OPERATION_RECEIVED = 0x02,
	SOURCE_RECEIVED = 0x03,
	DEST_RECEIVED = 0x04,
	LEN_RECEIVED = 0x05,
	LEN_INCOMPLETE = 0x06,
	PAYLOAD_RECEIVED = 0x07,
	COMPLETED = 0x08
}CMD_Rx_Status;

// Type Codes
typedef enum{
	INFO = 0x00,
	CMD = 0x01
}MessageType;

// OP Codes
typedef enum{
	ID = 0x00,
	ATR = 0x01,
	PPS = 0x02,
	APDU = 0x03,
	APDU_DEBUG = 0x04,
	FPGA_FLASH = 0x05
}SLJ52_Command;

// SRC/DST Codes
typedef enum{
	MONITOR = 0x00,
	FPGA = 0x01,
	SMARTCARD = 0x02,
	uSD = 0x03
}Devices;

 /*
 * CMD Message
 * |  Type  | Operation |     Source |  Destination |   Size   |			Data			|
 * | 1 byte | 1 byte 	| 1 byte 	 | 	  1 byte    |  4 byte  | 		variable size       |
 */

typedef struct{
	uint8_t type;
	uint8_t operation;
	uint8_t source;
	uint8_t destination;
	uint16_t size;
	uint8_t data[MAX_PAYLOAD_SIZE];
}CMD_Message;

void P0_ParseMessage();
void P0_SmartcardExec(CMD_Message command);
void ReceiveFragment(CMD_Message command);

#endif /* APPLICATION_USER_EXTERNAL_INC_PROTOCOL_H_ */
