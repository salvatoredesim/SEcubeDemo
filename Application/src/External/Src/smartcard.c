/*
 * smartcard.c
 *
 *  Created on: 21 dic 2016
 *      Author: Salvatore
 */

#include "smartcard.h"
#include "cp2102.h"

extern SMARTCARD_HandleTypeDef hsc6;

uint8_t SLJ52_ATR[MAX_ATR_SIZE] = {0x00};
PPSType PPS_Request;
PPSType PPS_Response;
SLJ52ATR ATR_Response;
uint8_t FailedByte = 0;
SLJ52States status_protocol = CARD_UNKNOWN;
APDU_Execution_State APDU_Execution = APDU_EXECUTION_OK;
uint8_t isTxReady = 0,isRxReady = 0;

uint32_t F_Table[16] = {372,372,558,744,1116,1488,1860,0,0,512,768,1024,1536,2048,0,0};
uint32_t D_Table[16] = {0,1,2,4,8,16,32,64,12,20,0,0,0,0,0,0};

/*
void ErrorsDetect(){
    uint32_t txe = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_TXE);
    uint32_t tc = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_TC);
    uint32_t rxne = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_RXNE);
    uint32_t idle = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_IDLE);
    uint32_t overrun = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_ORE);
    uint32_t noise = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_NE);
    uint32_t frame_err = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_FE);
    uint32_t parity_err = __HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_PE);
    return ;
}*/

void SLJ52_PWR(PowerState status){
	if(status != PW_UP)
		HAL_GPIO_WritePin(SC_ON_OFF_GPIO_Port,SC_ON_OFF_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SC_ON_OFF_GPIO_Port,SC_ON_OFF_Pin,GPIO_PIN_SET);
}


void SLJ52_RST(ResetState rst){
	if(rst != LOW_RESET){
		HAL_GPIO_WritePin(SC_RST_GPIO_Port,SC_RST_Pin,GPIO_PIN_SET);
	}else
		HAL_GPIO_WritePin(SC_RST_GPIO_Port,SC_RST_Pin,GPIO_PIN_RESET);
}

void SLJ52_Init(){
	SLJ52_RST(LOW_RESET);
	SLJ52_PWR(PW_UP);
	SLJ52_Interface_Configuration();
	for(int i = 0; i < 200; i++);
#if PROTOCOL_READY
	while(getbyte(FailedByte) == 0);
#endif
	SLJ52_RST(HIGH_RESET);
	status_protocol = COLD_RESET;
	SLJ52_ATR_Parse();
	SLJ52_PPS();
}

void SLJ52_Interface_Configuration() {
	hsc6.Instance = USART6;
	hsc6.Init.BaudRate = 11290;
	hsc6.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
	hsc6.Init.StopBits = SMARTCARD_STOPBITS_1_5;
	hsc6.Init.Parity = SMARTCARD_PARITY_EVEN;
	hsc6.Init.Mode = SMARTCARD_MODE_TX_RX;
	hsc6.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
	hsc6.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
	hsc6.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
	hsc6.Init.Prescaler = SMARTCARD_PRESCALER_SYSCLK_DIV20;
	hsc6.Init.GuardTime = 16;
	hsc6.Init.NACKState = SMARTCARD_NACK_DISABLED;
	while(HAL_SMARTCARD_Init(&hsc6) != HAL_OK){

	}
	status_protocol = CARD_ACTIVATION;
}

void SLJ52_TransmitByte(uint8_t byte){
	FailedByte = byte;
#if INTERRUPT_MODE
	HAL_SMARTCARD_Transmit_IT(&hsc6, &byte, 1);
	while(isTxReady == 0){

	}
	isTxReady = 0;
#else
	while(__HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_TXE)== RESET);
	HAL_SMARTCARD_Transmit(&hsc6,&byte,1,0);
	while(__HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_TC)== RESET);

#endif
}

void SLJ52_ReceiveByte(uint8_t* byte){
#if INTERRUPT_MODE
	HAL_SMARTCARD_Receive_IT(&hsc6, byte, 1);
	while(isRxReady == 0){

	}
	isRxReady = 0;
#else
	uint32_t counter = 16800000;
	while(__HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_RXNE) == RESET && counter > 0){counter--;}
	HAL_SMARTCARD_Receive(&hsc6,byte,1,0);
#endif
}

SLJ52ATR SLJ52_GetATR(){
	return ATR_Response;
}

