/*
 * crypto.c
 *
 *  Created on: 06 feb 2017
 *      Author: raidenfox
 */

#include "crypto.h"

// Crypto context to execute a crypto session
B5_tAesCtx crypto_context;

// Initialize the crypto session data like key size and the crypto mode
void SW_Crypto_Init(uint8_t* key,KeySize keySize, CriptoMode mode ){
	B5_Aes256_Init(&crypto_context,key,keySize,mode);
}

// Encode a text, returning data crypted in the passed param crypted
void SW_Crypto_Encode16Byte(uint8_t* block, uint8_t* crypted){
	uint8_t* encData = (uint8_t*) malloc(16*sizeof(uint8_t));
	memset(encData, 0x00,16);
	B5_Aes256_Update(&crypto_context,encData,block,1);
	memcpy(crypted,encData,16);
	free(encData);
}

// Same as Encode, but decrypt a chipered text
void SW_Crypto_Decode16Byte(uint8_t* block, uint8_t* decrypted){
	uint8_t* decData = (uint8_t*) malloc(16*sizeof(uint8_t));
	memset(decData, 0x00,16);
	B5_Aes256_Update(&crypto_context,block,decData,1);
	memcpy(decrypted,decData,16);
	free(decData);
}

// Close the crypto session
void SW_Crypto_DeInit(){
	B5_Aes256_Finit(&crypto_context);
}

/* Decrypt a file passing name, size and returns the pointer to the decrypted area in the FLASH
 This is done because of the large dimensions of bitstreams, so we need to pass it in flash
 and execute all the needed operations */
uint8_t* SW_Crypto_DecryptFile(const char* namefile, uint32_t* dec_size, uint32_t decrypted_address){

	// Open the target vme file
	FIL opened;
	FRESULT status;
	FATFS_fopen(&opened,namefile,&status,FA_READ);
	*dec_size = FATFS_GetSize(opened);

	// Allocate a 16 byte buffer for read blocks of 16 bite from VME file and clean it
	uint8_t* buffer = (uint8_t*) malloc(16*sizeof(uint8_t));
	memset(buffer,0x00,16);

	// Allocate a buffer for the decoded 16 byte text
	uint8_t* buffer_decoded = (uint8_t*) malloc(16*sizeof(uint8_t));
	memset(buffer_decoded,0x00,16);

	// Calculate number of words in bitstream file and the spare bytes
	uint32_t num_words = (*dec_size)/16;
	uint8_t spare_bytes = (*dec_size)%16;

	// Lets use the flash because RAM is small for bitstreams
	Flash_Initialize();
	Flash_Erase(decrypted_address,*dec_size);

	// Decode the file and write all the even 16 byte blocks in FLASH
	for(uint32_t word_index = 0; word_index < num_words; word_index++){
		uint8_t* read = FATFS_fget(&opened,16);
		memcpy(buffer,read,16);
		SW_Crypto_Decode16Byte(buffer,buffer_decoded);
		// Write each byte in flash
		for(int j = 0; j < 16; j++){
			Flash_Byte_Write((decrypted_address+(word_index*16))+j,*(buffer_decoded+j));
		}
		memset(buffer_decoded,0x00,16);
		free(read);
	}

	// If there are spare bytes (less than 16) write the remaining in the FLASH
	if(spare_bytes != 0){
		uint8_t* lastbyte = FATFS_fget(&opened,spare_bytes);
		memset(buffer,0x00,16);
		memcpy(buffer,lastbyte,spare_bytes);
		SW_Crypto_Decode16Byte(buffer,buffer_decoded);
		for(int j = 0; j < 16; j++){
				Flash_Byte_Write((decrypted_address+(num_words*16))+j,*(buffer_decoded+j));
			}
		//memcpy(decoded+(num_words*16),buffer_decoded,16);
		memset(buffer_decoded,0x00,16);
		free(lastbyte);
	}
	Flash_Lock();

	return (uint8_t*)decrypted_address;
}
