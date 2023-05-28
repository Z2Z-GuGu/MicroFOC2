/**********************************************************************************
 * @name        UART.c
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/2/12
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#include "UART.h"

char printBuf[30];
char *pTx = printBuf;
u8 rx_cnt = 0, tx_flag;

/*******************************************************************************
* @name   : UART_IRQHandler
* @brief  : UART interrupt function
* @param  : None
* @retval : void
*******************************************************************************/
void UART2_IRQHandler(void)
{
    if(UART_GetITStatus(UART, UART_IT_RXIEN) == SET) {                          //check receive status

        UART_ClearITPendingBit(UART, UART_IT_RXIEN);
        printBuf[rx_cnt % 100] = UART_ReceiveData(UART);

        if( printBuf[rx_cnt % 100] == '}') {                                   //check '\n',and set send flag;
            tx_flag = 1;
            pTx = printBuf;
        }
		tx_flag = 1;
        rx_cnt++;
    }
    if(UART->IER & UART_IT_TXIEN) {                                             //judge send empty interrupt enable
        if((UART_GetITStatus(UART, UART_IT_TXIEN) == SET)) {
            UartSendByte(*pTx++);
            UART_ClearITPendingBit(UART, UART_IT_TXIEN);
            rx_cnt--;
            if(rx_cnt == 0) {
                UART_ITConfig( UART,  UART_IT_TXIEN, DISABLE);
                tx_flag = 0;
            }
        }
    }
}

/*******************************************************************************
* @name   : Uart_ConfigInit
* @brief  : Uart Config Init
* @param  : u32 bound
* @retval : void
*******************************************************************************/
void Uart_ConfigInit(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_Init(& NVIC_InitStruct);

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

    UART_ITConfig( UART,  UART_IT_RXIEN, ENABLE);

    UART_Cmd(UART, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* @name   : UartSendByte
* @brief  : Uart Send Byte
* @param  : u8 dat
* @retval : void
*******************************************************************************/
void UartSendByte(u8 dat)
{
    UART_SendData( UART, dat);
    while(!UART_GetFlagStatus(UART, UART_FLAG_TXEPT));
}

/*******************************************************************************
* @name   : UartSendGroup
* @brief  : Uart Send Group
* @param  : u8* buf,u16 len
* @retval : void
*******************************************************************************/
void UartSendGroup(u8* buf, u16 len)
{
    while(len--)
        UartSendByte(*buf++);
}

/*******************************************************************************
* @name   : UartSendAscii
* @brief  : Uart Send Ascii
* @param  : char *str
* @retval : void
*******************************************************************************/
void UartSendAscii(char *str)
{
    while(*str)
        UartSendByte(*str++);
}