void SLJ52_ATR_Parse(){
	uint32_t i = 0, flag = 0, buf = 0, protocol = 0;
	for(int i = 0; i < ATR_SIZE ; i++){
			SLJ52_ReceiveByte(&SLJ52_ATR[i]);
#if PROTOCOL_READY
			putbyte(SLJ52_ATR[i]);
#endif
		}
	while(__HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_IDLE)== RESET);
	if (SLJ52_ATR[0] != 0x00) {
		ATR_Response.TS = SLJ52_ATR[0];
		ATR_Response.T0 = SLJ52_ATR[1];

		ATR_Response.Hlength = ATR_Response.T0 & (uint8_t) 0x0F;

		if ((ATR_Response.T0 & (uint8_t) 0x80) == 0x80) {
			flag = 1;
		}

		for (i = 0; i < 4; i++) {
			ATR_Response.Tlength = ATR_Response.Tlength
					+ (((ATR_Response.T0 & (uint8_t) 0xF0) >> (4 + i))
							& (uint8_t) 0x1);
		}

		for (i = 0; i < ATR_Response.Tlength; i++) {
			ATR_Response.T[i] = SLJ52_ATR[i + 2];
		}

		if ((ATR_Response.T0 & (uint8_t) 0x80) == 0x00) {
			protocol = 0;
		} else {
			protocol = ATR_Response.T[ATR_Response.Tlength - 1]
					& (uint8_t) 0x0F;
		}

		while (flag) {
			if ((ATR_Response.T[ATR_Response.Tlength - 1] & (uint8_t) 0x80)
					== 0x80) {
				flag = 1;
			} else {
				flag = 0;
			}

			buf = ATR_Response.Tlength;
			ATR_Response.Tlength = 0;

			for (i = 0; i < 4; i++) {
				ATR_Response.Tlength = ATR_Response.Tlength
						+ (((ATR_Response.T[buf - 1] & (uint8_t) 0xF0)
								>> (4 + i)) & (uint8_t) 0x1);
			}

			for (i = 0; i < ATR_Response.Tlength; i++) {
				ATR_Response.T[buf + i] = SLJ52_ATR[i + 2 + buf];
			}
			ATR_Response.Tlength += (uint8_t) buf;
		}

		for (i = 0; i < ATR_Response.Hlength; i++) {
			ATR_Response.H[i] = SLJ52_ATR[i + 2 + ATR_Response.Tlength];
		}
		ATR_Response.CheckSum = SLJ52_ATR[ATR_SIZE-1];
	}
	status_protocol = ATR_DECODED;
}

void SLJ52_PPS(){
	while(__HAL_SMARTCARD_GET_FLAG(&hsc6,SMARTCARD_FLAG_IDLE) == RESET);
	uint8_t Request[4] = {0x00};

	// Define the PTS Struct
	PPS_Request.PTSS = 0xFF;
	PPS_Request.PTS0 = 0x10;
	PPS_Request.PTS1 = ATR_Response.T[0];
	PPS_Request.PCK = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)0x18;

	Request[0] = PPS_Request.PTSS;
	Request[1] = PPS_Request.PTS0;
	Request[2] = PPS_Request.PTS1;
	Request[3] = PPS_Request.PCK;

	uint8_t Response[4] = {0x00};
	uint8_t flush;
	for(int i = 0; i < 4; i++){
		SLJ52_TransmitByte(Request[i]);
		SLJ52_ReceiveByte(&flush);
    }

	for(int i = 0; i < 4; i++){
		SLJ52_ReceiveByte(&Response[i]);
#if PROTOCOL_READY
		putbyte(Response[i]);
#endif
	}
	Request[0] = 0xFF;

	uint8_t flag = 1;
	for(int i = 0; i < 4; i++){
		if(Request[i] != Response[i]){
			flag = 0;
		}
	}

	if(flag == 1){
		// Set tehe PPS response
		PPS_Response.PTSS = Response[0];
		PPS_Response.PTS0 = Response[1];
		PPS_Response.PTS1 = Response[2];
		PPS_Response.PCK = Response[3];

		// Calculate the F and D Values
		uint8_t F = (ATR_Response.T[0]&0xF0) >> 4;
		uint8_t D = (ATR_Response.T[0]&0x0F);

		// Obtain the prescaled frequency
		uint32_t apb2_freq = HAL_RCC_GetPCLK2Freq();
		uint32_t prescaled_freq = (apb2_freq/((USART6->GTPR)&(uint16_t)0x00FF)/2);

		// Calculate the new baudrate
		uint32_t baudrate = (prescaled_freq*D_Table[D])/F_Table[F];

		//Set the new baudarate
		hsc6.Instance = USART6;
		hsc6.Init.BaudRate = baudrate;
		hsc6.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
		hsc6.Init.StopBits = SMARTCARD_STOPBITS_1_5;
		hsc6.Init.Parity = SMARTCARD_PARITY_EVEN;
		hsc6.Init.Mode = SMARTCARD_MODE_TX_RX;
		hsc6.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
		hsc6.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
		hsc6.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
		hsc6.Init.Prescaler = SMARTCARD_PRESCALER_SYSCLK_DIV20;
		hsc6.Init.GuardTime = 16;
		hsc6.Init.NACKState = SMARTCARD_NACK_DISABLED;
		while(HAL_SMARTCARD_Init(&hsc6) != HAL_OK){

		}
		status_protocol = PTS_EXCHANGED;
	}
}

