/*
 * FPGA_controller.h
 *
 *  Created on: 08 feb 2017
 *      Author: raidenfox
 */

#ifndef APPLICATION_SRC_EXTERNAL_INC_FPGA_CONTROLLER_H_
#define APPLICATION_SRC_EXTERNAL_INC_FPGA_CONTROLLER_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include "crypto.h"
#include "aes256.h"
#include "flashmem.h"
#include "FPGA.h"

#define ALGO_FILE 0x01
#define DATA_FILE 0x02

uint32_t decript_bitstream(const char* namefile,uint8_t* key,uint8_t type);

#endif /* APPLICATION_SRC_EXTERNAL_INC_FPGA_CONTROLLER_H_ */
