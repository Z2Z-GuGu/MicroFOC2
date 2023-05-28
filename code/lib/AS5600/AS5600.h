/**********************************************************************************
 * @name        AS5600.h
 * @version     R2.3
 * @author      Z2Z GuGu
 * @date        2023/5/23
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/
#ifndef _AS5600_H_
#define _AS5600_H_

#include "parameter.h"
#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"
#include "Basic.h"

#define I2C1_SCL_BUSCLK                  RCC_AHBPeriph_GPIOD
#define I2C1_SCL_PIN                     GPIO_Pin_1
#define I2C1_SCL_PORT                    GPIOD
#define I2C1_SCL_AFSOURCE                GPIO_PinSource1
#define I2C1_SCL_AFMODE                  GPIO_AF_1

#define I2C1_SDA_BUSCLK                  RCC_AHBPeriph_GPIOD
#define I2C1_SDA_PIN                     GPIO_Pin_0
#define I2C1_SDA_PORT                    GPIOD
#define I2C1_SDA_AFSOURCE                GPIO_PinSource0
#define I2C1_SDA_AFMODE                  GPIO_AF_1

#define AS5600_Addr         0x36
#define AS5600_raw_ang_hi   0x0c
#define AS5600_raw_ang_lo   0x0d
#define AS5600_ang_hi   	0x0e
#define AS5600_ang_lo   	0x0f

#define AS5600_CFG1_REG		0x07
#define AS5600_CFG2_REG		0x08
#define AS5600_STATE_REG	0x0B

// #define Angle_correction    0
#define Positive_Rotation_threshold		2000
#define Positive_Filter_threshold		200
#define Negative_Filter_threshold		-200
#define Negative_Rotation_threshold		-2000
#define Maximum_Angle					4096
#define N_Maximum_Angle					-4096

#define Position_Inc_Seq_MAX			50
#define Speed_Filter_MAX				8

#define AS5600_REV_LEN 2

extern volatile int16_t Last_Position;
extern volatile uint8_t AS5600_Data_Ready;
extern volatile int16_t D_Last_Position;
extern volatile int16_t D_Last_Increment;
extern volatile int32_t Absolute_Position;
extern volatile int16_t Circle_value;
extern volatile uint8_t AS5600_RAW_L;
extern volatile uint8_t AS5600_RAW_H;
extern volatile int16_t TEMP_Speed;
extern volatile uint8_t Position_Inc_Seq_Ring_Full;
extern volatile int16_t M_Last_Speed;
extern volatile uint8_t Speed_Filter_Ring_Full;
extern volatile int16_t Speed_Filter_Ring[Speed_Filter_MAX];
extern volatile uint8_t Speed_Filter_Index;
extern volatile int16_t Speed_Filter_SUM;
extern volatile int16_t M_Filter_Speed;
extern volatile int16_t Correct_Position;

void IIC_Init(void);
void IIC_interrupt_Init(void);
uint8_t AS5600_Init(void);
uint16_t AS5600_read_POLL(void);
void Read_AS5600_Start(void);

#endif

/*
	uint16_t TEMP, ODAT;
	TEMP = AS5600_read_POLL();
	if(ODAT != TEMP)
	{
		UartSendGroup((u8 *)printBuf, sprintf(printBuf, "%d \r\n", TEMP));
		ODAT = TEMP;
	}
	delay_ms(10);
*/

/* INIT:
	TEMP = AS5600_Init();
    if(TEMP != 0)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf, "AS5600 Error with code %d\r\n", TEMP));
    }
*/

//IIC AS5600 RTtest
//		TEMP = AS5600_read_POLL();
//		if(ODAT != TEMP)
//		{
//			UartSendGroup((u8 *)printBuf, sprintf(printBuf, "%d \r\n", TEMP));
//			ODAT = TEMP;
//		}
		//Read_AS5600_Start();
		//delay_ms(10);
//		if(AS5600_Data_Ready == 1)
//		{
//			//UartSendGroup((u8 *)printBuf, sprintf(printBuf, "%d \r\n", Last_Position));
//			while((I2C_GetITStatus(I2C1, I2C_IT_STOP_DET)) == 0);
//			AS5600_Data_Ready = 0;
//		}
		//UartSendGroup((u8 *)printBuf, sprintf(printBuf, "%x, %x \r\n", AS5600_H8bit_data, AS5600_L8bit_data));
		//while((I2C_GetITStatus(I2C1, I2C_IT_STOP_DET)) == 0);
		
