/**********************************************************************************
 * @name        Basic.h
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/2/13
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#ifndef _BASIC_H_
#define _BASIC_H_

#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"

extern u32 SystemCoreClock;
void delay_init(void);
void delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void LED_Init(void);
void KEY_Init(void);

#define LED1_Port  		GPIOB
#define LED1_Pin   		GPIO_Pin_6
#define LED1_OFF()  	GPIO_ResetBits(LED1_Port,LED1_Pin)
#define LED1_ON()  		GPIO_SetBits(LED1_Port,LED1_Pin)
#define LED1_TOGGLE()  	(GPIO_ReadOutputDataBit(LED1_Port,LED1_Pin))?(GPIO_ResetBits(LED1_Port,LED1_Pin)):(GPIO_SetBits(LED1_Port,LED1_Pin))

#define KEY_Port        GPIOA
#define KEY_Pin         GPIO_Pin_0
#define KEY_Value       GPIO_ReadInputDataBit(KEY_Port, KEY_Pin)

#endif
