/**********************************************************************************
 * @name        AS5600.c
 * @version     R2.3
 * @author      Z2Z GuGu
 * @date        2023/5/23
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "AS5600.h"

volatile uint8_t AS5600_RAW_H = 0;
volatile uint8_t AS5600_RAW_L = 0;
volatile uint8_t AS5600_Data_Ready = 0;
volatile int16_t Last_Position = 0;
volatile int16_t D_Last_Position = 0;
volatile int16_t Correct_Position = 0;
volatile int32_t Absolute_Position = 0;
volatile int16_t Circle_value = 0;
volatile int16_t D_Last_Increment = 0;
volatile int16_t TEMP_Speed;
uint8_t First_read = 1;

volatile int8_t Position_Inc_Seq_Ring[Position_Inc_Seq_MAX];
volatile uint8_t Position_Inc_Seq_Index = 0;
volatile uint8_t Position_Inc_Seq_Ring_Full = 0;
volatile int16_t M_Last_Speed = 0;

volatile int16_t Speed_Filter_Ring[Speed_Filter_MAX];
volatile uint8_t Speed_Filter_Index = 0;
volatile uint8_t Speed_Filter_Ring_Full = 0;
volatile int16_t Speed_Filter_SUM = 0;
volatile int16_t M_Filter_Speed;

//uint8_t IIC_DmaRevBuff[AS5600_REV_LEN];

/**********************************************************************************
 * @name        IIC_DMA_REV_Init
 * @author      Z2Z GuGu
 * @brief       DMA初始化，已弃用
 * @param       pbuff   DMA到RAM的地址
 * @param       len     DMA到RAM的长度
 **********************************************************************************/
void IIC_DMA_REV_Init(uint8_t *pbuff, u16 len)
{
    DMA_InitTypeDef  DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2C1->IC_DATA_CMD;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pbuff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;       //AS5600 use Mediu Priority DMA
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);                //Channel 3 is the DMAChannel for IIC REV
}

/**********************************************************************************
 * @name        I2C1BusFreeGPIOMode
 * @author      Z2Z GuGu
 * @brief       IIC引脚释放
 **********************************************************************************/
void I2C1BusFreeGPIOMode(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(I2C1_SCL_BUSCLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SCL_PIN;                                //I2C uses PB6, PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;                            //Set GPIO spped
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                               //Keep the bus free which means SCK & SDA is high
    GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);                              //Initializes the GPIOB peripheral

    RCC_AHBPeriphClockCmd(I2C1_SDA_BUSCLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SDA_PIN;                                //I2C uses PB6, PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;                            //Set GPIO spped
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                               //Keep the bus free which means SCK & SDA is high
    GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);                              //Initializes the GPIOB peripheral
}

/**********************************************************************************
 * @name        I2C1ConfigGPIOMode
 * @author      Z2Z GuGu
 * @brief       IIC引脚初始化
 **********************************************************************************/
void I2C1ConfigGPIOMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(I2C1_SCL_BUSCLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SCL_PIN;                                //I2C uses PB6, PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;                            //Set GPIO spped
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                             //Keep the bus free which means SCK & SDA is high
    GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);                              //Initializes the GPIOB peripheral

    RCC_AHBPeriphClockCmd(I2C1_SDA_BUSCLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = I2C1_SDA_PIN;                                //I2C uses PB6, PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;                            //Set GPIO spped
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                             //Keep the bus free which means SCK & SDA is high
    GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);                              //Initializes the GPIOB peripheral

    GPIO_PinAFConfig(I2C1_SCL_PORT, I2C1_SCL_AFSOURCE, I2C1_SCL_AFMODE);
    GPIO_PinAFConfig(I2C1_SDA_PORT, I2C1_SDA_AFSOURCE, I2C1_SDA_AFMODE);
}

/**********************************************************************************
 * @name        IIC_Init
 * @author      Z2Z GuGu
 * @brief       IIC初始化
 **********************************************************************************/
void IIC_Init(void)
{
    I2C_InitTypeDef I2C_InitStructure;
    // extern uint16_t I2C_DMA_DIR;

    // I2C_DMA_DIR = 1;
    // I2C1->IC_RX_TL = 2;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);                            //Enable I2C reset state

    I2C1BusFreeGPIOMode();

    I2C_InitStructure.I2C_Mode = I2C_Mode_MASTER;                                   //Configure I2C as master mode
    I2C_InitStructure.I2C_OwnAddress = 0;
    I2C_InitStructure.I2C_Speed = I2C_Speed_FAST;       //I2C_Speed_STANDARD
    I2C_InitStructure.I2C_ClockSpeed = 1000000;         //100000

    I2C_Init(I2C1, &I2C_InitStructure);                                             //Initializes the I2Cx peripheral according to the specified
    
    I2C_Send7bitAddress(I2C1, (AS5600_Addr<<1), I2C_Direction_Transmitter);         //Set the device address&&I2C_Direction_Transmitter 
    I2C_Cmd(I2C1, ENABLE);                                                          //Enable I2C

    I2C1ConfigGPIOMode();
}

