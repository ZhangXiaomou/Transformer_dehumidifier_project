/**
 * @file Initialization.c
 * @brief 系统初始化，,主要实现30s的自检测功能以及包含系统初始化时使用的函数等
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 * Created on June 30, 2018, 5:10 PM
 */
 
/* Includes ------------------------------------------------------------------*/
#include "Initialization.h"
#include "DigitalFilter.h"
#include "ErrorProcess.h"
#include "ReadADC.h"

#include "../Drive/DriveADC.h"
#include "../Drive/DriveIO.h"
#include "../Drive/DriveTemp.h"

/* 内部函数定义----------------------------------------------------------------*/ 
/**
 * @fn AutoDetection
 * @brief 系统第一次上电时进行的30s自检测
 */
static void AutoDetection();

/* 内部函数实现----------------------------------------------------------------*/ 
/**
 * @fn Init_Temp
 * @brief 初始化温度传感器，第一次上电使用
 * @param ch 温度传感器通道选择
 * 
 */
void Init_Temp(uint8_t ch)
{
    float temp = 0.0;
    uint8_t ret = 0;
    ret = DS18B20GetTemperature(ch, &temp, 800);
    if(ret)
    {
        switch(ch)
        {
            case TEMP_0_INTERNAL:
            {
                g_ErrorState |= ERROR_INTERNAL_T1;   //内部温度传感器错误
                break;
            }
            case TEMP_1_EXTERNAL:
            {
                g_ErrorState |= ERROR_EXTERNAL_T1;   //外部温度1传感器错误
                break;
            }
#ifdef USE_TEMP_2_EXTERNAL
            case TEMP_2_EXTERNAL:
            {
                g_ErrorState |= ERROR_EXTERNAL_T2;   //外部温度2传感器错误
                break;
            }
#endif
        }
    }
}


/**
 * @fn AutoDetection
 * @brief 系统第一次上电时进行的30s自检测
 * 
 */
static void AutoDetection(void)
{
    uint8_t cn = 0;
    float tempValue = 0.0;
    float adc1Value[6] = {0,0,0,0,0,0};
    float humValueData[6] = {0,0,0,0,0,0};
    float monitorValue = 0;    
    uint16_t humValue = 0;
    
    /*开启LED灯*/
    OUT_G_LED = ON;
    OUT_R_LED = ON;
    OUT_Y_LED = ON;
    RUN_LED = ON;
    IN_Y_LED = ON;
    IN_R_LED = ON;
    /*继电器循环启动*/
    HEATING_DISPALY = ON;
    Delay_ms(4000);      //4s
    HEATING_DISPALY = OFF;
    Delay_ms(2000);      //2s
    ERROR_RELAY5 = ON;
    Delay_ms(4000);      //4s
    ERROR_RELAY5 = OFF;
    Delay_ms(2000);      //2s
    IN_HEART = ON;
    Delay_ms(4000);      //4s
    IN_HEART= OFF;
    Delay_ms(2000);      //2s
    OUT_HEART_A = ON;
    Delay_ms(4000);      //4s
    OUT_HEART_A= OFF;
    Delay_ms(2000);      //2s
    OUT_HEART_B = ON;
    Delay_ms(4000);      //4s
    OUT_HEART_B = OFF;
    Delay_ms(2000);      //2s
    /*关闭指示灯*/
    OUT_G_LED = OFF;
    OUT_R_LED= OFF;
    OUT_Y_LED= OFF;
    RUN_LED = OFF;
    IN_R_LED = OFF;
    IN_Y_LED = OFF;

    /*湿度传感器自检*/
    for(cn = 0;cn < 6;cn++)
    {        
        ClrWdt();
        SoftSampleOnce();
        adc1Value[cn] = Get_MONITOR(); /*模拟量*/
        humValue = Get_HumOne();  /*湿度*/
        DS18B20GetTemperature(TEMP_1_EXTERNAL, &tempValue, 0);    /*一次读取温度*/
        humValueData[cn] = Get_TrueRH(humValue, tempValue);
    }
    monitorValue = DigitalFilter(adc1Value);    /*获取平均值*/
    humValue = DigitalFilter(humValueData);     /*获取平均值*/
    if(!humValue)
    {
        g_ErrorState |= ERROR_HIH_1;
    }
    /*模拟量自检测*/
    if(monitorValue < MinCurrent)
    {
        g_ErrorState |= ERROR_MONITOR_L;     //模拟量输出低于4mA
    }
    else if(monitorValue > MaxCurrent)
    {
        g_ErrorState |= ERROR_MONITOR_H;     //模拟量输出高于20mA
    }
}

/* 外部函数定义----------------------------------------------------------------*/ 
/**
 * @fn ResetDetection
 * @brief 检测看门狗复位，欠压复位，陷阱复位等
 */
void ResetDetection(HumFlag* flag, uint16_t* count)
{
    /*第一次上电后标志位清零*/
    if(RCONbits.POR)
    {
        RCON = 0;
    }
    /*判断是否发生上电延时复位、欠压复位以及看门狗超时复位*/
    if((!RCONbits.WDTO)&&(!RCONbits.BOR)&&(!RCONbits.TRAPR)&&(!RCONbits.SWR)&&(!RCONbits.POR))
    {
        *count = 0;             /*初始化*/
        *flag = HUM_FLAG_CLEAR;
        g_ErrorState = 0;       /*错误状态位初始化*/
        AutoDetection();        /*上电自检*/
    }
    else
    {
        RCONbits.WDTO = 0;
        RCONbits.BOR = 0;
        RCONbits.TRAPR = 0;
        RCONbits.SWR = 0;
    }
}


/* End -----------------------------------------------------------------------*/
