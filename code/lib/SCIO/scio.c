/**********************************************************************************
 * @name        scio.c
 * @brief       DMA优化后的UART库，接收和发送控制指令与数据
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/5/13
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "scio.h"

uint8_t rxdata[RX_LEN];
uint8_t txdata[TX_LEN];

/**********************************************************************************
 * @name        SCIO_Init
 * @author      Z2Z GuGu
 * @brief       SCIO初始化
 **********************************************************************************/
void SCIO_Init(void)
{
	UART2_DMA_TX_Init();
    UART2_DMA_RX_Init();
    SCIO_ConfigInit(UART_Speed);
	UART_DMACmd(UART2, UART_DMAReq_EN, ENABLE);

	UART2_DMA_Readbuf_Start(rxdata, RX_LEN);
}

/**********************************************************************************
 * @name        SCIO_ConfigInit
 * @author      Z2Z GuGu
 * @brief       UART初始化
 * @param       bound       UART波特率
 **********************************************************************************/
void SCIO_ConfigInit(uint32_t bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_3); //RX
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_4); //TX

    UART_InitStructure.UART_BaudRate = bound;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(UART, &UART_InitStructure);

    //UART_ITConfig( UART,  UART_IT_RXIEN, ENABLE);

    UART_Cmd(UART, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**********************************************************************************
 * @name        Serial_Bps_Setting
 * @author      Z2Z GuGu
 * @brief       UART 波特率设置
 * @param       _bps       UART波特率
 **********************************************************************************/
void Serial_Bps_Setting(uint32_t _bps)
{
    UART_InitTypeDef UART_InitStructure;
    UART_InitStructure.UART_BaudRate = _bps;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    
    UART_Init(UART, &UART_InitStructure);
}

/**********************************************************************************
 * @name        TX_Mode_Setting
 * @author      Z2Z GuGu
 * @brief       UART引脚 使/失能
 * @param       OUT_EN       1: 使能； 0: 失能
 **********************************************************************************/
void TX_Mode_Setting(uint8_t OUT_EN)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(OUT_EN)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

/**********************************************************************************
 * @name        UART2_DMA_TX_Init
 * @author      Z2Z GuGu
 * @brief       UART DMA TX初始化， txdata直接写进去了，反正以后也不能再用MM32了，复用是不可能的
 **********************************************************************************/
void UART2_DMA_TX_Init(void)
{
    DMA_InitTypeDef  DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART2->TDR); //UART2_DMA_TX数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txdata; //UART2_DMA_TX的数据存放基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//数据从memory到外设
    DMA_InitStructure.DMA_BufferSize = 1;//UART2_DMA_TX数据个数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);//SPI2 RX为DMA通道4
}

/**********************************************************************************
 * @name        UART2_DMA_RX_Init
 * @author      Z2Z GuGu
 * @brief       UART DMA RX初始化， rxdata直接写进去了，反正以后也不能再用MM32了，复用是不可能的
 **********************************************************************************/
