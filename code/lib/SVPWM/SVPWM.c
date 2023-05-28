/**********************************************************************************
 * @name        SVPWM.c
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/2/12
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "SVPWM.h"

/*******************************************************************************
* @name   : SVPWM_Init
* @brief  : Initialize SVPWM
* @param  : None
* @retval : void
*******************************************************************************/
void SVPWM_Init(void)
{
    initGPIO_Timer1();
    //initTIM1(0, 1023, 0);
	//initTIM1(47, 999, 500);
	Timers_Init(SVPWM_prescaler, SVPWM_cycle, Control_cycle);
    Timer_NVIC_Init();
	SVPWM_config(500, 200, 100);
}

/*******************************************************************************
* @name   : SVPWM_config
* @brief  : Set Tim1 3ch PWM duty
* @param  : A, B, C ch`s PWM duty
* @retval : void
*******************************************************************************/
// uint8_t Aformode, Bformode, Cformode;
void SVPWM_config(uint16_t A_duty, uint16_t B_duty, uint16_t C_duty)	    //1-2---1021-1022
{
	TIM_SetCompare1(TIM1, A_duty);
	TIM_SetCompare2(TIM1, B_duty);
	TIM_SetCompare3(TIM1, C_duty);
	
	/*
    // PMWpluse = (500 * duty) / 1000;
    if((A_duty > 1) && (A_duty < 1022)) 
    {
        if(Aformode == 1) 
        {
            Aformode = 0;
            TIM_ForcedOC1Config(TIM1, TIM_OCMode_PWM2);
        }
        TIM_SetCompare1(TIM1, A_duty);
    }
    else if(A_duty <= 1)
    {
        Aformode = 1;
        TIM_ForcedOC1Config(TIM1, TIM_ForcedAction_Active);
    }
    else
    {
        Aformode = 1;
        TIM_ForcedOC1Config(TIM1, TIM_ForcedAction_Active);
    }
    //-----------------------------------------------------------
    if((B_duty > 1) && (B_duty < 1022)) 
    {
        if(Bformode == 1) 
        {
            Bformode = 0;
            TIM_ForcedOC2Config(TIM1, TIM_OCMode_PWM2);
        }
        TIM_SetCompare2(TIM1, B_duty);
    }
    else if(B_duty <= 1)
    {
        Bformode = 1;
        TIM_ForcedOC2Config(TIM1, TIM_ForcedAction_Active);
    }
    else
    {
        Bformode = 1;
        TIM_ForcedOC2Config(TIM1, TIM_ForcedAction_Active);
    }
    //-----------------------------------------------------------
    if((C_duty > 1) && (C_duty < 1022)) 
    {
        if(Cformode == 1) 
        {
            Cformode = 0;
            TIM_ForcedOC3Config(TIM1, TIM_OCMode_PWM2);
        }
        TIM_SetCompare3(TIM1, C_duty);
    }
    else if(C_duty <= 1)
    {
        Cformode = 1;
        TIM_ForcedOC3Config(TIM1, TIM_ForcedAction_Active);
    }
    else
    {
        Cformode = 1;
        TIM_ForcedOC3Config(TIM1, TIM_ForcedAction_Active);
    }
	*/
}

/*******************************************************************************
* @name   : initGPIO_Timer1
* @brief  : Initialize TIM1 GPIO
* @param  : None
* @retval : void
*******************************************************************************/
void initGPIO_Timer1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3,  GPIO_AF_6);           //AH -- CH1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_2);           //AL -- CH1N
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4,  GPIO_AF_6);           //BH -- CH2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_2);           //BL -- CH2N
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0,  GPIO_AF_4);           //CH -- CH3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5,  GPIO_AF_6);           //CL -- CH3N

    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3 | GPIO_Pin_13 | GPIO_Pin_4 | GPIO_Pin_14 | GPIO_Pin_0;  //CH1, CH1N, CH2, CH2N, CH3, 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_5;                    //CH3N
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* @name   : initTIM1
* @brief  : Initialize TIM1
* @param  : u16 prescaler, u16 period, u16 control_cycle
* @retval : void
*******************************************************************************/
void Timers_Init(u16 prescaler, u16 period, u16 control_cycle)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//Init TIM1 16bit Timer
    TIM_TimeBaseStructInit(&TIM_StructInit);
    TIM_StructInit.TIM_Period = period - 1;
    TIM_StructInit.TIM_Prescaler = prescaler;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_CenterAligned1;		//TIM_CounterMode_Center
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_StructInit);
	
	//Synchronization signal(P207)
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);		//Open the TIM1 TRGO OUT (for Timer3)MMS = 010
	TIM_ITRxExternalClockConfig(TIM3, TIM_TS_ITR0);				//config TIM3 SMCR TS = 000
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);			//config TIM3 SMCR SMS = 111
	
	//Use Timer1 start Timer 3(P208)
//	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);		//Open the TIM1 TRGO OUT (for Timer3)MMS = 010
//	TIM_ITRxExternalClockConfig(TIM3, TIM_TS_ITR0);				//config TIM3 SMCR TS = 000
//	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Trigger);			//config TIM3 SMCR SMS = 110
	
	//Init TIM3 16bit Timer
	TIM_StructInit.TIM_Period = control_cycle * 2 - 1;
    TIM_StructInit.TIM_Prescaler = 0;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Down;		//TIM3_CounterMode_Up
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_StructInit);
	
	//TIM1 BDT config
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;		//运行模式下输出选择
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;		//空闲模式下输出选择
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;         	//锁定设置
	TIM_BDTRInitStructure.TIM_DeadTime = 0x09;                       	//死区时间设置	n ~ n*20.8ns
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;             	//刹车功能使能
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;	//刹车输入极性
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//自动输出使能
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);  

	//TIM1 OC config
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;				//TIM_OCMode_PWM2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = Start_dutyCycle;				//main interrupt
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//TIM_OCPolarity_High
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;		//TIM_OCNPolarity_High
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//TIM1 OC4/5 config
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;				//TIM_OCMode_PWM2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = Start_dutyCycle;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//TIM_OCPolarity_High
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;		//TIM_OCNPolarity_High
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC5Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC5PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//TIM3 OC1 config
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;				//TIM_OCMode_PWM2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//TIM_OCPolarity_High
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;		//TIM_OCNPolarity_High
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	//TIMx ARR Preload config
    TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//TIM interrupt config