/**********************************************************************************
 * @name        IIC_interrupt_Init
 * @author      Z2Z GuGu
 * @brief       IIC中断初始化
 **********************************************************************************/
void IIC_interrupt_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
//  I2C_ITConfig(I2C1, I2C_IT_RX_FULL | I2C_IT_TX_EMPTY, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;                             // Config I2C1_IRQn NVIC config
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                             // PreemptionPriority : 0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // Enable IRQn
    NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************************
 * @name        Parameter_Init
 * @author      Z2Z GuGu
 * @brief       AS5600部分参数初始化
 **********************************************************************************/
void Parameter_Init(void)
{
    uint8_t i;
    for (i = 0; i < Position_Inc_Seq_MAX; i++)
    {
        Position_Inc_Seq_Ring[i] = 0;
    }
    for (i = 0; i < Speed_Filter_MAX; i++)
    {
        Speed_Filter_Ring[i] = 0;
    }
    Position_Inc_Seq_Ring_Full = 0;
    Speed_Filter_Ring_Full = 0;
    First_read = 1;
}

/**********************************************************************************
 * @name        AS5600_Init
 * @author      Z2Z GuGu
 * @brief       AS5600初始化
 **********************************************************************************/
uint8_t AS5600_Init(void)
{
    uint8_t TEMP;
    IIC_Init();
    //AS5600config
    I2C_SendData(I2C1, AS5600_CFG1_REG);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);

    I2C_SendData(I2C1, 0x00);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    I2C_GenerateSTOP(I2C1, ENABLE);

    I2C_SendData(I2C1, AS5600_CFG2_REG);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    I2C_SendData(I2C1, 0x04);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    I2C_GenerateSTOP(I2C1, ENABLE);

    //Check sensor status
    I2C_SendData(I2C1, AS5600_STATE_REG);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);
    I2C_ReadCmd(I2C1);
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_RFNE) == 0);
    TEMP = I2C_ReceiveData(I2C1);
//  UartSendGroup((u8*)printBuf, sprintf(printBuf, "AS5600_STATE_REG : %d\r\n", TEMP));
    TEMP &= 0x38;

    // Read Angel Data
    Parameter_Init();

    switch (TEMP)
    {
    case 0x18:
        return 0;       //Magnet was detected
    case 0x28:
        return 1;       //Magnet too weak
    case 0x30:
        return 2;       //Magnet too strong
        
    default:
        return 3;
    }
}

/**********************************************************************************
 * @name        AS5600_read_POLL
 * @author      Z2Z GuGu
 * @brief       AS5600读取速度测试，已弃用
 **********************************************************************************/
uint16_t AS5600_read_POLL(void)     //Rev1.0 = 812us, Rev1.1 = 516us, Rev2.0 = 90us, Rev2.1 = 87us, Rev2.2 = 15.3us Rev2.3 = 24us
{
        uint16_t TEMP;
    //1st
//          LED1_ON();
    I2C1->IC_DATA_CMD = CMD_WRITE | AS5600_raw_ang_hi;      //-600ns = 666ns-458ns
//  I2C_SendData(I2C1, AS5600_raw_ang_hi);
//          LED1_OFF();
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE) == 0);       //1735ns
    //2st
//          LED1_ON();
    I2C1->IC_DATA_CMD = CMD_READ;               //669ns-485ns
//  I2C_ReadCmd(I2C1);
//          LED1_OFF();
    while(I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_RFNE) == 0);
    //3st
//          LED1_ON();
    TEMP = (uint8_t)I2C1->IC_DATA_CMD;          //666ns
//  TEMP = I2C_ReceiveData(I2C1);
//          LED1_OFF();
    I2C1->IC_DATA_CMD = CMD_READ;               //669ns-485ns
//  I2C_ReadCmd(I2C1);
    //4st
//          LED1_ON();
    TEMP <<= 8;
    TEMP |= (uint8_t)I2C1->IC_DATA_CMD;         //666ns
//  TEMP |= I2C_ReceiveData(I2C1);
//          LED1_OFF();
//  I2C1->IC_ENABLE |= 0x02;
    I2C_GenerateSTOP(I2C1, ENABLE);             //19us
    //5st
//          LED1_ON();
    while((I2C_GetITStatus(I2C1, I2C_IT_STOP_DET)) == 0);     //1349ns                   //Checks whether stop condition has occurred or not.
//          LED1_OFF();
    return TEMP;
}

/**********************************************************************************
 * @name        I2C1_IRQHandler
 * @author      Z2Z GuGu
 * @brief       IIC中断，写数据/读数据+数据处理（24us）
 **********************************************************************************/