void UART2_DMA_RX_Init(void)
{
    DMA_InitTypeDef  DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART2->RDR); //UART2_DMA_RX数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxdata;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//数据从外设到memory
    DMA_InitStructure.DMA_BufferSize = RX_LEN;//UART2_DMA_RX数据个数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不改变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//memory地址递增开启 DMA_MemoryInc_Enable
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);//
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @author MM
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  length : Transmit lengths.
/// @retval : None
////////////////////////////////////////////////////////////////////////////////
void DMA_Set_TransmitLen(DMA_Channel_TypeDef* channel, u16 length)
{
    channel->CNDTR = length;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @author MM
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  address : DMA memery address.
/// @retval : None
////////////////////////////////////////////////////////////////////////////////
void DMA_Set_MemoryAddress(DMA_Channel_TypeDef* channel, u32 address)
{
    channel->CMAR = address;
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  Set the DMA Channeln's Peripheral address.
/// @author MM
/// @param  channel : where n can be 1 to 7 for DMA1 to select the DMA Channel.
/// @param  length : Transmit lengths.
/// @retval : None
////////////////////////////////////////////////////////////////////////////////
void DMA_Set_MemoryInc_Enable(DMA_Channel_TypeDef* channel, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE) {
        channel->CCR |= DMA_MemoryInc_Enable;
    } else {
        channel->CCR  &= ~(DMA_MemoryInc_Enable);
    }
}
////////////////////////////////////////////////////////////////////////////////
/// @brief   DMA transmits packet
/// @author  MM
/// @param   ch: Pointer to a DMA channel.
/// @param   addr: The memory Buffer address.
/// @param   len: The length of data to be transferred.
/// @retval : None.
////////////////////////////////////////////////////////////////////////////////
void DRV_DMA_TransmitPacket(DMA_Channel_TypeDef* ch, u32 addr, u32 len)
{
    DMA_ITConfig(ch, DMA_IT_TC, ENABLE);
    DMA_Cmd(ch, DISABLE);
    DMA_Set_MemoryAddress(ch, addr);
    DMA_Set_TransmitLen(ch, len);
    DMA_Cmd(ch, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief   DMA transmit and receive packet
/// @author  MM
/// @param   ptx_buf: Pointer to SPI DMA send buffer(include send and recv bytes).
/// @param   prx_buf: Pointer to SPI DMA recv buffer(include send and recv bytes).
/// @param   len: The length of data , length equal send + recv len).
/// @retval : None.
////////////////////////////////////////////////////////////////////////////////********************************************************************************************************/
void UART2_WriteBuf(u8 *ptx_buf,  u32 len)
{
    DMA_Set_MemoryInc_Enable(DMA1_Channel4, ENABLE);
    DRV_DMA_TransmitPacket(DMA1_Channel4, (u32)ptx_buf, len);

//    UART_DMACmd(UART2,UART_DMAReq_EN,ENABLE);

    while(!DMA_GetFlagStatus(DMA1_FLAG_TC4));
    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
}
////////////////////////////////////////////////////////////////////////////////
/// @brief   DMA transmit and receive packet
/// @author  MM
/// @param   ptx_buf: Pointer to SPI DMA send buffer(include send and recv bytes).
/// @param   prx_buf: Pointer to SPI DMA recv buffer(include send and recv bytes).
/// @param   len: The length of data , length equal send + recv len).
/// @retval : None.
////////////////////////////////////////////////////////////////////////////////********************************************************************************************************/
void UART2_Readbuf( u8 *prx_buf, u32 len)
{
    DMA_Set_MemoryInc_Enable(DMA1_Channel5, ENABLE);
    DRV_DMA_TransmitPacket(DMA1_Channel5, (u32)prx_buf, len);

    while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
    DMA_ClearFlag(DMA1_FLAG_TC5);

    DMA_Cmd(DMA1_Channel5, DISABLE);
}

void UART2_DMA_Readbuf_Start( u8 *prx_buf, u32 len)
{
	DMA_Set_MemoryInc_Enable(DMA1_Channel5, ENABLE);
    DRV_DMA_TransmitPacket(DMA1_Channel5, (u32)prx_buf, len);
}

uint8_t UART2_updata_DMA_Rbuf()
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC5))
	{
		DMA_ClearFlag(DMA1_FLAG_TC5);
		DMA_Cmd(DMA1_Channel5, DISABLE);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
#define UARTTIMEOUT 0xf
#define UARTSUCESSFUL 0x1

////////////////////////////////////////////////////////////////////////////////
/// @brief   DMA transmit and receive packet
/// @param   ptx_buf: Pointer to SPI DMA send buffer(include send and recv bytes).
/// @param   prx_buf: Pointer to SPI DMA recv buffer(include send and recv bytes).
/// @param   len: The length of data , length equal send + recv len).
/// @retval : None.
////////////////////////////////////////////////////////////////////////////////
int UART2ReadbufTimeOut( u8 *prx_buf, u32 len, u32 Timeout)
{
    int result;
    u32 i = 0;
    DMA_Set_MemoryInc_Enable(DMA1_Channel5, ENABLE);
    DRV_DMA_TransmitPacket(DMA1_Channel5, (u32)prx_buf, len);

    while(1) {
        if(!DMA_GetFlagStatus(DMA1_FLAG_TC5) ) {
            delay(1);
            i++;
            if(i > Timeout) {
                result = UARTTIMEOUT;
                break;
            }
        } else {
            result = UARTSUCESSFUL;
            break;
        }
    }
    DMA_ClearFlag(DMA1_FLAG_TC5);

    DMA_Cmd(DMA1_Channel5, DISABLE);
    return result;
}

uint8_t UART2ReadBurst(uint8_t* pBuf, uint16_t len)
{
    if(len == 0)
        return 0;
    UART2_Readbuf(pBuf, len);
    return len;

}

void UART2WriteBurst(uint8_t* pBuf, uint16_t len)
{
    if(len == 0)
        return ;
    UART2_WriteBuf(pBuf, len);
    return ;
}
*/

/*
void UartSendByte(u8 dat)
{
    UART2_WriteBuf(&dat, (u32)1);
}

void UartSendGroup(u8* buf, u16 len)
{
    UART2_WriteBuf(buf, (u32)len);
}

void UartSendAscii(char *str)
{
    while(*str)
        UartSendByte(*str++);
}
*/


