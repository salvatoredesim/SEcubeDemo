/*
 * cp2102.c
 *
 *  Created on: 04 dic 2016
 *      Author: raidenfox
 */

#include "cp2102.h"

uint8_t rx_ok = 0;

void printuf(const char* bufferTx, uint32_t size){
	while(HAL_UART_Transmit_IT(&huart1,(uint8_t*)bufferTx,size) != HAL_OK);
}

void printbuf(const char bufferTx[]){
	int size = sizeof(bufferTx)/sizeof(char);
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)bufferTx,size);
}

void putch(char* c){
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)c,1);
}

void putbyte(uint8_t c){
	while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TXE) == RESET);
	HAL_UART_Transmit_IT(&huart1,&c,1);
	while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC) == RESET);
}

uint8_t getbyte(uint8_t* byte){
	uint8_t retval = 0;
	uint32_t counter = 25000;
	while((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE) == RESET) && (counter > 0)){counter--;}
	if(counter > 0){
	HAL_UART_Receive(&huart1,byte,1,0);
		retval = 1;
	}else{
		retval = 0;
	}
	return retval;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart1)
{

}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart1)
{
  /* Set transmission flag: transfer complete*/
  if(huart1->Instance==USART1){
	  rx_ok = 1;
  }

  /* Turn LED3 on: Transfer in reception process is correct */
}
