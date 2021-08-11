/*
 * parse_temp2can.h
 *
 *  Created on: Jul 30, 2021
 *      Author: swpark
 */

#ifndef INC_PARSE_TEMP2CAN_H_
#define INC_PARSE_TEMP2CAN_H_

#include "define.h"
#include "uart1_utils.h"
#include "i2c.h"
#include "can.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct _MODULE_INFO
{
    u8 type;
    u8 fwVer_Major;
    u8 fwVer_Minor;
    u8 hwVer_Major;
    u8 hwVer_Minor;

    u8 can_BT_BS1;
    u8 can_BT_BS2;
    u8 can_BT_SJW;

    u16 can_BT_Prescaler;

    u8 can_ID_type;
    u32 can_ID;

}MODULE_INFO;


typedef union _FLOAT_DATA
{
	u32 u32Data;
	float fData;
	u8 u8Data[4];
	int nData;
}FLOAT_DATA;

//#define CAN_ID_STD		0
//#define CAN_ID_EXT		4

void loop_parse();

void Parse_2CAN_UART(u8 *pParsingBuf, u32 length);

void Parse2CAN_ResSensorValue();
void Parse2CAN_ResReadStatus();
void Parse2CAN_ResDeviceInfo();

u16 _M_str2u16(u8* pString, u16 u16Num,u8 u8Digit);
u8 _M_str2u8(u8* pString, u16 u16Num,u8 u8Digit);
u32 _M_str2u32(u8* pString, u16 u16Num,u8 u8Digit);

void _flash_write(void);
void _flash_read(void);
u32 _ReadFlashData(u32 u32Addr);

#endif /* INC_PARSE_TEMP2CAN_H_ */
