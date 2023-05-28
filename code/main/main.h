/**********************************************************************************
 * @name        main.h
 * @version     R1.0
 * @author      Z2Z GuGu
 * @date        2022/2/12
 * @code        GB2312
 * @License     GPLv3
 **********************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#include "HAL_device.h"
#include "HAL_conf.h"
#include "stdio.h"

#include "parameter.h"

#include "Basic.h"
#include "SVPWM.h"
#include "AS5600.h"
#include "ADC.h"
#include "Ctrl.h"

#ifdef SCIO_lib
#include "scio.h"
#else
#include "UART.h"
#endif

#endif
