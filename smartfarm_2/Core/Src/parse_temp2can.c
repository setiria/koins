/*
 * parse_temp2can.c
 *
 *  Created on: Jul 30, 2021
 *      Author: swpark
 */


#include "parse_temp2can.h"


extern UART_HandleTypeDef huart1;
extern uartQ_t g_uartQ1;

extern MODULE_INFO g_ModuleInfo;


uint8_t _pParsingBuffer1[64];
uint8_t _u8FlagInput1;
uint32_t _u32InputCnt1;

CAN_FilterTypeDef canFilter1;
CAN_RxHeaderTypeDef canRxHeader;
CAN_TxHeaderTypeDef canTxHeader;
uint8_t can1Rx0Data[8];
uint32_t TxMailBox;
uint8_t can1Tx0Date[8];

void loop_parse()
{
	uint8_t bData;
	if(uartutils_isEmpty(&g_uartQ1) == 0)
	{
		bData = uartutils_pop(&g_uartQ1);

		if(bData==(uint8_t)(';') ) //|| bData==(u8)('O'))
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

			_u32InputCnt1=1;
			_u8FlagInput1=1;
			_pParsingBuffer1[0]=bData;
		}
		else if(bData==(uint8_t)(0x0d))
		{
			_u8FlagInput1=0;
			Parse_2CAN_UART(_pParsingBuffer1,_u32InputCnt1);
			memset(_pParsingBuffer1,0,64);
		}
		else
		{
			if(_u8FlagInput1==1)
			{
				_pParsingBuffer1[_u32InputCnt1]=bData;
				_u32InputCnt1++;
			}
		}

	}
}

void Parse_2CAN_UART(u8 *pParsingBuf, u32 length)
{
    char pData[64];
    u16 u16Length;
    u8 u8Cmd;
    /*u8 pData[64];
    u16 u16Length;*/
    //static u32 u32cnt=0;

    u8Cmd=_M_str2u8(&pParsingBuf[1],2,16);

    sprintf(pData, "command %d\r", u8Cmd);
    u16Length=strlen(pData);
    HAL_UART_Transmit(&huart1,pData,u16Length,10);

    switch(u8Cmd)
    {
    case 0x00:                  // Read Device Information
    	Parse2CAN_ResDeviceInfo();
        break;
    case 0x01:                  // Read Device Information
    	_flash_read();
    	Parse2CAN_ResReadStatus();
        break;
    case 0x03:
        g_ModuleInfo.can_ID_type=CAN_ID_STD;
        g_ModuleInfo.can_ID=(u32)_M_str2u32(&pParsingBuf[3],3,16);
        _flash_write();
        break;
    case 0x04:
        g_ModuleInfo.can_ID_type=CAN_ID_EXT;
        g_ModuleInfo.can_ID=(u32)_M_str2u32(&pParsingBuf[3],8,16);
        _flash_write();
        break;
    case 0x05:                  // Read Device Information
        g_ModuleInfo.can_BT_SJW=(u8)_M_str2u32(&pParsingBuf[3],1,16)-1;
        g_ModuleInfo.can_BT_BS1=(u8)_M_str2u32(&pParsingBuf[4],1,16)-1;
        g_ModuleInfo.can_BT_BS2=(u8)_M_str2u32(&pParsingBuf[5],1,16)-1;
        g_ModuleInfo.can_BT_Prescaler=(u16)_M_str2u32(&pParsingBuf[6],4,16);
        _flash_write();
        break;
    case 0x06:                  // Read CAN Information
    	Parse2CAN_ResSensorValue();
        break;
    default:
        break;
    }

}



void Parse2CAN_ResDeviceInfo()
{
    char pData[64];
    u16 u16Length;


    sprintf(pData,";00%.2X%.2X%.2X%.2X%.2X\r",g_ModuleInfo.type,g_ModuleInfo.fwVer_Major,g_ModuleInfo.fwVer_Minor,
                                            g_ModuleInfo.hwVer_Major,g_ModuleInfo.hwVer_Minor);
    u16Length=strlen(pData);
//    SendUSART1((u8*)pData, u16Length);
    HAL_UART_Transmit(&huart1,pData,u16Length,10);

}


