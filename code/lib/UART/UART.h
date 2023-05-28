/**********************************************************************************
 * @name        UART.h
 * @version     R1.1
 * @author      Z2Z GuGu
 * @date        2022/2/12
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#ifndef _UART_H_
#define _UART_H_

#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"

extern char printBuf[30];

void Uart_ConfigInit(u32 bound);
void UartSendByte(u8 dat);
void UartSendGroup(u8* buf, u16 len);
void UartSendAscii(char *str);

#endif

/*
    UartSendGroup((u8*)printBuf, sprintf(printBuf, "sprintf ok\r\n"));          //pintf stdio data
    UartSendAscii("UartSendAscii\r\n");                                         //printf string
    UartSendAscii("Please input data ,end with '}' \r\n");
*/
