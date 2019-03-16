/**
 * @file Initialization.h
 * @brief 系统初始化库，包含系统初始化时使用的函数等
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 * Created on June 30, 2018, 5:10 PM
 */
 
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_INITIALIZATION_H
#define	XC_INITIALIZATION_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"

/* 宏定义---------------------------------------------------------------------*/
#define INITTEMP 0x0550
   
typedef enum enum_HumFlag
{
    HUM_FLAG_CLEAR = 0,                     /*清除标志位*/
    
    HUM_FLAG_ACHIEVE_50 = (1 << 1),         /*湿度达到了50%*/
    HUM_FLAG_ACHIEVE_40 = (1 << 2),         /*湿度达到了40%*/
    HUM_FLAG_ACHIEVE_60 = (1 << 3),         /*湿度达到了60%*/
    HUM_FLAG_HEAT_ACHIEVE = (1 << 4),       /*湿度达到了加热条件*/
    HUM_FLAG_HEAT_START = (1 << 5),         /*开始加热，也是正在加热的标志*/
    HUM_FLAG_HEAT_STOP = (1 << 6),          /*加热暂停*/
    HUM_FLAG_HEAT_OVER_TIME = (1 << 7),     /*加热停止*/
            
    HUM_FLAG_OVER = 0xFFFF                  /*结束，方便添加标志位*/
}HumFlag;


/* 外部函数定义----------------------------------------------------------------*/ 
void ResetDetection(HumFlag* flag, uint16_t* count);
void Init_Temp(uint8_t ch);

/* End -----------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

