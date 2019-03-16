/**
 * @file ReadADC.c
 * @brief 获取ADC码值
 * @date 2017.07.03
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "../Drive/DriveADC.h"
#include "ReadADC.h"

#define HUM_VOLATGE_MAX     3.198f
#define HUM_VOLATGE_MIN     0.826f

/* 外部函数实现----------------------------------------------------------------*/
/**
 * @fn  Get_HumOne()
 * @brief 获取湿度传感器——1的测量值
 * @param void
 * @return float
 */
inline float Get_HumOne(void)
{
    return ADCBUF1 * ADCRATIO;
}

/**
 * @fn Get_HumTow()
 * @brief 获取湿度传感器——2的测量值
 * @param void
 * @return float
 */
inline float Get_HumTow(void)
{
    return ADCBUF2 * ADCRATIO;
}

/**
 * @fn Get_Power()
 * @brief 获取电源电压的测量值
 * @param void
 * @return float
 */
inline float Get_Power(void)
{
    return ADCBUF3 * ADCRATIO;
}

/**
* @fn Get_MONITOR()
* @brief 获取4-20mA反馈电压的测量值
* @param void
* @return float
*/
inline float Get_MONITOR(void)
{
    return ADCBUF0 * ADCRATIO;
}

/**
 * @fn GetTrueRH()
 * @brief 获取湿度真实值
 * @note 湿度计算Voltage output (1 st g_Order curve fit) VOUT=(VSUPPLY)(0.0062(sensor RH) + 0.16), typical at 25 ºC
 *        温度补偿Temperature compensation True RH = (Sensor RH)/(1.0546 – 0.00216T), T in ºC
 *        sensor RH = 32.25806 * Vout - 25.80645
 *        sensorRH = (vout - 0.826) / 0.0315;
 * @param vout 采样电压
 * @param temperateValue 环境温度 摄氏度
 * @return 真实的湿度值
 * 
 */
float Get_TrueRH(float vout, float temperateValue)
{
    float sensorRH = 0;
    float trueRH_G = 0;
    if((vout >= HUM_VOLATGE_MIN) && (vout <= HUM_VOLATGE_MAX))
    {
        sensorRH = (vout - HUM_VOLATGE_MIN) / 0.0315;
    }
    else if(vout > HUM_VOLATGE_MAX)
    {
        return 99.90f;
    }
    else if(vout < HUM_VOLATGE_MIN)
    {
        return 0.0f;
    }
    trueRH_G = (sensorRH)/(1.0546 - 0.00216 * temperateValue);
    return trueRH_G;
}

/* End -----------------------------------------------------------------------*/
