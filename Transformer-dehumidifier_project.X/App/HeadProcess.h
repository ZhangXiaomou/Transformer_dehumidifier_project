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

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef HEADPROCESS_H
#define	HEADPROCESS_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define MINUSTEN        (-10.0f)    //零下10摄氏度
#define TENDEGREES      10.0f       //零上10摄氏度
#define ALARMTEMP       (-30.0f)    //零下30摄氏度
#define ZERO            0.0f   //零度
#define STOPHEATTIME    5400    //1.5小时时间
#define ONESECOND       100     //1S

typedef struct TagHeatingConfig
{
	uint8_t  currentState;  //当前的状态
	uint8_t  outOrder;      //要执行的命令
	uint8_t  inOrder;       //盒内加热片执行的命令
    uint8_t  lastOrder;     //上一次执行的指令
	uint32_t heatingTime;	//加热时间
	uint32_t systemTime;    //当前的系统时间
}HeatingConfig;

#define ORDER_START_HEATING  0x55

void INHeatProcess(void);
void OUTHeatProcess(void);
void LNHeatProcess(void);
void ReadBreathHum(void);
void HeatProtection(void);
void InitConfig(void);

extern float g_Temp1Value;
extern HeatingConfig g_HeatingConfig;
extern uint8_t _PERSISTENT g_Order;  //用于存储执行的命令
extern uint32_t _PERSISTENT g_OrderRunTime;  //用于存储命令执行的时间

#endif	/* XC_HEADER_TEMPLATE_H */

