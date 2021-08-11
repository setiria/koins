/*
 * uart1_utils.c
 *
 *  Created on: Jul 30, 2021
 *      Author: swpark
 */

#include "uart1_utils.h"

//extern
//HAL_UART_Transmit(&huart1, &rx_data, 1, 10);

void uartutils_init(uartQ_t* u)
{
  u->head = 0;
  u->tail = 0;
  memset(u->buffer, 0, sizeof(u->buffer));
}

void uartutils_push(uartQ_t* u, uint8_t data)
{
  u->buffer[u->head] = data;

  u->head++;

  if (u->head >= MAX_BUFFER_SIZE) {
    u->head = 0;
  }
}

uint8_t uartutils_pop(uartQ_t* u)
{
  uint8_t data = u->buffer[u->tail];

  u->tail++;

  if (u->tail >= MAX_BUFFER_SIZE) {
    u->tail = 0;
  }

  return data;
}

uint8_t uartutils_isEmpty(uartQ_t* u)
{
  return u->head == u->tail;
}

