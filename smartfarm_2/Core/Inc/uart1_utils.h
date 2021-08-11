/*
 * uart1_utils.h
 *
 *  Created on: Jul 30, 2021
 *      Author: swpark
 */

#ifndef INC_UART1_UTILS_H_
#define INC_UART1_UTILS_H_


#include "stm32f1xx_hal.h"

#define MAX_BUFFER_SIZE    1024

typedef struct{
  uint8_t head;
  uint8_t tail;
  uint8_t buffer[MAX_BUFFER_SIZE];
}uartQ_t;

uint8_t uartutils_isEmpty(uartQ_t* u);
uint8_t uartutils_pop(uartQ_t* u);
void uartutils_push(uartQ_t* u, uint8_t data);
void uartutils_init(uartQ_t* u);

#endif /* INC_UART1_UTILS_H_ */