void Parse2CAN_ResReadStatus()
{
    char pData[64];
    u16 u16Length;

    sprintf(pData, "test %d\r", g_ModuleInfo.can_ID_type);

    if(g_ModuleInfo.can_ID_type==CAN_ID_STD)
    {
        sprintf(pData,";02%.1x%.1x%.1x%.4xS%.3x\r", g_ModuleInfo.can_BT_SJW+1,
              g_ModuleInfo.can_BT_BS1+1, g_ModuleInfo.can_BT_BS2+1,
              g_ModuleInfo.can_BT_Prescaler,g_ModuleInfo.can_ID);

    }
    else if(g_ModuleInfo.can_ID_type==CAN_ID_EXT)
    {
        sprintf(pData,";02%.1x%.1x%.1x%.4xE%.8x\r", g_ModuleInfo.can_BT_SJW,
              g_ModuleInfo.can_BT_BS1, g_ModuleInfo.can_BT_BS2,
              g_ModuleInfo.can_BT_Prescaler,g_ModuleInfo.can_ID);
    }

    u16Length=strlen(pData);
//    SendUSART1((u8*)pData, u16Length);
    HAL_UART_Transmit(&huart1,pData,u16Length,10);
}


void Parse2CAN_ResSensorValue()
{
//    char pData[64];
//    u16 u16Length;
//
//
//    sprintf(pData,";07,%.4d,%.4d\r",ADCConvertedValue[0],ADCConvertedValue[1]);
//    u16Length=strlen(pData);
//    SendUSART1(pData, u16Length);

	char pData[64];
	u16 u16Length;
	u16 u16Temperature, u16Humidity;
	  uint16_t addr = 0x44;
	  uint16_t Waddr = addr<<1;
	  uint16_t Raddr = (addr<<1)+1;
	  uint8_t mps[2] = {0x21, 0x30};
	  uint8_t fetch_data[2] = {0xE0, 0x00};
	  uint8_t data[6] = {0, 1, 2, 3, 4, 5};

	HAL_I2C_Master_Transmit(&hi2c1, Waddr, &mps[0], 2, 100);
	HAL_Delay(10);
	HAL_I2C_Master_Transmit(&hi2c1, Waddr, &fetch_data[0], 2, 100);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
	{	}

	HAL_I2C_Master_Receive(&hi2c1, Raddr, &data[0], 6, 100);

	uint16_t temp = data[0]<<8 | data[1];
	uint16_t hum = data[3]<<8 | data[4];

	float temperature = ((float)temp/65535.0*175.0 - 45.0)*10.0;
	float humidity = (100.0*(float)hum/65535.0)*10.0;

	u16Temperature = temperature;
	u16Humidity = humidity;

	sprintf(pData,";07,%.4d,%.4d\r",u16Temperature,u16Humidity);
	u16Length=strlen(pData);
	HAL_UART_Transmit(&huart1,pData,u16Length,10);

	  canTxHeader.StdId = 0x102;
	  canTxHeader.RTR = CAN_RTR_DATA;
	  canTxHeader.IDE = CAN_ID_STD;
	  canTxHeader.DLC = 7;

	  can1Tx0Date[0] = u16Temperature;
	  can1Tx0Date[1] = u16Temperature>>8;
	  can1Tx0Date[2] = u16Humidity;
	  can1Tx0Date[3] = u16Humidity>>8;
	  can1Tx0Date[4] = 0;
	  can1Tx0Date[5] = 0;
	  can1Tx0Date[6] = 0;

	  TxMailBox = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
	  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, &can1Tx0Date[0], &TxMailBox);
}


u32 _M_str2u32(u8* pString, u16 u16Num,u8 u8Digit)
{
    u32 Result=0;
    u8 pTempStr1[8]={0,};
    memcpy(pTempStr1,pString,u16Num);
    Result=(u32)strtoul(pTempStr1,0,16);
    return Result;
}

u8 _M_str2u8(u8* pString, u16 u16Num,u8 u8Digit)
{
    u8 Result=0;
    u8 pTempStr1[8]={0,};
    memcpy(pTempStr1,pString,u16Num);
    Result=(u8)strtoul(pTempStr1,0,16);
    return Result;
}


u16 _M_str2u16(u8* pString, u16 u16Num,u8 u8Digit)
{
    u8 Result=0;
    u8 pTempStr1[8]={0,};
    memcpy(pTempStr1,pString,u16Num);
    Result=(u16)strtoul(pTempStr1,0,16);
    return Result;
}

u32 _ReadFlashData(u32 u32Addr)
{
	u32 u32Data;
	u32Data=(*(vu32*)u32Addr);
	return u32Data;
}