void I2C1_IRQHandler(void)
{
//    static uint8_t data_filter_en = 0;
//    static uint8_t Zero_crossing = 0;
    uint8_t TEMP_Reg;
    if(I2C_GetITStatus(I2C1, I2C_IT_TX_EMPTY) != RESET) 
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_TX_EMPTY);
        I2C_ITConfig(I2C1, I2C_IT_TX_EMPTY, DISABLE);
        I2C1->IC_DATA_CMD = CMD_READ;
        I2C1->IC_RX_TL = 3;
        I2C_ITConfig(I2C1, I2C_IT_RX_FULL, ENABLE);
    }
    if(I2C_GetITStatus(I2C1, I2C_IT_RX_FULL) != RESET){
        I2C_ClearITPendingBit(I2C1, I2C_IT_RX_FULL);
        
        AS5600_RAW_H = (uint8_t)I2C1->IC_DATA_CMD;
        AS5600_RAW_L = (uint8_t)I2C1->IC_DATA_CMD;
        I2C_ITConfig(I2C1, I2C_IT_RX_FULL, DISABLE);
        // I2C_GenerateSTOP(I2C1, ENABLE);
        I2C1->IC_ENABLE |= 0x02;
        Last_Position = (AS5600_RAW_L | (AS5600_RAW_H << 8)) & 0x0FFF;
#ifdef Angle_correction
        Correct_Position = Last_Position + Angle_correction;
        if(Correct_Position >= Maximum_Angle)
        {
            Correct_Position -= Maximum_Angle;
        }
        Last_Position = Correct_Position;
#endif
        // figer speed parma
        if(!First_read)
        {
            // Zero crossing & Speed
            TEMP_Speed = Last_Position - D_Last_Position;
            if(TEMP_Speed >= Positive_Rotation_threshold)
            {
                if(TEMP_Speed >= Maximum_Angle)
                {
                    Last_Position = D_Last_Position;
                }
                else
                {
                    D_Last_Increment = TEMP_Speed - Maximum_Angle;
                    Circle_value--;
                }
            }
            else if(TEMP_Speed <= Negative_Rotation_threshold)
            {
                if(TEMP_Speed <= N_Maximum_Angle)
                {
                    Last_Position = D_Last_Position;
                }
                else
                {
                    D_Last_Increment = TEMP_Speed + Maximum_Angle;
                    Circle_value++;
                }
            }
            else
            {
                D_Last_Increment = TEMP_Speed;
            }
            Absolute_Position = Last_Position + (Circle_value << 12);
            M_Last_Speed = M_Last_Speed - Position_Inc_Seq_Ring[Position_Inc_Seq_Index] + D_Last_Increment;
            Position_Inc_Seq_Ring[Position_Inc_Seq_Index] = D_Last_Increment;
            Position_Inc_Seq_Index++;
            if(Position_Inc_Seq_Index >= Position_Inc_Seq_MAX)
            {
                Position_Inc_Seq_Index = 0;
                Position_Inc_Seq_Ring_Full = 1;
            }
            // Filter
            M_Filter_Speed = M_Last_Speed;
            if(Position_Inc_Seq_Ring_Full)
            {
                Speed_Filter_SUM = Speed_Filter_SUM - Speed_Filter_Ring[Speed_Filter_Index] + M_Filter_Speed;
                Speed_Filter_Ring[Speed_Filter_Index] = M_Filter_Speed;
                M_Filter_Speed = Speed_Filter_SUM / Speed_Filter_MAX;
                Speed_Filter_Index++;
                if(Speed_Filter_Index >= Speed_Filter_MAX)
                {
                    Speed_Filter_Index = 0;
                    Speed_Filter_Ring_Full = 1;
                }
            }
            AS5600_Data_Ready = 1;
        }
        First_read = 0;
        D_Last_Position = Last_Position;
        while (I2C1->IC_ENABLE & 0x02)      // I2C_GenerateSTOP(I2C1, ENABLE);
        ;
        TEMP_Reg = I2C1->IC_CLR_TX_ABRT;
        // LED1_OFF();
    }
}

/**********************************************************************************
 * @name        Read_AS5600_Start
 * @author      Z2Z GuGu
 * @brief       开始读取AS5600，在中断中取数据，不再这里while了
 **********************************************************************************/
void Read_AS5600_Start(void)
{
    // LED1_ON();
    // if(First_read)
    // {
    //     I2C1->IC_DATA_CMD = CMD_WRITE | AS5600_raw_ang_hi;
    //     I2C_ITConfig(I2C1, I2C_IT_TX_EMPTY, ENABLE);
    //     First_read = 0;
    // }
    // else
    // {
    //     // 10.4us
    //     I2C1->IC_DATA_CMD = CMD_READ;
    //     I2C_ITConfig(I2C1, I2C_IT_RX_FULL, ENABLE);
    // }
    I2C1->IC_DATA_CMD = CMD_WRITE | AS5600_raw_ang_hi;
    I2C_ITConfig(I2C1, I2C_IT_TX_EMPTY, ENABLE);

    // First_read = 0;
    // LED1_OFF();
}

