/**********************************************************************************
 * @name        scio.h
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/5/13
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/

#ifndef _SCIO_H_
#define _SCIO_H_

#include "parameter.h"
#include "HAL_device.h"
#include "stdio.h"
#include "string.h"
#include "HAL_conf.h"

#define RX_LEN 8
#define TX_LEN 1

extern uint8_t rxdata[RX_LEN];
extern uint8_t txdata[TX_LEN];

void SCIO_Init(void);
void TX_Mode_Setting(uint8_t OUT_EN);
void Serial_Bps_Setting(uint32_t _bps);
void SCIO_ConfigInit(uint32_t bound);
void UART2_DMA_TX_Init(void);
void UART2_DMA_RX_Init(void);
void DMA_Set_TransmitLen(DMA_Channel_TypeDef *channel, u16 length);
void DMA_Set_MemoryAddress(DMA_Channel_TypeDef *channel, u32 address);
void DMA_Set_MemoryInc_Enable(DMA_Channel_TypeDef *channel, FunctionalState NewState);
void DRV_DMA_TransmitPacket(DMA_Channel_TypeDef *ch, u32 addr, u32 len);
void UART2_WriteBuf(u8 *ptx_buf, u32 len);
void UART2_Readbuf(u8 *prx_buf, u32 len);
void UART2_DMA_Readbuf_Start( u8 *prx_buf, u32 len);
uint8_t UART2_updata_DMA_Rbuf(void);

#endif
