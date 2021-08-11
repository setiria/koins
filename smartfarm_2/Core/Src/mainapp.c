/*
 * mainapp.c
 *
 *  Created on: Jul 29, 2021
 *      Author: swpark
 */


#include "mainapp.h"
#include "uart1_utils.h"
#include "parse_temp2can.h"

extern UART_HandleTypeDef huart1;

uint8_t g_u8RxData;

uartQ_t g_uartQ1;
MODULE_INFO g_ModuleInfo;


void setup()
{
	HAL_CAN_Start(&hcan);
	uartutils_init(&g_uartQ1);
	HAL_UART_Receive_IT(&huart1, &g_u8RxData, 1);
}

void loop()
{
	loop_parse();
}

// �씤�꽣�읇�듃 肄쒕갚 �븿�닔: �씤�꽣�읇�듃媛� 諛쒖깮�릺硫� �씠 �븿�닔媛� �샇異쒕맂�떎.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	if (huart->Instance == USART1) {
//        // �뜲�씠�꽣 1媛쒕�� �닔�떊�븯硫� �씤�꽣�읇�듃瑜� 諛쒖깮�떆�궓�떎.
//		HAL_UART_Receive_IT(&huart1, &g_u8RxData, 1);
//
//        // 諛쏆� �뜲�씠�꽣瑜� �쟾�넚�븳�떎.
//		HAL_UART_Transmit(&huart1, &rx_data, 1, 10);
//	}

	if (huart->Instance == USART1) {
	    uartutils_push(&g_uartQ1,g_u8RxData);	//push data into Queue
	    HAL_UART_Receive_IT(&huart1, &g_u8RxData, 1);//permit RX interrupt.
	  }
}



void uart_send_string(uint8_t *pString, uint16_t len)
{
	HAL_StatusTypeDef status;
	status=HAL_UART_Transmit(&huart1, pString, len, 10);
}

