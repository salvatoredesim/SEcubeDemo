/*
 * smartcard.h
 *
 *  Created on: 21 dic 2016
 *      Author: Salvatore
 */

#ifndef APPLICATION_USER_SMARTCARD_H_
#define APPLICATION_USER_SMARTCARD_H_

#include "stm32f4xx_hal.h"

/* Costants */
#define MAX_ATR_SIZE	   40
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define ATR_SIZE		   24

#define INIT_ETU_CLK_CICLES		250000
#define WORK_ETU_CLK_CICLES		25*250000
#define INIT_ETU 		   22
#define WORK_ETU 		   7

#define MAX_APDU_DATA_SIZE 256

/* INS Codes */
#define ACTIVATE_FILE						0x44
#define APPEND_RECORD       				0xE2
#define CHANGE_REFERENCE_DATA 				0x24
#define CREATE_FILE							0xE0
#define DEACTIVATE_FILE						0x04
#define DELETE_FILE							0xE4
#define DISABLE_VERIFICATION_REQUIREMENT	0x26
#define ENABLE_VERIFICATION_REQUIREMENT		0x28
#define ENVELOPE1							0xC2
#define ENVELOPE2							0xC3
#define ERASE_BINARY_1						0x0E
#define ERASE_BINARY_2						0x0F
#define ERASE_RECORD_S						0x0C
#define EXTERNAL_MUTUAL_AUTHENTICATE		0x82
#define GENERAL_AUTHENTICATE1				0x86
#define GENERAL_AUTHENTICATE2				0x87
#define GENERATE_ASYMMETRIC_KEY_PAIR		0x46
#define GET_CHALLENGE						0x84
#define GET_DATA1							0xCA
#define GET_DATA2							0xCB
#define GET_RESPONSE						0xC0
#define INTERNAL_AUTHENTICATE				0x88
#define MANAGE_CHANNEL						0x70
#define	MANAGE_SECURITY_ENVIRONMENT			0x22
#define PERFORM_SCQL_OPERATION				0x10
#define PERFORM_SECURITY_OPERATION			0x2A
#define PERFORM_TRANSACTION_OPERATION		0x12
#define PERFORM_USER_OPERATION				0x14
#define PUT_DATA1							0xDA
#define PUT_DATA2							0xDB
#define READ_BINARY1						0xB0
#define	READ_BINARY2						0xB1
#define READ_RECORD_S_1						0xB2
#define READ_RECORD_S_2						0xB3
#define	RESET_RETRY_COUNTER					0x2C
#define SEARCH_BINARY1						0xA0
#define SEARCH_BINARY2						0xA1
#define SEARCH_RECORD						0xA2
#define SELECT								0xA4
#define TERMINATE_CARD_USAGE				0xFE
#define TERMINATE_DF						0xE6
#define TERMINATE_EF						0xE8
#define UPDATE_BINARY1						0xD6
#define UPDATE_BINARY2						0xD7
#define UPDATE_RECORD1						0xDC
#define UPDATE_RECORD2						0xDD
#define VERIFY1								0x20
#define VERIFY2								0x21
#define WRITE_BINARY0						0xD0
#define WRITE_BINARY1						0xD1
#define WRITE_RECORD						0xD2

/* Control Flags */
#define INTERRUPT_MODE 		0
#define PROTOCOL_READY		0

typedef enum
{
  PW_DOWN = GPIO_PIN_RESET,
  PW_UP = GPIO_PIN_SET
}PowerState;

typedef enum{
	LOW_RESET = GPIO_PIN_RESET,
	HIGH_RESET = GPIO_PIN_SET
}ResetState;

// Card states
typedef enum{
	CARD_UNKNOWN = 0x00,
	CARD_ACTIVATION = 0x01,
	COLD_RESET = 0x02,
	ATR_RESPONSE = 0x03,
	ATR_DECODED = 0x04,
	PTS_REQUEST_SENT = 0x05,
	PTS_RESPONSE = 0x06,
	PTS_EXCHANGED = 0x07
}SLJ52States;

// APDU States
typedef enum{
	IDLE = 0x00,
	SENT = 0x01,
	RECEIVED = 0x02
} APDU_State;

// APDU Execution states
typedef enum{
	APDU_EXECUTION_OK = 0x01,
	APDU_EXECUTION_WARNING = 0x02,
	APDU_EXECUTION_ERROR = 0x03,
	APDU_CHECKING_ERROR = 0x04,
	APDU_UNKNOWN_ERROR = 0x05
} APDU_Execution_State;

// APDU structure
typedef struct{
	uint8_t CLA;
	uint8_t INS;
	uint8_t P1;
	uint8_t P2;
	uint8_t LC;
	uint8_t Data[MAX_APDU_DATA_SIZE];
	uint8_t LE;
}SLJ52_APDU;

// APDU Response structure
typedef struct{
	uint8_t SW1;
	uint8_t SW2;
	uint8_t Data[1024];
}SLJ52_APDU_Response;

// Parsed ATR structure
typedef struct {
	uint8_t TS;
	uint8_t T0;
	uint8_t T[SETUP_LENGTH];
	uint8_t H[HIST_LENGTH];
	uint8_t Tlength;
	uint8_t Hlength;
	uint8_t CheckSum;
}SLJ52ATR;

// PPS States
typedef struct{
	uint8_t PTSS;
	uint8_t PTS0;
	uint8_t PTS1;
	uint8_t PTS2;
	uint8_t PTS3;
	uint8_t PCK;
}PPSType;


/**
 * @brief  Power on/off the Smartcard
 * @param  status: The ON/OFF state
 */
void SLJ52_PWR(PowerState status);


/**
 * @brief  Reset the card
 * @param  rst: the reset is low or high, depends on the operation you want to do
 */
void SLJ52_RST(ResetState rst);


/**
 * @brief  Initialize the SLJ52G Card, manage params and switch speeds
 */
void SLJ52_Init();


/**
 * @brief  Change the speed based on the negotiated params with the serial interface
 */
void SLJ52_Interface_Configuration();


/**
 * @brief  Get the ATR
 */
SLJ52ATR SLJ52_GetATR();


/**
 * @brief  Parse the ATR and analyze all the information to manage switch of speed
 */
void SLJ52_ATR_Parse();

/**
 * @brief  Operate the Parameter To Selection on the card
 */
void SLJ52_PPS();


/**
 * @brief  Create an Empty APDU
 */
SLJ52_APDU SLJ52_Empty_APDU();

/**
 * @brief  Send an APDU to the Card
 * @param  APDU: the APDU you want to send
 * @param  size: the size of response
 * @param  APDUResp: the response to the submitted APDU
 */
void SLJ52_SendAPDU(SLJ52_APDU APDU, uint8_t * size,SLJ52_APDU_Response* APDUResp);

// END

// NOT STABLE FUNCTIONS - IGNORE
//void SLJ52_SendAPDU(SLJ52_APDU APDU,uint8_t* size, uint8_t* Response);
//void SLJ52_SendAPDU_TimeExec(SLJ52_APDU APDU, uint8_t * size,SLJ52_APDU_Response* APDUResp);
#endif /* APPLICATION_USER_SMARTCARD_H_ */
