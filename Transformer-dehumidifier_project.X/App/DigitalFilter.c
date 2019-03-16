/**
 * @file DigitalFilter.c
 * @brief 数字滤波文件
 * @date 2017.07.03
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "../Drive/DriveTemp.h"
#include "../Drive/DriveADC.h"
#include "DigitalFilter.h"
#include "ReadADC.h"
#include "ErrorProcess.h"

/* 公有函数实现 ---------------------------------------------------------------*/

/**
 * @fn DigitalFilter()
 * @brief 简单的数字滤波函数
 * @param value 指向要计算平均值的数组的指针
 * @return 平均值
 */
float DigitalFilter(float *value)
{
    uint8_t index = 0;
    float min = 0;
    float max = 0;
    float sum = 0;
    float val = 0;
    sum += value[0];
    min = value[0];
    max = value[0];
    for(index = 0;index < 5;index++)
    {
        val = value[index + 1];
        max = (val > max) ? val : max;
        ClrWdt();
        min = (val < min) ? val : min;
        sum += val;
    }
    sum = (sum - max - min) / 4;
    return sum;
}

/**
 * @fn GetAverage_Temp()
 * @brief 温度值取平均
 * @brief ch 选择要获取的温度传感器
 * @retfloat GetAverage_Temp(uint8_t ch)urn 获取的温度值
 */
float GetAverage_Temp(uint8_t ch)
{
    ClrWdt();
    uint8_t index = 0;
    uint8_t ret = 0;
    float tempvalue = 0;
    float temp[6];
    for(index = 0; index < 6;index++)
    {   
        ret = DS18B20GetTemperature(ch, &temp[index], 0);    //一次读取温度
        if(ret)
        {
            tempvalue = TEMP_ERROR;
            break;
        }
    }
    if((index < 6) && (tempvalue == TEMP_ERROR))
    {
        switch(ch)
        {
        case TEMP_0_INTERNAL:
        {
            g_ErrorState |= ERROR_INTERNAL_T1;    /*内部温度传感器报警*/
            return 0.0f;
        }
        case TEMP_1_EXTERNAL:
        {
            g_ErrorState |= ERROR_EXTERNAL_T1;    /*外部温度传感器报警*/
            return 0.0f;
        }
    #ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {

        }
    #endif
        }  /*switch(ch)*/
    }
    else
    {
        switch(ch)
        {
        case TEMP_0_INTERNAL:
        {
            g_ErrorState &= ~ERROR_INTERNAL_T1;    /*消除报警*/
            break;
        }
        case TEMP_1_EXTERNAL:
        {
            g_ErrorState &= ~ERROR_EXTERNAL_T1;    /*消除报警*/
            break;
        }
    #ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {

        }
    #endif
        }  /*switch(ch)*/
    }
    
    tempvalue = DigitalFilter(temp);
    return tempvalue;
}

/**
 * @fn GetAverage_RH()
 * @brief 湿度值取平均
 * @return 获取到的湿度值
 */
void GetAverage_RH(FarmValue *value)
{
    uint8_t index = 0;
    float data = 0.0;
    float temp1Value[6];
    float hum1Value[6];
    float montor[6];
    float power[6];
    for(index = 0; index < 6;index++)
    {
        DS18B20GetTemperature(TEMP_1_EXTERNAL, &temp1Value[index], 0);    /*一次读取温度*/
        SoftSampleOnce();
        data = Get_HumOne();    
        montor[index] = Get_MONITOR();          /*获取4-20mA反馈值*/
        power[index] = Get_Power();
        hum1Value[index] = Get_TrueRH(data, temp1Value[index]);  
    }
    value->humValue = DigitalFilter(hum1Value);         /*获取湿度的平均值*/
    value->temp_ExValue = DigitalFilter(temp1Value);    /*获取外部温度的平均值*/
    value->montorValue = DigitalFilter(montor);         /*获取4-20mA反馈值的平均值*/
    value->powerValue = DigitalFilter(power);           /*获取电源电压值*/
}


/* End ------------------------------------------------------------------*/
