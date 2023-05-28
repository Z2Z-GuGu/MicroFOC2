/**********************************************************************************
 * @name        main.c
 * @version     R2.1
 * @author      Z2Z GuGu
 * @date        2022/04/29
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/
#include "main.h"

int main(void)
{
	delay_init();

	LED_Init();
	LED1_OFF();
    KEY_Init();

	ID_Flash_Init();
	Read_IDs_From_Flash();

	// UART
	SCIO_Init();

	//ADC
//	ADC_Single_Init();
//	ADC_Start();
	
	// AS5600
	AS5600_Init();
	IIC_interrupt_Init();

	delay_ms(500);

	// TIMx
	// TIM2_Init();

	Ctrl_Parameter_Init();
	SVPWM_Init();
	FOAC(0);
	
    while(1) 
	{
		Flash_Process();
		FeedBack_Process();
    }
	return 0;
}
