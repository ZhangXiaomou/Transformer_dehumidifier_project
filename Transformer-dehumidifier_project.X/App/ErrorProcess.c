/**
 * @file ErrorProcess.c
 * @brief 错误报警处理
 * @date 2017.07.03
 * @note 故障指示继电器说明： 只有在湿度传感器、温度传感器、以及模拟量输出不正确时动作
 *        故障指示灯说明： 
 *        1.外部温度传感器
 *          针对外部温度传感器，由于其对于整个系统的运行起着至关重要的作用，所以其报错
 *        采取的是持续报错，只能手动复归。现象为：内部红色指示灯常量，外部红色指示灯以
 *        1Hz的频
 *        率闪烁
 *        2.外部湿度传感器
 *          针对外部湿度传感器，由于其对于整个系统的运行起着至关重要的作用，所以其报错
 *        采取的是持续报错，只能手动复归。现象为：内部红色指示灯常量，外部红色指示灯常
 *        量
 *        3.模拟量输出超标报错
 *          外部绿色指示灯以2Hz的频率闪烁，内部红色指示灯不亮，内部黄色指示灯与外部绿
 *        色指示灯同频率闪烁。
 *        4.模拟量输出过低报错
 *          外部绿色指示灯以2Hz的频率闪烁，内部红色指示灯不亮，内部黄色指示灯以1Hz的
 *        频率闪烁。
 *        5.内部黄色指示灯常亮为盒内加热器启动加热。
 *        6.外部黄色指示灯常亮为烘干加热器启动加热。
 *        7.正常运行时，外部绿色指示灯为常量，内部绿色指示灯1Hz频率闪烁。
 * 
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "../Drive/DriveTemp.h"
#include "../Drive/DriveIO.h"

#include "ErrorProcess.h"

/*全局变量定义 ----------------------------------------------------------------*/
SysTemErrorState g_ErrorState = ERROR_NO; /*错误状态*/

/*私有变量定义 ----------------------------------------------------------------*/

/*内部函数定义 ----------------------------------------------------------------*/

/*内部函数实现 ----------------------------------------------------------------*/


/*内部函数实现 ----------------------------------------------------------------*/
/**
 * @fn ErrorProcess
 * @brief 错误处理函数，根据g_ErrorState的值进行相应的错误处理
 * 
 */
void ErrorProcess(SysTemErrorState state)
{
    static uint16_t i = 0;
    uint8_t errorFlag = 0;
    for(i = 0;i < 7; i++)
    {
        switch(state & (1 << i))
        {
            case ERROR_EXTERNAL_T1:     /*外部1号温度传感器出错*/
            {
                Set_Led(&g_LedTypdef[INTERNAL_RED], LED_MODE_ON, 0);            /*常亮*/
                Set_Led(&g_LedTypdef[EXTERNAL_RED], LED_MODE_FLICKER, 100);     /*1Hz闪烁*/
                Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_ON);    /*错误指示继电器开启*/
                errorFlag = i;
                break;
            }
            case ERROR_INTERNAL_T1:     /*盒内温度传感器出错*/
            {
                break;
            }
            case ERROR_HIH_1:           /*1号湿度传感器出错*/
            {
                Set_Led(&g_LedTypdef[INTERNAL_RED], LED_MODE_ON, 0);            /*常亮*/
                Set_Led(&g_LedTypdef[EXTERNAL_RED], LED_MODE_ON, 0);            /*常亮*/
                Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_ON);    /*错误指示继电器开启*/
                errorFlag = i;
                break;
            }
            case ERROR_MONITOR_H:       /*模拟量输出大于20mA*/
            {
                Set_Led(&g_LedTypdef[EXTERNAL_GREEN], LED_MODE_FLICKER, 50);    /*2Hz闪烁*/
                Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_FLICKER, 50);   /*2Hz闪烁*/
                Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_ON);    /*错误指示继电器开启*/
                errorFlag = i;
                break;
            }
            case ERROR_MONITOR_L:       /*模拟量输出小于4mA*/
            {
                Set_Led(&g_LedTypdef[EXTERNAL_GREEN], LED_MODE_FLICKER, 50);    /*2Hz闪烁*/
                Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_FLICKER, 100);  /*1Hz闪烁*/
                Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_ON);    /*错误指示继电器开启*/
                errorFlag = i;
                break;
            }
#ifdef USE_TEMP_2_EXTERNAL
            case ERROR_EXTERNAL_T2:     /*外部第二号温度传感器出错*/
            {
                errorFlag = i;
                break;
            }
            case ERROR_HIH_2:           /*2号湿度传感器出错*/
            {
                errorFlag = i;
                break;
            }
#endif
            case ERROR_NO:              /*无错误*/
            default:
            {
                if(i < 5)
                {
                    if(errorFlag == 0)  /*未出现错误再进行关闭指示灯操作*/
                    {
                        Set_Led(&g_LedTypdef[INTERNAL_RED], LED_MODE_OFF, 0);   /*熄灭*/
                        Set_Led(&g_LedTypdef[EXTERNAL_RED], LED_MODE_OFF, 0);   /*熄灭*/
                    }
                }
                else /*if((i >= 5) && (i < 7))*/
                {
                    if(errorFlag == 0)  /*未出现错误再进行关闭指示灯操作*/
                    {
                        Set_Led(&g_LedTypdef[EXTERNAL_GREEN], LED_MODE_ON, 0);      /*常亮*/
                        Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_OFF, 0);    /*灭*/
                    }
                }
                break;
            }
        }
    }
    /*任何错误都没有才会关闭继电器*/
    if(ERROR_NO == state)   
    {
        Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_OFF);   /*错误指示继电器关闭*/
    }
}


/*End ------------------------------------------------------------------------*/

