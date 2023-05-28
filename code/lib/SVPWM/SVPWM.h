/**********************************************************************************
 * @name        SVPWM.h
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/2/12
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#ifndef _SVPWM_H_
#define _SVPWM_H_

#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"

#include "parameter.h"
#include "Ctrl.h"
#include "Basic.h"

void SVPWM_Init(void);
void SVPWM_config(uint16_t A_duty, uint16_t B_duty, uint16_t C_duty);
void Timer_NVIC_Init(void);
void Timers_Init(u16 prescaler, u16 period, u16 control_cycle);
void TIM2_Init(void);
void initGPIO_Timer1(void);
void SVPWM_Stop(void);
void SVPWM_Start(void);
//old code
void SVPWM(uint8_t m, uint16_t n);
void FOAC(int16_t angle);

#endif

/*
void delay_10us(uint32_t _t)
{
	_t *= 7;
	while(_t--);
}
main£º
	for(TEMP = 0; TEMP < 4116; TEMP+=2)
	{
		LED1_ON();
		FOAC(TEMP);
		LED1_OFF();
		delay_10us(30);
//			delay_ms(1);
	}
*/

//interrupt RTtest
//		LED1_ON();
//		LED1_OFF();
//		if(test_data1 == 0)
//		{
//			LED1_ON();
//		}
//		else{
//			LED1_OFF();
//		}
