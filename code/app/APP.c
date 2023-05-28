/**********************************************************************************
 * @name        APP.c
 * @version     R1.2
 * @author      Z2Z GuGu
 * @date        2023/5/26
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "APP.h"

//extern:
//ADC_RAW_Data_ready
//ADC_RAW_Data
//AS5600_Data_Ready
//Last_Position

uint8_t P_ID, G_ID, G_Index;
uint8_t Update_ID_to_Flash = 0;
uint8_t Feedback_Mode = 0;
uint8_t New_Print = 0;

int16_t Current_Increment = 0;
int32_t Position_Ref;
uint16_t Knob_Angle = 0;

int16_t Point_Speed = 0;
int16_t Current_Speed = 0;
int16_t Point_Position = 0;
int32_t Current_Position = 0;
uint8_t Ctrl_Mode = 5;
uint8_t scio_new_data = 0;


void delay_10us(uint32_t _t)
{
	_t *= 7;
	while(_t--);
}

void Get_new_Data(void)
{
    New_Print = 1;
}

void Write_IDs_To_Flash(void)
{
	FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(Motor_IDs_START_ADDR);
    FLASH_ClearFlag(FLASH_FLAG_EOP );

    FLASH_ProgramWord(P_ID_FLASH_ADDR, (uint32_t)P_ID);
    FLASH_ProgramWord(G_ID_FLASH_ADDR, (uint32_t)G_ID);
    FLASH_ProgramWord(G_INDEX_FLASH_ADDR, (uint32_t)G_Index);

    FLASH_ClearFlag(FLASH_FLAG_EOP );
    FLASH_Lock();
}

void Read_IDs_From_Flash(void)
{
	P_ID = (uint8_t)(*(__IO uint32_t *)P_ID_FLASH_ADDR);
	G_ID = (uint8_t)(*(__IO uint32_t *)G_ID_FLASH_ADDR);
	G_Index = (uint8_t)(*(__IO uint32_t *)G_INDEX_FLASH_ADDR);
}

void ID_Flash_Init(void)
{
	P_ID = Default_P_ID;
	G_ID = Default_G_ID;
	G_Index = Default_G_INDEX;
	Write_IDs_To_Flash();
}

uint16_t SCIO_Data_Analysis(void)
{
	uint8_t G_ID_Mode = 0;
	uint16_t TEMP_Param;
	uint32_t TEMP_Param_32t;
	uint16_t TEMP_Mode;
	// ID
	if((rxdata[1] & ID_Kind_Bit) == 0x80)
	{	// G-ID
		if(rxdata[1] != G_ID)
		{
			return 0xFFFF;
		}
		G_ID_Mode = 1;
	}
	else
	{	// P-ID
		if(rxdata[1] != P_ID)
		{
			return 0xFFFF;
		}
	}

	// Data Formate
	if((rxdata[0] & Data_Formate_Bit) == 0x80)
	{	// Q-Data
		if(G_ID_Mode)
		{	// G_ID_Mode
			TEMP_Param = (rxdata[(G_Index + 1) * 2] << 8) | rxdata[(G_Index + 1) * 2 + 1];
		}
		else
		{	// P_ID_Mode
			TEMP_Param = (rxdata[2] << 8) | rxdata[3];
		}
	}
	else
	{	// S-Data
		TEMP_Param = (rxdata[7] - '0') + (rxdata[6] - '0') * 10 + (rxdata[5] - '0') * 100 + (rxdata[4] - '0') * 1000 + (rxdata[3] - '0') * 10000;
		if(rxdata[2] == '-')
		{
			TEMP_Param = -TEMP_Param;
		}
	}
	
	// MODE
	TEMP_Mode = rxdata[0] & MODE_MASK;
	switch(TEMP_Mode)
	{
		case Speed_Ctrl_Mode:
			Point_Speed = TEMP_Param;
			Ctrl_Mode = 0;
			return Ctrl_Mode;
		// break;
		case Position_Ctrl_Mode:
			Point_Position = TEMP_Param;
			Ctrl_Mode = 1;
			return Ctrl_Mode;
		// break;
		case Friction_Ctrl_Mode:
			Point_Speed = 0;
			Ctrl_Mode = 5;
			return Ctrl_Mode;
		// break;
		case Knob_Ctrl_Mode:
			Knob_Angle = TEMP_Param;
			Ctrl_Mode = 1;
			return Ctrl_Mode;
		// break;
		case SET_P_ID_Mode:
			P_ID = (uint8_t)TEMP_Param;
			Update_ID_to_Flash = 1;
			return TEMP_Param;
		// break;
		case SET_G_ID_Mode:
			G_ID = (uint8_t)TEMP_Param;
			Update_ID_to_Flash = 1;
			return TEMP_Param;
		// break;
		case SET_G_Index_Mode:
			G_Index = (uint8_t)TEMP_Param;
			Update_ID_to_Flash = 1;
			return TEMP_Param;
		// break;
		case STOP_FEEDBACK_Mode:
			Feedback_Mode = 0;
			return TEMP_Param;
		// break;
		case FEEDBACK_Auto_Mode:
			Feedback_Mode = 1;
			return TEMP_Param;
		// break;
		case FEEDBACK_SPEED_Mode:
			Feedback_Mode = 2;
			return TEMP_Param;
		// break;
		case FEEDBACK_POSITION_Mode:
			Feedback_Mode = 3;
			return TEMP_Param;
		// break;
		case SET_Serial_SPEED_Mode:
			LED1_OFF();
			TEMP_Param_32t = (rxdata[4] << 24) | (rxdata[5] << 16) | (rxdata[6] << 8) | rxdata[7];
			Serial_Bps_Setting(TEMP_Param_32t);
			return TEMP_Param;
		// break;
		default:
			TEMP_Param_32t = (rxdata[4] << 24) | (rxdata[5] << 16) | (rxdata[6] << 8) | rxdata[7];
			Set_Ctrl_Parma(TEMP_Mode, TEMP_Param_32t);
			return TEMP_Param;
		// break;
	}
	// return 0;
}

void FeedBack_Process(void)
{
    if(Feedback_Mode)
    {
        TX_Mode_Setting(1);
        switch(Feedback_Mode)
        {
            case 1:	// Auto
                if(New_Print)
                {
                    /*
                    *	Last_Position
                    *  Correct_Position
                    *	D_Last_Increment
                    *	M_Filter_Speed
                    */
                    switch(Ctrl_Mode)
                    {
                        case 0: // Speed
                            UartSendGroup((u8 *)printBuf, sprintf(printBuf, "1600,-1600,P:%d,C:%d\r\n", Point_Speed, Current_Speed));
                            break;
                        case 3: // Position
                            UartSendGroup((u8 *)printBuf, sprintf(printBuf, "1000,-1000,P:%d,C:%d\r\n", Point_Position, Current_Position));
                            break;
                        case 5: // Speed
                            UartSendGroup((u8 *)printBuf, sprintf(printBuf, "1600,-1600,P:%d,C:%d\r\n", Point_Speed, Current_Speed));
                            break;
                    }
                    New_Print = 0;
                }
            break;
            case 2:	// OUTPUT Speed
                UartSendGroup((u8 *)printBuf, sprintf(printBuf, "S:%d\r\n", Current_Speed));
            break;
            case 3:	// OUTPUT Position
                UartSendGroup((u8 *)printBuf, sprintf(printBuf, "P:%d\r\n", Last_Position));
            break;
        }
    }
    else
    {
        TX_Mode_Setting(0);
    }
}

