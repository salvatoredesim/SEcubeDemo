/**
 ******************************************************************************
  * @file    cp2102.h
  * @brief   This file describes the primitives adopted to send data on UART1 connected to the cp2102 uart
  ******************************************************************************
  * @attention
  * This driver uses the Interruptions on the MCU. So before to call the primitives
  * defined, you have to enable the interrupts in the usart.c file, defining in the
  * Msp function these HAL primitives
  *
  * HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
  *
  * HAL_NVIC_EnableIRQ(USART1_IRQn);
  *
  * and disable IT in the DeInit primitive calling
  *
  * HAL_NVIC_DisableIRQ(USART1_IRQn);
  *
  * HOW TO USE THIS DRIVER
  *
  * If You want to print on the terminal a string, call the printuf function
  * passing as argument the string and the size of the string, with the spaces.
  *
  ******************************************************************************
  */

#ifndef APPLICATION_EXTERN_INC_CP2102_H_
#define APPLICATION_EXTERN_INC_CP2102_H_

#include "usart.h"

void putch(char* c);
/**
 * @brief  Print a string on the terminal
 * @param  bufferTx: The string you want to write
 * @param  size: the size of the string
 */
void printuf(const char* bufferTx, uint32_t size);

/**
 * @brief  Print a buffer on the serialterminal
 * @param  bufferTx: The buffer you want to write
 */
void printbuf(const char bufferTx[]);

/**
 * @brief  Print a string on the terminal
 * @param  c: The character you want to print on the serial interface
 */
void putbyte(uint8_t c);

/**
 * @brief  Read a string on the terminal
 * @param  byte: Address of the character you want to read on the serial interface
 */
uint8_t getbyte(uint8_t* byte);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart1);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart1);

#endif /* APPLICATION_EXTERN_INC_CP2102_H_ */
