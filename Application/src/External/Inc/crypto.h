/*
 * crypto.h
 *
 *  Created on: 06 feb 2017
 *      Author: raidenfox
 */

#ifndef APPLICATION_SRC_EXTERNAL_INC_CRYPTO_H_
#define APPLICATION_SRC_EXTERNAL_INC_CRYPTO_H_

#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include <string.h>
#include <stdlib.h>
#include "aes256.h"
#include "flashmem.h"

#define ECB_DEC B5_AES256_ECB_DEC
#define ECB_ENC B5_AES256_ECB_ENC

#define AES_128 B5_AES_128
#define AES_192 B5_AES_192
#define AES_256 B5_AES_256

typedef uint8_t CriptoMode;
typedef int16_t KeySize;

/**
  * @brief  Init the crypto session
  * @param  key: the key passed to the crypto session
  * @param  keySize: key size
  * @param  mode: Specify if is a crypt or decrypt session
  */
void SW_Crypto_Init(uint8_t* key,KeySize keySize, CriptoMode mode);

/**
  * @brief  Encode 16 byte with the key passed
  * @param  block: buffer for the data to crypt
  * @param  crypted: buffer for the crypted data
  */
void SW_Crypto_Encode16Byte(uint8_t* block, uint8_t* crypted);

/**
  * @brief  Decode 16 byte with the key passed
  * @param  block: buffer for the data to decrypt
  * @param  crypted: buffer for the decrypted data
  */
void SW_Crypto_Decode16Byte(uint8_t* block, uint8_t* decrypted);

/**
  * @brief  Deinit the Crypto session
  */
void SW_Crypto_DeInit();

/**
  * @brief  Decrypt the VME file
  * @param  namefile: name of the file to decrypt
  * @param  dec_size: size of the file to decrypt
  * @param  decrypted_address: address of the decrypted area in FLASH
  */
uint8_t* SW_Crypto_DecryptFile(const char* namefile, uint32_t* dec_size, uint32_t decrypted_address);

#endif /* APPLICATION_SRC_EXTERNAL_INC_CRYPTO_H_ */
