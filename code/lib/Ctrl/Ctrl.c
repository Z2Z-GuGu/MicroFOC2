/**********************************************************************************
 * @name        Ctrl.c
 * @version     R1.1
 * @author      Z2Z GuGu
 * @date        2022/5/24
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "Ctrl.h"

uint16_t SPEED_IN_limit = 500;
float SPEED_OUT_limit = 1.0;

// // 带载
// PID Speed_PID_Param = {
//     5, // P
//     0.03,	// I
//     0,	// D
//     1000,	// limit
// };
// 空载
PID Speed_PID_Param = {
    0.3, // P
    0.02,	// I
    15,	// D
    1000,	// limit
};

Error Speed_PID_Error = {
	0,	//Current_Error
	0,	//Last_Error
	0,  //Previous_Error
};

// // 带载
// PID Position_PID_Param = {
//     0.5, // P
//     0,	// I
//     0.5,	// D
//     0,	// limit
// };
// 空载
PID Position_PID_Param = {
    0.2, // P
    0,	// I
    0,	// D
    0,	// limit
};

Error Position_PID_Error = {
	0,	//Current_Error
	0,	//Last_Error
	0,  //Previous_Error
};

// int16_t Error_Seq_Ring[Error_Seq_Ring_MAX];
// uint8_t Error_Seq_Index;
// int16_t Error_Seq_SUM;
/**********************************************************************************
 * @name        Set_Ctrl_Parma
 * @author      Z2Z GuGu
 * @brief       设置控制参数
 * @param       Mode    设置哪个
 * @param       val     数值
 **********************************************************************************/
void Set_Ctrl_Parma(uint8_t Mode, uint32_t val)
{
    switch(Mode)
    {
        case SET_S_PID_P_Mode:
            Speed_PID_Param.P = (float)val;
            break;
        case SET_S_PID_I_Mode:
            Speed_PID_Param.I = (float)val;
            break;
        case SET_S_PID_D_Mode:
            Speed_PID_Param.D = (float)val;
            break;
        case SET_S_PID_L_Mode:
            Speed_PID_Param.limit = (float)val;
            break;
        case SET_P_PID_P_Mode:
            Position_PID_Param.P = (float)val;
            break;
        case SET_P_PID_I_Mode:
            Position_PID_Param.I = (float)val;
            break;
        case SET_P_PID_D_Mode:
            Position_PID_Param.D = (float)val;
            break;
        case SET_P_PID_L_Mode:
            Position_PID_Param.limit = (float)val;
            break;
        case SET_S_OUT_L_Mode:
            SPEED_OUT_limit = (float)val;
            break;
        case SET_S_IN_L_Mode:
            SPEED_IN_limit = (uint16_t)val;
            break;
        default:
            break;
    }
}

/**********************************************************************************
 * @name        Ctrl_Parameter_Init
 * @author      Z2Z GuGu
 * @brief       初始化控制参数，已弃用
 **********************************************************************************/
void Ctrl_Parameter_Init(void)
{
    // uint8_t i;
    // for (i = 0; i < Error_Seq_Ring_MAX; i++)
    // {
    //     Error_Seq_Ring[i] = 0;
    // }
    // Error_Seq_Index = 0;
    // Error_Seq_SUM = 0;
}

/**********************************************************************************
 * @name        PID_Speed
 * @author      Z2Z GuGu
 * @brief       位置式PID，速度环优化
 * @param       sptr    误差参数
 * @param       pid     PID参数
 * @param       NowPlace    当前位置
 * @param       Point   预期位置
 **********************************************************************************/
float PID_Speed(Error *sptr,PID *pid, int16_t NowPlace, int16_t Point)
{
 
	int16_t iError;	// 当前误差
	float Realize;  // 实际输出	
 
	iError = Point - NowPlace;	// 计算当前误差
    // Error_Seq_SUM = Error_Seq_SUM - Error_Seq_Ring[Error_Seq_Index] + iError;
    // Error_Seq_Ring[Error_Seq_Index] = iError;
    // + pid->I * Error_Seq_SUM   //积分I
    // 上面过程自动限幅
    sptr->Current_Error += pid->I * iError;	// 误差积分
      sptr->Current_Error = sptr->Current_Error > pid->limit?pid->limit:sptr->Current_Error;//积分限幅
      sptr->Current_Error = sptr->Current_Error <-pid->limit?-pid->limit:sptr->Current_Error;
	Realize = pid->P * iError       //比例P
            + sptr->Current_Error   //积分I
			+ pid->D * (iError - sptr->Last_Error);  //微分D
	sptr->Last_Error = iError;		  	// 更新上次误差
	return Realize;	// 返回实际值
}

