/*
 * File:   AutoCheck.c
 * Author: zhangxiaomou
 *
 * Created on December 6, 2016, 8:11 PM
 */


#include "Header.h"

/********************************************************
 * 函数名：     AutoCheck()
 * 形参：       无
 * 返回值：     无
 * 功能：       定时自检函数
 ********************************************************/
void AutoCheck(void)
{    
    if(!IsOverTimeStamp( &g_TimeStampCollect.selftestTime))
    {
       return; 
    }
    uint8_t cn = 0;    
    float adc1value[6] = {0,0,0,0,0,0};
    float adc2value[6] = {0,0,0,0,0,0};
    float analogvalue = 0;    
    float powervalue = 0;
    ClrWdt();
    //数字滤波
    for(cn = 0;cn < 6;cn++)
    {
        ClrWdt();
        SoftSampleOnce();
        adc1value[cn] = GetMONITOR_U(); //模拟量
        adc2value[cn] = GetPower();     //电源电压
    }
    analogvalue = DigitalFilter(adc1value);
    powervalue = DigitalFilter(adc2value);
    ClrWdt();
    
    if(analogvalue > MaxCurrent) //电流大于20mA
    {
        MONITORErrorProcess(HIGH);
    }
    else if(analogvalue < MinCurrent)
    {
        MONITORErrorProcess(LOW);            
    }
    else
    {
        if(g_ErrorState == ERROR_MONITOR_H || g_ErrorState == ERROR_MONITOR_L)  //发生模拟量错误
        {
            g_ErrorState = 0;   //Clear
        }
    }
    
    //电源电压检测
    if(powervalue < LowVoltage)
    {
        UsysErrorProcess();
    }
    else
    {
        if(g_ErrorState == ERROR_POWER)  //产生了电源错误
        {
            g_ErrorState = 0;   //Clear
        }
    }
    ClrWdt();
    
    g_TimeStampCollect.selftestTime.startTime = g_TimeStampCollect.msTicks;
}

/********************************************************
 * 函数名：     HeatDetection()
 * 形参：       无
 * 返回值：     无
 * 功能：       烘干器自检函数，自带错误处理函数
 * 创建时间：   2016.12.08
 * 实现方法：   采用两个传感器的温度值对比实现，该程序采用的是温度传感器不做转换运算，
 * 均采用二进制数进行比较，所以需要根据最高四位的值进行比较从而得出结论
 ********************************************************/
void HeatDetection(void)
{
    g_SystemStatus.stopHeatingTemp = TempAverage(TERM1_CH);
    /*对比开始加热和停止加热之间的数值以及设计一个干燥剂最大烘干次数*/
    if((g_SystemStatus.statrHeatingTemp > g_SystemStatus.stopHeatingTemp) || (g_SystemStatus.heatingCount > MAXCN))    
    {
        ClrWdt();
        DryerErrorProcess();    //烘干器错误处理程序
    }
    else
    {
        if(g_ErrorState == ERROR_DRYER)  //干燥剂不能干燥错误
        {
            g_ErrorState = 0;   //Clear
        }
    }
}
