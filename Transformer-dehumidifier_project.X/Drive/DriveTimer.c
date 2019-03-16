/**
 * @file DriveTimer.c
 * @brief 定时器配置模块
 * @date 2018.08.05
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "DriveTimer.h"


const uint16_t SystemTickTime = 1000;     /*系统心跳时钟1s*/
static uint16_t SytemTickCount = 0;

/* 外部函数实现----------------------------------------------------------------*/

/**
 * @fn Init_Timer3
 * @brief 初始化定时器3
 * @param ms
 */
void Init_Timer3(void)
{
    T1CON = 0;
    IPC1bits.T3IP = PRIO_TIMER; /*中断优先级*/
    T3CONbits.TON = 0;
    T3CONbits.TCKPS = 0b11;     /*1:256*/
    T3CONbits.TCS = 0;
    T3CONbits.TGATE = 0;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;          /*中断使能*/
    TMR3 = 0;
    PR3 = (uint16_t)((float32_t)FCY/1000.00/256.0*(float32_t)SystemTickTime)-1;
    SytemTickCount = PR3;
    T3CONbits.TON = 0;
    
} 

/**
 * @fn StartTimer3
 * @brief 开始计时
 */
inline void StartTimer3(void)
{
    IFS0bits.T3IF = 0;
    TMR3 = 0;
    PR3 =  SytemTickCount;
    T3CONbits.TON = 1;
}

/**
 * @fn StopTimer3
 * @brief 停止计时
 */
inline void StopTimer3(void)
{
    T3CONbits.TON = 0;
    IFS0bits.T3IF = 0;
}

/* 中断函数 -------------------------------------------------------------------*/

/* End -----------------------------------------------------------------------*/

