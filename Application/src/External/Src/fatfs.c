/*
 * ll_fatfs.c
 *
 *  Created on: 26 nov 2016
 *      Author: raidenfox
 */

#include "fatfs.h"

void FATFS_Init(){
	  retSD = FATFS_LinkDriver(&SD_Driver, SD_Path);
	  f_mount(&fileSystem, SD_Path, 1);
}

FIL FATFS_fopen(FIL* opened,const char*filename, FRESULT* status, uint8_t mode){
	f_open(opened,filename,mode);
	*status = FR_OK;
	return *opened;
}

uint8_t FATFS_fgetc(FIL* linked){
		  UINT counted;
		  uint8_t buff;
		  f_read(linked, &buff, 1, &counted);
		  //sendTostream(*buff);
		  return buff;
}

uint16_t FATFS_fgetw(FIL* linked){
		  UINT counted;
		  uint16_t buff;
		  f_read(linked, &buff, 2, &counted);
		  //sendTostream(*buff);
		  return buff;
}

uint8_t* FATFS_fget(FIL* linked, uint8_t size){
		  UINT counted;
		  uint8_t* buff = (uint8_t*) malloc(size*sizeof(uint8_t));
		  memset(buff,0x00,size);
		  f_read(linked, buff, size, &counted);
		  //sendTostream(*buff);
		  return buff;
}

void FATFS_fread(){
		  uint32_t size = linkedFile.fsize;
		  UINT counted;
		  uint8_t* buff = (uint8_t*) malloc(sizeof(uint8_t));
		  for(int i = 0; i < size ; i++){
			  f_read(&linkedFile, buff, 1, &counted);
		  }
}

uint8_t FATFS_fgets(){
		  UINT counted;
		  uint8_t* buff = (uint8_t*) malloc(sizeof(uint8_t));
		  f_read(&linkedFile, buff, 1, &counted);
		  return *buff;
}

uint32_t FATFS_GetSize(FIL file){
	return file.fsize;
}

void FATFS_UnlinkFile(){
	  f_close(&linkedFile);
}

void FATFS_fclose(FIL* file){
	  f_close(file);
}
