/**********************************************************************************
 * @name        APP.h
 * @version     R1.2
 * @author      Z2Z GuGu
 * @date        2023/5/26
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/
#ifndef _APP_H_
#define _APP_H_

#include "AS5600.h"
#include "SVPWM.h"
#include "parameter.h"
#include "Basic.h"
#include "Ctrl.h"
#include "main.h"
#include "UART.h"

#define Motor_IDs_START_ADDR    ((uint32_t)0x08003C00)
#define P_ID_FLASH_ADDR         ((uint32_t)0x08003C00)
#define G_ID_FLASH_ADDR         ((uint32_t)0x08003C04)
#define G_INDEX_FLASH_ADDR      ((uint32_t)0x08003C08)

// #define Default_P_ID            ((uint8_t)0x30)     // '0'
// #define Default_G_ID            ((uint8_t)0x80)     // '0'
// #define Default_G_INDEX         ((uint8_t)0x00)     // 0, 1, 2

#define Default_P_ID            ((uint8_t)0x32)     // '2'
#define Default_G_ID            ((uint8_t)0x80)     // '0'
#define Default_G_INDEX         ((uint8_t)0x01)     // 0, 1, 2

#define ID_Kind_Bit             0x80
#define Data_Formate_Bit        0x80
#define MODE_MASK               0x7F

#define Speed_Ctrl_Mode         0x53        // 'S'
#define Position_Ctrl_Mode      0x50        // 'P'
#define Friction_Ctrl_Mode      0x46        // 'F'
#define Knob_Ctrl_Mode          0x4B        // 'K'
#define SET_P_ID_Mode           0x70        // Set P-ID
#define SET_G_ID_Mode           0x67        // Set G-ID
#define SET_G_Index_Mode        0x69        // Set G-Index
#define FEEDBACK_Auto_Mode      0x61        // Auto Mode
#define FEEDBACK_SPEED_Mode     0x73        // OUTPUT Speed
#define FEEDBACK_POSITION_Mode  0x77        // OUTPUT Position
#define STOP_FEEDBACK_Mode      0x7A        // STOP OUTPUT
#define SET_Serial_SPEED_Mode   0x62        // Set Serial bps

#define SET_S_PID_P_Mode        0x20        // 
#define SET_S_PID_I_Mode        0x21        // 
#define SET_S_PID_D_Mode        0x22        // 
#define SET_S_PID_L_Mode        0x23        // 
#define SET_S_OUT_L_Mode        0x24        // 
#define SET_S_IN_L_Mode         0x25        // 
#define SET_P_PID_P_Mode        0x26        // 
#define SET_P_PID_I_Mode        0x27        // 
#define SET_P_PID_D_Mode        0x28        // 
#define SET_P_PID_L_Mode        0x29        //

void delay_10us(uint32_t _t);
void Get_new_Data(void);
void Write_IDs_To_Flash(void);
void Read_IDs_From_Flash(void);
void ID_Flash_Init(void);
uint16_t SCIO_Data_Analysis(void);
void FeedBack_Process(void);
void Flash_Process(void);
void SCIO_Process(void);
void AS5600_Process(void);
void Ctrl_Process(void);

#endif