/* TODO: Take the times of APDU exec
void SLJ52_SendAPDU_TimeExec(SLJ52_APDU APDU, uint8_t * size,SLJ52_APDU_Response* APDUResp){
	uint32_t start_tick = HAL_GetTick();
	SLJ52_SendAPDU2(APDU,size,APDUResp);
	uint32_t end_tick = HAL_GetTick();
	uint32_t duration = end_tick - start_tick;
	uint8_t duration4byte[4] = {0x00};
	memcpy(duration4byte,(uint8_t*)&duration,4);
	if(*size == 2){
	memcpy((APDUResp->Data)+(2-(*size)),(uint8_t*)&duration,4);
	*size = (*size)+2;
	}
	else{
	memcpy((APDUResp->Data)+((*size)),(uint8_t*)&duration,4);
	uint8_t* ptr = APDUResp->Data;
	//*APDUResp->Data[2-(*size)] =
	*size = (*size)+4;
	}
	int x = 0;


}
*/

void SLJ52_SendAPDU(SLJ52_APDU APDU, uint8_t * size,SLJ52_APDU_Response* APDUResp){
	// Buffer for sending and receivin the APDUs
	uint8_t Response[300] = {0x00};
	uint8_t flush[300] = {0x00};
	flush[4] = 0xFF;
	flush[5] = 0xFF;
	uint8_t* APDU_ADRR = (uint8_t*)&APDU;
	APDUResp->SW1 = 0x00;
	APDUResp->SW2 = 0x00;
	for(int i = 0; i < 1024; i++)
		APDUResp->Data[i] = 0x00;

	// Send header of the APDU
	for(int i = 0; i < 4; i++){
		SLJ52_TransmitByte(*(APDU_ADRR+i));
		SLJ52_ReceiveByte(&flush[i]);
	}

	if(APDU.LC){
		/* If Lc != 0 we are on the 3rd case */
		SLJ52_TransmitByte(APDU.LC);
		SLJ52_ReceiveByte(&flush[4]);
	}else if(APDU.LE){
		/* If Le != 0 we are on the 2nd case */
		SLJ52_TransmitByte(APDU.LE);
		SLJ52_ReceiveByte(&flush[5]);
	}else{
		/* If Lc == 0 and Le == 0 we are on the 1st case
		 * so we send 0x00 as P3 */
		SLJ52_TransmitByte(0x00);
		SLJ52_ReceiveByte(&flush[4]);
	}

	/* Receive the status byte */
	SLJ52_ReceiveByte(&Response[0]);

	if(((Response[0] & (uint8_t)0xF0) == 0x60) || ((Response[0] & (uint8_t)0xF0) == 0x90)){
		APDUResp->SW1 = Response[0];
		SLJ52_ReceiveByte(&Response[1]);
		APDUResp->SW2 = Response[1];
		*size = 2;
	}
	else if(((Response[0] & (uint8_t)0xFE) == (((uint8_t)~(APDU.INS)) & (uint8_t)0xFE)) || \
	    ((Response[0] & (uint8_t)0xFE) == (APDU.INS & (uint8_t)0xFE))){
		APDUResp->Data[0] = Response[0];
		*size = 1;
	}

	/* If no status bytes received ---------------------------------------------*/
	  if(APDUResp->SW1 == 0x00)
	  {
	    /* Send body data to SC --------------------------------------------------*/
	    if(APDU.LC)
	    {
	      /* Send body data */
	      for(int i = 0; i < APDU.LC; i++){
	    	SLJ52_TransmitByte(*(APDU_ADRR+5+i));
	    	SLJ52_ReceiveByte(&flush[5+i]);
	      }
	      SLJ52_ReceiveByte(&Response[0]);
	      SLJ52_ReceiveByte(&Response[1]);

	      /* Decode the SW1 */
	      APDUResp->SW1 = Response[0];

	      /* Decode the SW2 */
	      APDUResp->SW2 = Response[1];
	      *size = 2;
	    }

	    /* Or receive body data from SC ------------------------------------------*/
	    else if(APDU.LE)
	    {
	      /* Start the receive IT process: to receive the command answer from the card */
	      for(int i = 0; i < APDU.LE+2; i++){
		      SLJ52_ReceiveByte(&Response[i]);

	      }

	      /* Decode the body data */
	      for(int i = 0; i < APDU.LE; i++)
	      {
	    	  APDUResp->Data[i] = Response[i];
	      }

	      /* Decode the SW1 */
	      APDUResp->SW1 = Response[APDU.LE];

	      /* Decode the SW2 */
	      APDUResp->SW2 = Response[APDU.LE+1];
	      *size = APDU.LE;
	    }
	  }

}