void Flash_Process(void)
{
    if(Update_ID_to_Flash)
    {
        Update_ID_to_Flash = 0;
        Write_IDs_To_Flash();
    }
}

void SCIO_Process(void)
{
	// SCIO
	scio_new_data = UART2_updata_DMA_Rbuf();
//	scio_new_data = UART2_updata_DMA_Rbuf(rxdata, RX_LEN);
	if(scio_new_data == 1)
	{
		scio_new_data = 0;
		Knob_Angle = 0;
		SCIO_Data_Analysis();
		UART2_DMA_Readbuf_Start(rxdata, RX_LEN);
	}
}

void AS5600_Process(void)
{
		if(Speed_Filter_Ring_Full && AS5600_Data_Ready)
	{
		// Speed Ring
		Current_Speed = M_Filter_Speed;

		// Position
		// Current_Increment = 0;
		switch(Ctrl_Mode)
		{
			case 1:	// 相对位置环：归零
				Current_Position = 0;
				Circle_value = 0;
				// if(Knob_Angle != 0)
				// {
				// 	Point_Position = 0;
				// }
				Ctrl_Mode = 2;
				break;
			case 2:	// 相对位置环：获取相对基准
				Position_Ref = Absolute_Position;
				Point_Position += Position_Ref;
				Ctrl_Mode = 3;
				break;
			case 3:	// 相对位置环：获取当前值
				Current_Position = Absolute_Position;
				if(Knob_Angle != 0)
				{	// 卡位旋钮
					if((Point_Position - Current_Position) > (Knob_Angle / 2))
					{
						Point_Position -= Knob_Angle;
					}
					else if((Current_Position - Point_Position) > (Knob_Angle / 2))
					{
						Point_Position += Knob_Angle;
					}
				}
				break;
			case 4:	// 绝对位置环
				Current_Position = Last_Position;
				break;
			case 5:	// 有摩擦
				Point_Speed = Current_Speed;
				break;
			case 6:	// 有摩擦
				Point_Speed = Current_Speed;
				break;
		}

		Speed_Filter_Ring_Full = 0;
		AS5600_Data_Ready = 0;
		Get_new_Data();
	}
	Read_AS5600_Start();
}

void Ctrl_Process(void)
{
	switch(Ctrl_Mode)
	{
		case 0:	// Speed
			Motor_Speed_Ctrl(Point_Speed, Current_Speed);
		break;
		case 3:	// Increment Position
			Point_Speed = Motor_Increment_Position_Ctrl(Point_Position, Current_Position);
			Motor_Speed_Ctrl(Point_Speed, Current_Speed);
		break;
		case 4:	// Position
			Point_Speed = Motor_Increment_Position_Ctrl(Point_Position, Current_Position);
			Motor_Speed_Ctrl(Point_Speed, Current_Speed);
		break;
		case 5:	// 有摩擦
			Motor_Speed_Ctrl(Point_Speed, Current_Speed);
		break;
	}
}

//main rhythm Interrupt handler
// 341us
void TIM3_IRQHandler()
{	
	// LED1_TOGGLE();
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

	// SCIO
	SCIO_Process();

	// AS5600
	AS5600_Process();
	// Control
	Ctrl_Process();
}

//TEST
void TIM2_IRQHandler()
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	// LED1_TOGGLE();
}

	// INA180
//	ADC_RAW_Data = ADC_Data_Get();
//	ADC_RAW_Data_ready = 1;
//	ADC_Start();
