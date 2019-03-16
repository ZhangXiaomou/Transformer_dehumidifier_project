/**
 * @file GetValue.c
 * @brief 获取真实的湿度值
 * @date 2017.07.03
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

#ifndef READADC_H
#define	READADC_H

/* Includes ------------------------------------------------------------------*/
#include "../FreeRTOS_Task/FreeRTOS_TaskConfig.h"

/* 外部函数定义----------------------------------------------------------------*/
extern float Get_HumOne(void);
extern float Get_HumTow(void);
extern float Get_Power(void);
extern float Get_MONITOR(void);
extern float Get_TrueRH(float vout, float temperateValue);
/* End -----------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

