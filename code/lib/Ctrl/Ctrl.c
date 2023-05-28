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

// // ����
// PID Speed_PID_Param = {
//     5, // P
//     0.03,	// I
//     0,	// D
//     1000,	// limit
// };
// ����
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

// // ����
// PID Position_PID_Param = {
//     0.5, // P
//     0,	// I
//     0.5,	// D
//     0,	// limit
// };
// ����
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
 * @brief       ���ÿ��Ʋ���
 * @param       Mode    �����ĸ�
 * @param       val     ��ֵ
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
 * @brief       ��ʼ�����Ʋ�����������
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
 * @brief       λ��ʽPID���ٶȻ��Ż�
 * @param       sptr    ������
 * @param       pid     PID����
 * @param       NowPlace    ��ǰλ��
 * @param       Point   Ԥ��λ��
 **********************************************************************************/
float PID_Speed(Error *sptr,PID *pid, int16_t NowPlace, int16_t Point)
{
 
	int16_t iError;	// ��ǰ���
	float Realize;  // ʵ�����	
 
	iError = Point - NowPlace;	// ���㵱ǰ���
    // Error_Seq_SUM = Error_Seq_SUM - Error_Seq_Ring[Error_Seq_Index] + iError;
    // Error_Seq_Ring[Error_Seq_Index] = iError;
    // + pid->I * Error_Seq_SUM   //����I
    // ��������Զ��޷�
    sptr->Current_Error += pid->I * iError;	// ������
      sptr->Current_Error = sptr->Current_Error > pid->limit?pid->limit:sptr->Current_Error;//�����޷�
      sptr->Current_Error = sptr->Current_Error <-pid->limit?-pid->limit:sptr->Current_Error;
	Realize = pid->P * iError       //����P
            + sptr->Current_Error   //����I
			+ pid->D * (iError - sptr->Last_Error);  //΢��D
	sptr->Last_Error = iError;		  	// �����ϴ����
	return Realize;	// ����ʵ��ֵ
}

/**********************************************************************************
 * @name        PID_Position
 * @author      Z2Z GuGu
 * @brief       λ��ʽPID��λ�û��Ż�
 * @param       sptr    ������
 * @param       pid     PID����
 * @param       NowPlace    ��ǰλ��
 * @param       Point   Ԥ��λ��
 **********************************************************************************/
int16_t PID_Position(Error *sptr,PID *pid, int16_t NowPlace, int16_t Point)
{
 
	int16_t iError;	// ��ǰ���
	float Realize;  // ʵ�����	
 
	iError = Point - NowPlace;	// ���㵱ǰ���
    sptr->Current_Error += pid->I * iError;	// ������
	Realize = pid->P * iError       //����P
            + sptr->Current_Error   //����I
			+ pid->D * (iError - sptr->Last_Error);  //΢��D
	sptr->Last_Error = iError;		  	// �����ϴ����
	return (int16_t)Realize;	// ����ʵ��ֵ
}

/**********************************************************************************
 * @name        MY_Motor_Speed_Ctrl
 * @author      Z2Z GuGu
 * @brief       λ��ʽPI���ԣ�������ɣ�������
 * @param       INPUT_Speed     Ԥ��λ��
 * @param       NOW_Speed       ��ǰλ��
 **********************************************************************************/
float C_Vref = 0.0;
float PID_P = 2;
float PID_I = 0.03;
float PID_D = 0;
void MY_Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed)
{
    static int16_t ALL_Error;
    int16_t Speed_Error;
    float Realize;	//���ó���ʵ������

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
 * @brief       �ٶȻ����������
 * @param       INPUT_Speed     Ԥ��λ��
 * @param       NOW_Speed       ��ǰλ��
 **********************************************************************************/
void Motor_Speed_Ctrl(int16_t INPUT_Speed, int16_t NOW_Speed)
{
    float Realize;	//���ó���ʵ������

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
 * @brief       λ�û�������λ�����������������
 * @param       INPUT_Position     Ԥ��λ��
 * @param       NOW_Position       ��ǰλ��
 **********************************************************************************/
int16_t Motor_Increment_Position_Ctrl(int16_t INPUT_Position, int16_t NOW_Position)
{
    int16_t Realize;	//���ó���ʵ������

    // PID
    Realize = PID_Position(&Position_PID_Error, &Position_PID_Param, NOW_Position, INPUT_Position);

    // Realize /= 2;

    if(Realize > SPEED_IN_limit)     Realize = SPEED_IN_limit;
	if(Realize < -SPEED_IN_limit)    Realize = -SPEED_IN_limit;

    // OUTPUT
    return Realize;
}
