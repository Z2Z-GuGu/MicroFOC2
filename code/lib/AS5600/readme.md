# AS5600

## Introduction

+ IIC读不出来，程序我觉得没问题
+ 逻辑分析仪测了一下发现MM32给从地址发了两次，推测IIC组件有地址自动发送的功能
+ 确实有这玩意儿，MM32的SDK写的确实不行，但人家硬件设计的不错，写个邮件让他们改一下吧
+ 实现AS5600磁场方向传感器的读取（812us）
+ 注：硬件版本2.0以后的设计中，AS5600倒置，读取方向与实际方向相反，注意一下
+ 测试发现MM32可以通过强行降低分频来提高SCL频率，不局限于400K，目前提高至1MHZ（AS5600最高支持频率）
+ 时间测试：516us
+ 上DMA试一下
+ 能读出数据，但数据不对，和AS5600发的不一样，算了吧，中断就中断，无所谓了，反正又不是什么大数
+ 修改读取策略，就不在while里等了，进中断吧
+ 时间测试：87us
+ 读就不进中断了，下一次时间颗粒正好读上一次的结果，美汁儿汁儿
+ 时间测试：15.3us，相当不错
+ 连读有问题，读不对，初步怀疑第二个byte实际上读成第一个了
+ 确实
+ 问题是我设置的FIFO大小是2呀，硬件BUG，到了时候不给NAK还继续发ACK
+ 还是改回去吧
+ 时间轴上出现周期性的数据跳变，怀疑电机磁极会干扰结果，滤吧，反正时间有的是
+ 加数据有效性监测，拿上一个值代替一下
+ 上一个值加上次增量模拟本次值
+ 加滑动滤波器吧
+ 测试发现8个比较合适
+ 倒是能滤掉，问题是过零时有跳变啊，改回来吧，不加速度
+ 跳变时有时无
+ 串口速度加快发现每次都有，应该不是加增量的问题
+ 绝了，根本不是电机磁场干扰，还是读IIC的问题，MM32寄存器说的每一句话都不能信，浪费我两天
+ 好了，不想记录怎么回事，重构了好几遍，反正改好了，很麻烦，MM32的芯片我TMD是一天都不想用了，以后项目再用MM32我是傻逼
+ 时间测试：24us，还行
+ 我发现IIC终止信号还能再省一点时间，给他拆了吧
+ 时间测试：13us，相当不错，加上数据处理一共23us，还可以
