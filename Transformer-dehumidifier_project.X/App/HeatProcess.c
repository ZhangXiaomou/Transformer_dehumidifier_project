/*
 * File:   HeatProcess.c
 * Author: zhangxiaomou
 *
 * Created on December 6, 2016, 7:03 PM
 */


#include "Header.h"
#include "Delay.h"

#define STOP_HEATTIME   5400000 //1.5 hour
#define IDLE_ORDER      0x00    //空指令

const uint8_t hunidityMoreFifty = 0xFF;
const uint8_t orderRedayHeating = 0xAA;

HeatingConfig g_HeatingConfig;
uint8_t _PERSISTENT g_Order;  //用于存储执行的命令
uint32_t _PERSISTENT g_OrderRunTime;  //用于存储命令执行的时间
uint32_t HratTimeCount = 0; //加热时间计数

float g_Temp1Value = 0;
static float HumValue = 0;

void InitConfig(void)
{
    g_HeatingConfig.currentState = IDLE_ORDER;
    g_HeatingConfig.lastOrder = IDLE_ORDER;
    g_HeatingConfig.outOrder = IDLE_ORDER;
    g_HeatingConfig.inOrder = IDLE_ORDER;
    g_HeatingConfig.systemTime = 0;
    g_HeatingConfig.heatingTime = STOP_HEATTIME;
    
    if(g_Order == ORDER_START_HEATING)
    {
        g_Order = 0;
        
        g_HeatingConfig.currentState = hunidityMoreFifty;
        g_SystemStatus.statrHeatingTemp = TempAverage(TERM1_CH);     //获取开始加热时的温度值
        g_HeatingConfig.systemTime = g_TimeStampCollect.msTicks;
        g_HeatingConfig.heatingTime = STOP_HEATTIME - g_OrderRunTime;
        g_HeatingConfig.outOrder = ORDER_START_HEATING;  //开始加热
    }
    
}
/********************************************************
 * 函数名：     ReadBreathHum()
 * 形参：       无
 * 返回值：     无
 * 功能：       判断呼气函数
 ********************************************************/
void ReadBreathHum(void)
{
    ClrWdt();
    HumValue = GetAverageRH();
    ClrWdt();
    if(HumValue > FIFTY_PERCENT)        //判断湿度值是否大于50%
    {
        ClrWdt();
        g_HeatingConfig.currentState = hunidityMoreFifty;   //当前状态湿度大于50%了
    }
    /*湿度值大于50%的情况下判断湿度值是否小于40%*/
    if((g_HeatingConfig.currentState == hunidityMoreFifty) && (HumValue <= FORTY_PERCENT ) && (g_HeatingConfig.outOrder == IDLE_ORDER))
    {
        ClrWdt();
        g_HeatingConfig.outOrder = orderRedayHeating;  //准备开始加热
    }
    ClrWdt();
    if((g_HeatingConfig.outOrder == orderRedayHeating) && (g_HeatingConfig.currentState == hunidityMoreFifty))
    {
        g_SystemStatus.statrHeatingTemp = TempAverage(TERM1_CH);     //获取开始加热时的温度值
        g_HeatingConfig.systemTime = g_TimeStampCollect.msTicks;
        g_HeatingConfig.heatingTime = STOP_HEATTIME;
        g_HeatingConfig.outOrder = ORDER_START_HEATING;  //开始加热
        g_Order = ORDER_START_HEATING;
    }
    ClrWdt();
}

/********************************************************
 * 函数名：     ReadBreathHum()
 * 形参：       无
 * 返回值：     无
 * 功能：       保护函数
 * 建立时间：   2016.12.07
 ********************************************************/
void HeatProtection(void)
{
    if(g_HeatingConfig.outOrder)
    {
        if(HumValue > EIGHTY_PERCENT)        //判断湿度值是否大于80%
        {
            ClrWdt();
            g_HeatingConfig.outOrder = FALSE;          //clear
            g_HeatingConfig.currentState = FALSE;   //clear
        }
    }
}

/********************************************************
 * 函数名：     OUTHeatProcess()
 * 形参：       无
 * 返回值：     无
 * 功能：       烘干启动与停止函数
 ********************************************************/
void OUTHeatProcess(void)
{
    ClrWdt();
    if((g_HeatingConfig.outOrder == ORDER_START_HEATING) && (g_HeatingConfig.currentState == hunidityMoreFifty))
    {
        HeatedOut(ON);
        if(IsOverTime(g_HeatingConfig.systemTime, g_HeatingConfig.heatingTime))   //如果时间到则关闭加热
        {
            HeatedOut(OFF);
            g_HeatingConfig.lastOrder = g_HeatingConfig.outOrder;
            g_HeatingConfig.outOrder = FALSE;          //clear
            g_HeatingConfig.currentState = FALSE;   //clear
            g_Order = 0;
            g_OrderRunTime = 0;
        }
    }
    else
    {
        HeatedOut(OFF); //刷新关闭加热继电器
        g_Order = 0;
        g_OrderRunTime = 0;
    }
    if(g_HeatingConfig.lastOrder)
    {
        g_SystemStatus.heatingCount++;
        HeatDetection();    //烘干加热自检测
        g_HeatingConfig.lastOrder = FALSE;
    }
}

/********************************************************
 * 函数名：     INHeatProcess()
 * 形参：       无
 * 返回值：     无
 * 功能：       判断盒内是否需要加热
 ********************************************************/
void INHeatProcess(void)
{
    float temp0 = TempAverage(TERM0_CH);
    if(temp0 < MINUSTEN)         //温度低于-10℃后开启
    {
        ClrWdt();
        g_HeatingConfig.inOrder = ORDER_START_HEATING;
        HeatedIn(ON);
    }
    else if(temp0 > TENDEGREES) //温度高于10℃后停止
    {
        g_HeatingConfig.inOrder = IDLE_ORDER;
        HeatedIn(OFF);
        HratTimeCount = 0;
    }
    if(g_HeatingConfig.inOrder == ORDER_START_HEATING)
    {
        HratTimeCount++;
        ClrWdt();
        /*如果温度低于-25摄氏度则证明盒内温度加热器坏掉*/
        if((temp0 < ALARMTEMP) && (HratTimeCount >= 50000))        //温度低于-30℃且加热了一段时间后报警,
        {
            InHeatErrorProcess();
        }
        
    }
    else
    {
        if(g_ErrorState == ERROR_IN_HEAT)  //内部加热错误
        {
            g_ErrorState = 0;   //Clear
        }
    }
}

/********************************************************
 * 函数名：     LNHeatProcess()
 * 形参：       无
 * 返回值：     无
 * 功能：       烘干启动与停止函数,防冷凝加热器启动加热后内部黄灯闪烁
 ********************************************************/
void LNHeatProcess(void)
{
   if(g_Temp1Value <= MINUSTEN)
   {
       ClrWdt();
       IN_Y_LED = ON;   //内部黄灯
       LN_HEART = ON;
   }
   else if(g_Temp1Value > TENDEGREES)
   {
       if(g_HeatingConfig.inOrder)
       {
           return;
       }
       ClrWdt();
       IN_Y_LED = OFF;
       LN_HEART = OFF;
   }
}
