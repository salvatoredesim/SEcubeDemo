/*
 * protocol.c
 *
 *  Created on: 31 dic 2016
 *      Author: raidenfox
 */

#include "protocol.h"
#include "smartcard.h"

CMD_Message op;
CMD_Rx_Status cmd_status = IDLERX;
uint8_t buffer_rx[MAX_PAYLOAD_SIZE] = {0xFF};
uint16_t count = 0x0000;
uint8_t header_size;
uint16_t payload_size;

void P0_ParseMessage(){
	switch(cmd_status){
	case IDLERX:
		if(getbyte(&buffer_rx[0])){
			cmd_status = TYPE_RECEIVED;
		}
		break;
	case TYPE_RECEIVED:
		if(getbyte(&buffer_rx[1])){
			cmd_status = OPERATION_RECEIVED;
		}
		break;
	case OPERATION_RECEIVED:
		if(getbyte(&buffer_rx[2])){
			cmd_status = SOURCE_RECEIVED;
		}
		break;
	case SOURCE_RECEIVED:
		if(getbyte(&buffer_rx[3]))
			cmd_status = DEST_RECEIVED;
		break;
	case DEST_RECEIVED:
		if(getbyte(&buffer_rx[4])){
			if(buffer_rx[4] == 0xFF)
				cmd_status = LEN_INCOMPLETE;
			else
				cmd_status = LEN_RECEIVED;
		}
		break;
	case LEN_INCOMPLETE:
		if(getbyte(&buffer_rx[5])){
			cmd_status = LEN_RECEIVED;
		}
		break;
	case LEN_RECEIVED:
		header_size = 0x05;
		payload_size = 0x0000;
		if(buffer_rx[4] == 0xFF){
			header_size++;
			payload_size = buffer_rx[4]+buffer_rx[5];
		}else{
			payload_size = buffer_rx[4];
		}

		if(getbyte(&buffer_rx[header_size+count])){
					count++;
				}
		if((count == payload_size) || (count == 0)){
				count = 0;
				cmd_status = COMPLETED;
			}
		break;
	default:
		break;
	}

	if(cmd_status == COMPLETED){
		op.type = buffer_rx[0];
		op.operation = buffer_rx[1];
		op.source = buffer_rx[2];
		op.destination = buffer_rx[3];
		op.size = payload_size;
		if(op.size > 0){
			for(int i = 0; i < op.size; i++){
				op.data[i] = buffer_rx[header_size+i];
			}
		}
		switch(op.destination){
			case SMARTCARD:
				P0_SmartcardExec(op);
			break;

			case FPGA:
				ReceiveFragment(op);
			break;
			default:
			break;
		}
		cmd_status = IDLERX;
		memset(&buffer_rx,0x00, MAX_PAYLOAD_SIZE*sizeof(uint8_t));
		memset(&op,0x00,sizeof(CMD_Message));
	}
}

void ReceiveFragment(CMD_Message command){
		putbyte(0x90);
}

void P0_SmartcardExec(CMD_Message command){
	SLJ52ATR atr;
	SLJ52_APDU apdu;
	switch(command.operation){
		case ATR:
			op.size = 0;
			atr = SLJ52_GetATR();
			putbyte(atr.TS);
			putbyte(atr.T0);
			for(int i = 0; i < atr.Tlength ;i++){
				putbyte(atr.T[i]);

			}
			for(int i = 0; i < atr.Hlength ;i++){
				putbyte(atr.H[i]);

			}

			putbyte(atr.CheckSum);
			break;
		case APDU:
		case APDU_DEBUG:
		    memset(&apdu, 0x00, sizeof(SLJ52_APDU));

			apdu.CLA = op.data[0];
			apdu.INS = op.data[1];
			apdu.P1 = op.data[2];
			apdu.P2 = op.data[3];
			apdu.LC = op.data[4];
			for(int i = 0; i < apdu.LC ; i++){
				apdu.Data[i] = op.data[5+i];
			}
			apdu.LE = op.data[5+apdu.LC];
			SLJ52_APDU_Response Resp;
			uint8_t size = 0;

			/*
			uint8_t response[1024] = {0x00};
			SLJ52_SendAPDU(apdu,&size,response);

			for(int i = 0; i < size; i++){
				putbyte(response[i]);
			}
			*/
			if(command.operation != APDU_DEBUG)
				SLJ52_SendAPDU(apdu,&size,&Resp);
			//else
				//SLJ52_SendAPDU_TimeExec(apdu,&size,&Resp);

			if(size > 0){
				if(size <=2){
					putbyte(Resp.SW1);
					putbyte(Resp.SW2);
				}
				else
				{
					putbyte(Resp.SW1);
					putbyte(Resp.SW2);
					for(int i = 0; i < size; i++){
						putbyte(Resp.Data[i]);
					}
				}
			}
			else{
				putbyte(0xFF);
				putbyte(0xFF);
			}
			break;
		default:
			break;
	}
}