/**********************************************************************************
 * @name        PID_Position
 * @author      Z2Z GuGu
 * @brief       位置式PID，位置环优化
 * @param       sptr    误差参数
 * @param       pid     PID参数
 * @param       NowPlace    当前位置
 * @param       Point   预期位置
 **********************************************************************************/
int16_t PID_Position(Error *sptr,PID *pid, int16_t NowPlace, int16_t Point)
{
 
	int16_t iError;	// 当前误差
	float Realize;  // 实际输出	
 
	iError = Point - NowPlace;	// 计算当前误差
    sptr->Current_Error += pid->I * iError;	// 误差积分
	Realize = pid->P * iError       //比例P
            + sptr->Current_Error   //积分I
			+ pid->D * (iError - sptr->Last_Error);  //微分D
	sptr->Last_Error = iError;		  	// 更新上次误差
	return (int16_t)Realize;	// 返回实际值
}

/**********************************************************************************
 * @name        MY_Motor_Speed_Ctrl
 * @author      Z2Z GuGu
 * @brief       位置式PI测试，测试完成，已弃用
 * @param       INPUT_Speed     预期位置
 * @param       NOW_Speed       当前位置
 **********************************************************************************/
float C_Vref = 0.0;
float PID_P = 2;
float PID_I = 0.03;
float PID_D = 0;
void MY_Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed)
{
    static int16_t ALL_Error;
    int16_t Speed_Error;
    float Realize;	//最后得出的实际增量

    // PID
    Speed_Error = INPUT_Speed - NOW_Speed;
    ALL_Error += Speed_Error;
    if(ALL_Error > PID_I_limit)     ALL_Error = PID_I_limit;
    if(ALL_Error < -PID_I_limit)    ALL_Error = -PID_I_limit;

    Realize = (PID_P * Speed_Error) + (PID_I * ALL_Error);
    Realize /= 1500;

    if(Realize > SPEED_OUT_limit)     Realize = SPEED_OUT_limit;
	if(Realize < -SPEED_OUT_limit)    Realize = -SPEED_OUT_limit;

    // OUTPUT
    if(Realize > 0) 
    {
        C_Vref = Realize;
        FOAC(Last_Position - 1000);
    }
    else
    {
        C_Vref = -Realize;
        FOAC(Last_Position + 1000);
    }
}

/**********************************************************************************
 * @name        Motor_Speed_Ctrl
 * @author      Z2Z GuGu
 * @brief       速度环，电机控制
 * @param       INPUT_Speed     预期位置
 * @param       NOW_Speed       当前位置
 **********************************************************************************/
void Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed)
{
    float Realize;	//最后得出的实际增量

    // PID
    Realize = PID_Speed(&Speed_PID_Error, &Speed_PID_Param, NOW_Speed, INPUT_Speed);
    Realize /= 1500;

    if(Realize > SPEED_OUT_limit)     Realize = SPEED_OUT_limit;
	if(Realize < -SPEED_OUT_limit)    Realize = -SPEED_OUT_limit;

    // OUTPUT
    if(Realize > 0.0001) 
    {
        C_Vref = Realize;
        FOAC(Last_Position - 1000);
    }
    else if(Realize < -0.0001)
    {
        C_Vref = -Realize;
        FOAC(Last_Position + 1000);
    }
    else
    {
        C_Vref = 0;
        FOAC(Last_Position + 1000);
    }
}

/**********************************************************************************
 * @name        Motor_Increment_Position_Ctrl
 * @author      Z2Z GuGu
 * @brief       位置环（采用位置增量），电机控制
 * @param       INPUT_Position     预期位置
 * @param       NOW_Position       当前位置
 **********************************************************************************/
int16_t Motor_Increment_Position_Ctrl(int16_t INPUT_Position, int16_t NOW_Position)
{
    int16_t Realize;	//最后得出的实际增量

    // PID
    Realize = PID_Position(&Position_PID_Error, &Position_PID_Param, NOW_Position, INPUT_Position);

    // Realize /= 2;

    if(Realize > SPEED_IN_limit)     Realize = SPEED_IN_limit;
	if(Realize < -SPEED_IN_limit)    Realize = -SPEED_IN_limit;

    // OUTPUT
    return Realize;
}