/*
void SLJ52_SendAPDU(SLJ52_APDU APDU, uint8_t * size,uint8_t* Response){
	// Buffer for sending and receivin the APDUs
	uint8_t flush[100] = {0x00};
	uint8_t* APDU_ADRR = (uint8_t*)&APDU;

	// Sending CLA INS P1 P2 P3
	for(int i = 0; i < 5; i++){
		SLJ52_TransmitByte(*(APDU_ADRR+i));
		SLJ52_ReceiveByte(&flush[i]);
#if PROTOCOL_READY
		putbyte(*(APDU_ADRR+i));
#endif
	}

	// Receive the status byte
	SLJ52_ReceiveByte(&Response[0]);

#if PROTOCOL_READY
	putbyte(Response[0]);
#endif

	// Validating status byte
	if((Response[0] >= 0x60) &&(Response[0] <= 0x6F)){
		if(Response[0] == 0x90 || Response[0] == 0x61){
			APDU_Execution = APDU_EXECUTION_OK;
		}else if(Response[0] == 0x62 || Response[0] == 0x63){
			APDU_Execution = APDU_EXECUTION_WARNING;
		}else if(Response[0] >= 0x64 || Response[0] <= 0x66){
			APDU_Execution = APDU_EXECUTION_ERROR;
		}else{
			APDU_Execution = APDU_CHECKING_ERROR;
		}
	}else{
		APDU_Execution = APDU_UNKNOWN_ERROR;
	}

	switch(APDU_Execution){
		case APDU_EXECUTION_OK:
			if(Response[0] == 0x61){

				// Get size of data in GET_RESPONSE INS
				SLJ52_ReceiveByte(&Response[1]);
				uint8_t size_content = Response[1];
				*size = size_content+5;
				// Modify APDU with the GET_RESPONSE APDU and Transmit it
				APDU_ADRR = (uint8_t*)&APDU;
				APDU.INS = GET_RESPONSE;
				APDU.P1 = 0x00;
				APDU.P2 = 0x00;
				APDU.LE = size_content;
				// Sending CLA INS P1 P2 P3
				for(int i = 0; i < 4; i++){
					SLJ52_TransmitByte(*(APDU_ADRR+i));
					SLJ52_ReceiveByte(&flush[i]);
				}

				SLJ52_TransmitByte(APDU.LE);
				SLJ52_ReceiveByte(&flush[4]);
				// Receive the status byte
				for(int i = 0; i < size_content+3; i++){
					SLJ52_ReceiveByte(&Response[i+2]);
				}
			}
			break;
		case APDU_EXECUTION_WARNING:
			break;
		case APDU_EXECUTION_ERROR:
			// Receive the status byte
			SLJ52_ReceiveByte(&Response[1]);
			*size = 2;
			break;
		case APDU_CHECKING_ERROR:
			SLJ52_ReceiveByte(&Response[1]);
			*size = 2;
			break;
		case APDU_UNKNOWN_ERROR:
			for(int i = 0; i < APDU.LC; i++){
				SLJ52_TransmitByte(*(APDU_ADRR+5+i));
				SLJ52_ReceiveByte(&flush[5+i]);
			}
			SLJ52_ReceiveByte(&Response[1]);
			SLJ52_ReceiveByte(&Response[2]);
			*size = 3;
			break;
		default:
			break;
	}

}
*/

SLJ52_APDU SLJ52_Empty_APDU(){
	SLJ52_APDU apdu;
	apdu.CLA = 0x00;
	apdu.INS = 0x00;
	apdu.P1 = 0x00;
	apdu.P2 = 0x00;
	apdu.LC = 0x00;
	for(int i = 0; i < MAX_APDU_DATA_SIZE; i++)
		apdu.Data[i] = 0x00;
	apdu.LE = 0x00;
	return apdu;
}

void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef* hsc){
	if(hsc->Instance == USART6){
		isTxReady = 1;
	}
}

void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef* hsc){
	if(hsc->Instance == USART6){
		isRxReady = 1;
	}
}

void SC_ParityErrorHandler(void)
{
  HAL_SMARTCARD_Transmit(&hsc6, (uint8_t *)&FailedByte, 1, WORK_ETU);
}

#if INTERRUPT_MODE
/**
  * @brief SMARTCARD error callbacks
  * @param hsc: SMARTCARD handle
  * @retval None
  */
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc)
{
  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_FE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
    /* Resend the byte that failed to be received (by the Smartcard) correctly */
    SC_ParityErrorHandler();
  }

  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_PE)
  {
    /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
    __HAL_SMARTCARD_ENABLE_IT(hsc, SMARTCARD_IT_RXNE);
    /* Flush the SC_USART DR register */
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }

  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_NE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }

  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_ORE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }
}
#endif
