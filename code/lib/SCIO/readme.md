# SCIO Lib

## Introduction

+ 针对MicroFOC时间颗粒运行模式优化的UART库
+ 加一个DMA，不用老进中断了，直接在时间颗粒开头判断就行
+ 测试效果良好
+ 未知错误：运行FOC时UART TX时有点问题，会卡住，改用UART库的Send函数吧，这MM32是一天都不想碰了，谁知道是MM的锅还是我的锅，不想改了，搁while1里边儿while吧，算是充分利用闲余CPU时间哈哈哈哈哈
