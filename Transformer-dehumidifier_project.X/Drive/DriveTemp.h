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
 * @file DriveTemp.c
 * @brief DS18B20温度传感器初始化，获取温度值等
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_INITTEMP_H
#define	XC_INITTEMP_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"
    
/*宏定义 ----------------------------------------------------------------------*/

#define TEMP_0_INTERNAL 0xA0   /*盒内温度传感器*/
#define TEMP_1_EXTERNAL 0xA1   /*配合湿度传感器A*/

#ifdef USE_TEMP_2_EXTERNAL
#define TEMP_2_EXTERNAL 0xA2   /*配合湿度传感器B*/
#endif
#define TEMP_ERROR  333.3f      /*特殊的错误号*/

/*函数定义 -------------------------------------------------------------------*/

extern uint8_t DS18B20GetTemperature( uint8_t ch, float* tempValue, uint16_t delay);
extern uint8_t Init_DS18B20();
/*End ------------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

