/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/**
 * @file ErrorProcess.c
 * @brief 错误报警处理
 * @date 2017.07.03
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_ERRORPROCESS_H
#define	XC_ERRORPROCESS_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"

/* 宏定义----------------------------------------------------------------------*/
#define LowVoltage 2.0166016    //2.0166016V
#define MinCurrent 0.659        //0.659V
#define MaxCurrent 3.311        //3.311V
#define DryerErrorHum 50
#define HIGH 0xF1
#define LOW  0xF2
#define HIHERROR    0x0091

/* 枚举定义--------------------------------------------------------------------*/
/**********************************************************/
typedef enum 
{
    ERROR_NO        = 0,         //(0x0000)无错误
    ERROR_EXTERNAL_T1 = 1 << 0,  //(0x0001)外部温度传感器1出错
    ERROR_EXTERNAL_T2 = 1 << 1,  //(0x0002)外部温度传感器2出错
    ERROR_INTERNAL_T1 = 1 << 2,  //(0x0004)内部温度传感器出错
    ERROR_HIH_1       = 1 << 3,  //(0x0008)外部湿度传感器1出错
    ERROR_HIH_2       = 1 << 4,  //(0x0010)外部湿度传感器2出错
            
    ERROR_MONITOR_H = 1 << 5,    //(0x0020)模拟量输出高错误
    ERROR_MONITOR_L = 1 << 6,    //(0x0040)模拟量输低出错误
            
    ERROR_POWER     = 1 << 7,    //(0x0080)电源电压错误
    ERROR_DRYER     = 1 << 8,    //(0x0100)干燥剂不能干燥错误
    ERROR_IN_HEAT   = 1 << 9,    //(0x0200)内部加热错误
    
    SysError_Over = 0xFFFF
}SysTemErrorState;
/**********************************************************/


/* 全局变量定义----------------------------------------------------------------*/
extern SysTemErrorState g_ErrorState;  //错误状态

/* 外部函数定义----------------------------------------------------------------*/
void ErrorProcess(SysTemErrorState state);

/* End -----------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