void _flash_read(void)
{
//    u32 u32FlashBaseAddr = FLASH_BASEADDR;
//
//    g_ModuleInfo.can_ID=(u32)ReadFlashData(u32FlashBaseAddr);
//    g_ModuleInfo.can_BT_BS1=(u8)ReadFlashData(u32FlashBaseAddr+4);
//    g_ModuleInfo.can_BT_BS2=(u8)ReadFlashData(u32FlashBaseAddr+8);
//    g_ModuleInfo.can_BT_SJW=(u8)ReadFlashData(u32FlashBaseAddr+12);
//    g_ModuleInfo.can_BT_Prescaler=(u16)ReadFlashData(u32FlashBaseAddr+16);
//    g_ModuleInfo.can_ID_type=(u8)ReadFlashData(u32FlashBaseAddr+20);

	u32 u32FlashBaseAddr = FLASH_BASEADDR;
	u32 u32Temp;
	u32Temp=*(__IO uint32_t*)u32FlashBaseAddr;
	g_ModuleInfo.can_ID=(u32)(u32Temp);
	u32Temp=*(__IO uint32_t*)(u32FlashBaseAddr+4);
	g_ModuleInfo.can_BT_BS1=(u8)(u32Temp);
	u32Temp=*(__IO uint32_t*)(u32FlashBaseAddr+8);
	g_ModuleInfo.can_BT_BS2=(u8)(u32Temp);
	u32Temp=*(__IO uint32_t*)(u32FlashBaseAddr+12);
	g_ModuleInfo.can_BT_SJW=(u8)(u32Temp);
	u32Temp=*(__IO uint32_t*)(u32FlashBaseAddr+16);
	g_ModuleInfo.can_BT_Prescaler=(u16)(u32Temp);
	u32Temp=*(__IO uint32_t*)(u32FlashBaseAddr+20);
	g_ModuleInfo.can_ID_type=(u8)(u32Temp);

//	MODULE_INFO temp_info;
//
//	memcpy(&temp_info, (MODULE_INFO*)u32FlashBaseAddr, sizeof(temp_info));
//
//	memcpy(&g_ModuleInfo, &temp_info, sizeof(temp_info));
}

void _flash_write(void)
{
    u32 u32FlashBaseAddr = FLASH_BASEADDR;
    char pData[64];
    u16 u16Length;
    int check = 0;
    static FLASH_EraseInitTypeDef ErageStruct;

    ErageStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    ErageStruct.PageAddress = FLASH_BASEADDR;
    ErageStruct.NbPages = 1;

    HAL_FLASH_Unlock();
    int pageerror;
    if(HAL_FLASHEx_Erase(&ErageStruct, &pageerror) != HAL_OK)
    {
    	sprintf(pData, "erage page error\r");
    }
    else
    {
    	sprintf(pData, "erage page success\r");
    }

    u16Length=strlen(pData);
                HAL_UART_Transmit(&huart1,pData,u16Length,10);


    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+0,(u32)g_ModuleInfo.can_ID);
    if(check == HAL_OK)
    {
    	sprintf(pData, "test1\r");
    }
    else
    {
    	sprintf(pData, "test1 error %d\r", check);
    }
    u16Length=strlen(pData);
            HAL_UART_Transmit(&huart1,pData,u16Length,10);
    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+4,(u32)g_ModuleInfo.can_BT_BS1);
    if(check == HAL_OK)
        {
        	sprintf(pData, "test2\r");
        }
    else
        {
        	sprintf(pData, "test2 error %d\r", check);
        }
    u16Length=strlen(pData);
            HAL_UART_Transmit(&huart1,pData,u16Length,10);
    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+8,(u32)g_ModuleInfo.can_BT_BS2);
    if(check == HAL_OK)
        {
        	sprintf(pData, "test3\r");
        }
    else
        {
        	sprintf(pData, "test3 error %d\r", check);
        }
    u16Length=strlen(pData);
            HAL_UART_Transmit(&huart1,pData,u16Length,10);
    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+12,(u32)g_ModuleInfo.can_BT_SJW);
    if(check == HAL_OK)
        {
        	sprintf(pData, "test4\r");
        }
    else
        {
        	sprintf(pData, "test4 error %d\r", check);
        }
    u16Length=strlen(pData);
            HAL_UART_Transmit(&huart1,pData,u16Length,10);
    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+16,(u32)g_ModuleInfo.can_BT_Prescaler);
    if(check == HAL_OK)
        {
        	sprintf(pData, "test5\r");
        }
    else
        {
        	sprintf(pData, "test5 error %d\r", check);
        }
    u16Length=strlen(pData);
            HAL_UART_Transmit(&huart1,pData,u16Length,10);
    check = HAL_FLASH_Program(TYPEPROGRAM_WORD, u32FlashBaseAddr+20,(u32)g_ModuleInfo.can_ID_type);
    if(check == HAL_OK)
        {
        	sprintf(pData, "test6\r");
        }
    else
        {
        	sprintf(pData, "test6 error %d\r", check);
        }
    HAL_FLASH_Lock();

    u16Length=strlen(pData);
        HAL_UART_Transmit(&huart1,pData,u16Length,10);
}




