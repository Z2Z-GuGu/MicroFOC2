# MicroFOC2
 A low-cost cascaded FOC software and hardware

## 1、器件描述

- 主控：MM32F003
- 时钟频率：48MHZ
- MOS管：WSD3020DN
- 电流采集：INA180A4
- 磁编码器：AS5600（可配置）
- 位置编码器：[ PT9 + IR9 ] * 2（940nm）

## 2、性能描述

- 控制周期：10-20 us
- SVPWM可变频率：1K - 15KHZ
- 最高电压：30V
- 电流： 
  - 7A @30 °C
  - 12A @40 °C

## 3、特性描述

- 支持非磁编码器
- 可变控制频率
- 高频母线电流采样
- 支持Z2C@UART总线通讯协议
  - 支持平级通信，搜索总线挂载的FOC系统
  - 支持主从通信，用于设置和搜索
  - 支持电机序号自定义，同时支持设置上电记忆和清除功能
  - 支持单电机控制和电机集群控制
  - 支持电流数据收集
  - 支持位置信息反馈
- 物料成本低于20¥

## 4、引脚分配

+ 电源及复位
  1. [1] - nRST
  2. [4] - VSSA/VSS
  3. [5] - VCAP
  4. [6] - VDDA/VDD
+ GPIO
  5. [2] - PD0：SDA
  6. [3] - PD1：SCL
  7. [7] - PA0：INTA
  8. [8] - PA4：INTB
  9. [9] - PA5：CH3N
  10. [10] - PB0：CH3
  11. [11] - PB1：ADC
  12. [12] - PB13：CH1N
  13. [13] - PB14：CH2N
  14. [14] - PA13：SWDIO
  15. [15] - PA14：SWDCLK
  16. [16] - PB3：CH1
  17. [17] - PB4：CH2
  18. [18] - PB6：LED&KEY
  19. [19] - PB7：UART2_TX
  20. [20] - PA6：UART2_RX

