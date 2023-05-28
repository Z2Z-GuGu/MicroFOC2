/**********************************************************************************
 * @name        ADC.c
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/4/23
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/
#ifndef _ADC_H_
#define _ADC_H_

#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"

#define ADCR_SWSTART_Set ((uint32_t)0x00000100)

extern uint16_t ADC_RAW_Data;
extern uint8_t ADC_RAW_Data_ready;

void ADC_Single_Init(void);
uint16_t ADC1_SingleChannel_Get(void);
void ADC_Start(void);
uint16_t ADC_Data_Get(void);

#endif

//while1:
//		TEMP = ADC1_SingleChannel_Get();
//		UartSendGroup((u8*)printBuf, sprintf(printBuf, "PB1: %d\r\n", TEMP));
//intrrupt:
//	hander:
//		ADC_RAW_Data = ADC_Data_Get();
//		ADC_RAW_Data_ready = 1;
//		ADC_Start();
//	init:
//		ADC_Single_Init();
//		ADC_Start();
//	while(1):
//		if(ADC_RAW_Data_ready == 1)
//		{
//			UartSendGroup((u8*)printBuf, sprintf(printBuf, "PB1: %d\r\n", ADC_RAW_Data));
//			ADC_RAW_Data_ready = 0;
//		}
