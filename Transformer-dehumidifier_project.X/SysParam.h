/**
 * @file SysParam.h
 * @brief 系统头文件，包含系统所需的库以及功能函数等
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 * Created on June 30, 2018, 5:10 PM
 */
 
#ifndef _SYSPARAM_H
#define	_SYSPARAM_H

/* 系统时钟宏定义，较为特殊-----------------------------------------------------*/
#define FCY 4e6

/* Includes ------------------------------------------------------------------*/
/**
 * @brief Sys And CPU Includes
 */

#include <libpic30.h>
#if 0
#include <p30fxxxx.h>
#endif

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "tydef.h"

/* 宏定义---------------------------------------------------------------------*/
#define Reset() {__asm__ volatile ("RESET");}
#define FEED_DOG()  ClrWdt()
extern volatile unsigned int  ClrWdt __attribute__((__sfr__,__deprecated__,__unsafe__));

typedef struct tagFarmValue
{
    float humValue;         /*湿度值*/
    float temp_InValue;     /*内部温度值*/
    float temp_ExValue;     /*外部湿度值*/
    float montorValue;      /*模拟量输出值*/
    float powerValue;       /*电源电压值*/
}FarmValue;

enum PRIO
{
    PRIO_GET_PARAM = 1,
    PRIO_MAIN = 2,
    PRIO_USART = 3,
    
    PRIO_OVER = 5
};
/* 外部函数定义----------------------------------------------------------------*/

void Delay_ms(uint16_t ms);

/* End -----------------------------------------------------------------------*/

#endif	/* SYSPARAM_H */

