/**
 ******************************************************************************
  * @file    fatfs.h
  * @brief   This file describes the primitives adopted to read/write on the uSD
  ******************************************************************************
  * @attention
  *
  * HOW TO USE THIS DRIVER
  *
  * Initializate the FATFS_Init function to link the FATFS Driver in the app.
  * Then use the FATFS_fopen primitive to open a file and get his handle. Obtained
  * the handle you can use the primitive FATFS_fgetc to read byte by byte or
  * FATFS_fclose to close the file previously opened.
  *
  ******************************************************************************
  */

#ifndef INC_FATFS_H_
#define INC_FATFS_H_

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include <string.h>
#include <malloc.h>/* defines SD_Driver as external */


#define BLOCK_SIZE 4096

uint32_t refreshed_alg;
uint32_t refreshed_data;

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

uint8_t retSD;    /* Return value for SD */
char SD_Path[4];  /* SD logical drive path */

FATFS fileSystem;
FIL linkedFile;
FIL debugFile;

/**
  * @brief  Initializes the driver to work with the uSD
  */
void FATFS_Init();
void FATFS_UnlinkFile();
void FATFS_fread();
uint8_t FATFS_fgets();
/**
 * @brief  Get the size of the file in bytes
 * @param  file: Handler obtained with the FATFS_Init function
 */
uint32_t FATFS_GetSize(FIL file);
/**
 * @brief  Get the handle of the file to open
 * @param  file: Handler to obtain with the FATFS_Init function, passed as reference
 * @param  filename: Name of the file to open from uSD card
 * @param  state: state of the operation
 * @param  mode: open mode (FA_READ, FA_WRITE)
 */
FIL FATFS_fopen(FIL* file,const char*filename, FRESULT* state, uint8_t mode);
/**
 * @brief  Get the actual byte to read
 * @param  linked: Handler to obtain with the FATFS_Init function, passed as reference
 * @retval the byte read at the actual reading pointer
 */
uint8_t FATFS_fgetc(FIL* linked);
uint16_t FATFS_fgetw(FIL* linked);
uint8_t* FATFS_fget(FIL* linked, uint8_t size);
/**
 * @brief  Close the file and destroy the handler
 * @param  file: Handler to obtain with the FATFS_Init function, passed as reference
 */
void FATFS_fclose(FIL* file);


#endif /* INC_FATFS_H_ */
