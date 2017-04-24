/**
 ******************************************************************************
  * @file    FPGA.h
  * @brief   This file describes the primitives adopted to program the FPGA present in SEcube
  ******************************************************************************
  * @attention
  *
  * HOW TO USE THIS DRIVER
  *
  * Simply, call the B5_FPGA_Programming primitive, passing name of the algo and data file, with
  * his extenstion.
  *
  ******************************************************************************
  */

#ifndef APPLICATION_SRC_FPGA_H_
#define APPLICATION_SRC_FPGA_H_

#include "flashmem.h"

#define FLASH_DATA_ADDR   ADDR_FLASH_SECTOR_5   /* Start @ of data Flash area, modify if you need */
#define FLASH_ALGO_ADDR   ADDR_FLASH_SECTOR_17   /* Start @ of algo Flash area, modify if you need */
/**
 * @brief  Programs the FPGA after a reset of the previous bitstream configuration
 * @param  nameAlgo: name of the algo VME file present in the uSD, with extension (algo.vme)
 * @param  nameData: name of the data VME file present in the uSD, with extension (data.vme)
 * @retval the status code of the operation, 0 if programming is OK
 */
int32_t 	B5_FPGA_Programming(uint32_t algosize, uint32_t datasize);
void        B5_FPGA_SetMux (uint8_t mux);
void        B5_FPGA_FpgaCpuGPIO (uint8_t gpioNum, GPIO_PinState set);

#endif /* APPLICATION_SRC_FPGA_H_ */
