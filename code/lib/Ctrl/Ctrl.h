/**********************************************************************************
 * @name        Ctrl.h
 * @version     R1.1
 * @author      Z2Z GuGu
 * @date        2022/5/24
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#ifndef _CTRL_H_
#define _CTRL_H_

#include "AS5600.h"
#include "SVPWM.h"
#include "parameter.h"
#include "Basic.h"
#include "APP.h"
#include "main.h"

// #define PID_P 1
// #define PID_I 0.5
// #define PID_D 0
#define PID_I_limit     30000

#define Error_Seq_Ring_MAX   20

typedef struct PID
{ 
  float P,I,D,limit;
}PID;
 
typedef struct Error
{
  int16_t Current_Error;//当前误差
  int16_t Last_Error;//上一次误差
  int16_t Previous_Error;//上上次误差
}Error;

void Set_Ctrl_Parma(uint8_t Mode, uint32_t val);
void Ctrl_Parameter_Init(void);
void MY_Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed);
void Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed);
int16_t Motor_Increment_Position_Ctrl(int16_t INPUT_Position, int16_t NOW_Position);

extern float C_Vref;

#endif