//  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
//	TIM_ITConfig(TIM1, TIM_IT_COM, ENABLE);
//	TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
	
//	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
//	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
//	TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);
//	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);

	//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

	//TIM1 PWM Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	//TIMx time base Enable
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void TIM2_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_StructInit;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//Init TIM1 16bit Timer
    TIM_TimeBaseStructInit(&TIM_StructInit);
    TIM_StructInit.TIM_Period = 9999;		//10ms
    TIM_StructInit.TIM_Prescaler = 47;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_CenterAligned1;		//TIM_CounterMode_Center
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_StructInit);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                          // Config TIM1_CC_IRQn NVIC config
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                             // PreemptionPriority : 1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // Enable IRQn
    NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

/*******************************************************************************
* @name   : initNVIC_TIM1
* @brief  : Initialize TIM1 NVIC
* @param  : None
* @retval : void
*******************************************************************************/
void Timer_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                          // Config TIM1_CC_IRQn NVIC config
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                             // PreemptionPriority : 1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // Enable IRQn
    NVIC_Init(&NVIC_InitStructure);
	
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;                          // Config TIM1_CC_IRQn NVIC config
//    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                             // PreemptionPriority : 1
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // Enable IRQn
//    NVIC_Init(&NVIC_InitStructure);
	
//	  NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;                          // Config TIM1_CC_IRQn NVIC config
//    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;                             // PreemptionPriority : 1
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // Enable IRQn
//    NVIC_Init(&NVIC_InitStructure);
}


void SVPWM_Stop(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	
	GPIO_PinLockConfig(GPIOB, GPIO_PinSource3);             //AH -- CH1
    GPIO_PinLockConfig(GPIOB, GPIO_PinSource13);            //AL -- CH1N
    GPIO_PinLockConfig(GPIOB, GPIO_PinSource4);             //BH -- CH2
    GPIO_PinLockConfig(GPIOB, GPIO_PinSource14);            //BL -- CH2N
    GPIO_PinLockConfig(GPIOB, GPIO_PinSource0);             //CH -- CH3
    GPIO_PinLockConfig(GPIOA, GPIO_PinSource5);             //CL -- CH3N
}

void SVPWM_Start(void)
{
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3,  GPIO_AF_6);           //AH -- CH1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_2);           //AL -- CH1N
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4,  GPIO_AF_6);           //BH -- CH2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_2);           //BL -- CH2N
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0,  GPIO_AF_4);           //CH -- CH3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5,  GPIO_AF_6);           //CL -- CH3N
}


//old code

const uint16_t sinm[98] = {0, 22, 44, 66, 87, 109, 131, 153, 175, 197, 218, 240, 262, 
	283, 305, 327, 348, 370, 391, 413, 434, 456, 477, 498, 519, 540, 561, 582, 603, 
	624, 645, 666, 686, 707, 727, 748, 768, 788, 809, 829, 849, 868, 888, 908, 927, 
	947, 966, 985, 1004, 1024, 1042, 1061, 1080, 1098, 1117, 1135, 1153, 1171, 1189, 
	1207, 1224, 1242, 1259, 1276, 1293, 1310, 1327, 1344, 1360, 1376, 1392, 1408, 1424, 
	1440, 1455, 1470, 1486, 1501, 1515, 1530, 1544, 1559, 1573, 1587, 1600, 1614, 1627, 
	1640, 1653, 1666, 1679, 1691, 1704, 1716, 1727, 1739, 1750, 1762};

void SVPWM(uint8_t m, uint16_t n)	//分区（0 ~ 5）、角度n（0 ~ 97）
{
	uint16_t T0, T1, T2;
	T1 = C_Vref * sinm[97 - n];
	T2 = C_Vref * sinm[n];
	T0 = 1024 - 0.5 * (T1 + T2);
	T0 /= 2;
	T1 /= 2;
	T2 /= 2;
	
	switch(m)
	{
		case 0:
			SVPWM_config(T1 + T2 + T0, T2 + T0, T0);
			break;
		case 1:
			SVPWM_config(T1 + T0, T1 + T2 + T0, T0);
			break;
		case 2:
			SVPWM_config(T0, T1 + T2 + T0, T2 + T0);
			break;
		case 3:
			SVPWM_config(T0, T1 + T0, T1 + T2 + T0);
			break;
		case 4:
			SVPWM_config(T2 + T0, T0, T1 + T2 + T0);
			break;
		case 5:
			SVPWM_config(T1 + T2 + T0, T0, T1 + T0);
			break;
	}
}

/*******************************************************************************
* @name   : FOAC
* @brief  : 极坐标to磁矢量 = FOC uPark + uClark
* @param  : None
* @retval : void
*******************************************************************************/
void FOAC(int16_t angle)	//angle~(0,4095)
{
	uint16_t TEMP;
    if(angle < 0)
    {
        angle += 4096;
    }
    TEMP = (uint16_t)(angle * 1.005) % 588;
	SVPWM(TEMP / 98, TEMP % 98);
//	delay_10us(10);
}
