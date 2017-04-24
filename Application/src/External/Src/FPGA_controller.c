/*
 * FPGA_controller.c
 *
 *  Created on: 08 feb 2017
 *      Author: raidenfox
 */

#include "FPGA_controller.h"

uint32_t decript_bitstream(const char* namefile,uint8_t* key,uint8_t type){

	// Initialize AES-128 Crypto Session
	SW_Crypto_Init(key,AES_128, ECB_DEC);
	uint32_t size = 0;
	switch(type){
		case ALGO_FILE:
			SW_Crypto_DecryptFile(namefile,&size,FLASH_ALGO_ADDR);
			break;
		case DATA_FILE:
			SW_Crypto_DecryptFile(namefile,&size,FLASH_DATA_ADDR);
			break;
	}
	SW_Crypto_DeInit();
	return size;
}
