/**********************************************************************************
 * @name        ADC.c
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/4/23
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "ADC.h"

uint16_t ADC_RAW_Data = 0;
uint8_t ADC_RAW_Data_ready = 0;

void ADC_Single_Init(void)
{
	//GPIO_init
    GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                         //GPIO clock starts
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                         //Enable ADC clock
    
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_1;                                 //pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           //Output speed
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                               //GPIO mode
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_16;                     //ADC prescale factor
    ADC_InitStructure.ADC_Mode = ADC_Mode_Single;                      			//Set ADC mode to continuous conversion mode
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //AD data right-justified
    ADC_InitStructure.ADC_TRGEN = ADC_TRG_Disable;								//Disable trigger mode
    ADC_Init(ADC1, &ADC_InitStructure);

    //	ADC_RegularChannelConfig(ADC, ADC_Channel_All_Disable, 0, ADC_SampleTime_13_5Cycles);//Block all channels
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 0, ADC_SampleTime_239_5Cycles);//Enable the channel
	
	//ADC_SampleTime_239_5Cycles ~ 88us
	//ADC_SampleTime_1_5Cycles ~ 5us

    //Enable ADCDMA
    ADC_Cmd(ADC1, ENABLE);                                                       //Enable AD conversion
}

//5/13test Error!
uint16_t ADC1_SingleChannel_Get_(void)
{
//    u16 puiADData;
    /*ADCR????ADST???,??????*/
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC1->ADCR |= ADCR_SWSTART_Set;
//    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
	while(!(ADC1->ADSTA & ADC_FLAG_EOC))
//    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC1->ADSTA |= ADC_FLAG_EOC;
//    puiADData = ADC_GetConversionValue(ADC1);
//	puiADData = (uint16_t)ADC1->ADDATA;
//    return puiADData;
	return (uint16_t)ADC1->ADDATA;
}

uint16_t ADC1_SingleChannel_Get(void)
{
    u16 puiADData;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    puiADData = ADC_GetConversionValue(ADC1);
    return puiADData;
}

void ADC_Start(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t ADC_Data_Get(void)
{
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	return ADC_GetConversionValue(ADC1);
}
