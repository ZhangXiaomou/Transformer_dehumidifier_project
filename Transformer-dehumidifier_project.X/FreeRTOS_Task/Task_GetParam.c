/**
 * @file Task_Main.c
 * @brief 主任务，完成通信功能，状态指示灯功能，信号量采集等功能
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 * Created on June 30, 2018, 5:10 PM
 */
 
/*include Sys*/
#include "FreeRTOS_TaskConfig.h"

/*include FreeRTOS*/
#include "../FreeRTOS/FreeRTOSConfig.h"
#include "../FreeRTOS/Source/include/FreeRTOS.h"
#include "../FreeRTOS/Source/include/task.h"

/*include Drive*/
#include "../Drive/DriveIO.h"
#include "../Drive/DriveTimer.h"

/*include App*/
#include "../App/Initialization.h"


const static float HUM_VALUE_40 = 40.0f;
const static float HUM_VALUE_50 = 50.0f;
const static float HUM_VALUE_60 = 60.0f;
const static uint16_t HUM_HEAT_OVER_TIME = 5400;     /*1.5h = 90 * 60 = 5400（秒）*/

static uint8_t g_FlagStartTime = false;

#define START_TIME()                (g_FlagStartTime = true)
#define STOP_TIME()                 (g_FlagStartTime = false)
#define GET_START_TIME_FLAG()       (g_FlagStartTime == true)

/**
 * @fn DryingProcess
 * @brief 烘干处理
 * @param humFlag 指向HumFlag的指针
 * 
 */
static void DryingProcess(HumFlag* humFlag, uint16_t *timeCount)
{
    HumFlag flag = *humFlag;    /*获取标志位*/
    if(flag == HUM_FLAG_CLEAR)
    {
        return ;
    }
    if(GET_FLAG(flag, (HUM_FLAG_HEAT_START | HUM_FLAG_ACHIEVE_40 | HUM_FLAG_ACHIEVE_50)) \
        == (HUM_FLAG_HEAT_START | HUM_FLAG_ACHIEVE_40 | HUM_FLAG_ACHIEVE_50))
    {
        Set_Relay(&g_RelayTypdef[RELAY_HEAT_STATE], RELAY_MODE_ON);     /*烘干加热指示继电器开启*/
        Set_Relay(&g_RelayTypdef[RELAY_EX_HEAT], RELAY_MODE_ON);        /*烘干加热器开启*/
        Set_Led(&g_LedTypdef[EXTERNAL_YELLOW], LED_MODE_ON, 0);         /*外部黄色指示灯常亮*/
        /*开始计时*/
        START_TIME();
    }
    else if(GET_FLAG(flag, (HUM_FLAG_HEAT_STOP | HUM_FLAG_ACHIEVE_40 | HUM_FLAG_ACHIEVE_50)) \
            == (HUM_FLAG_HEAT_STOP | HUM_FLAG_ACHIEVE_40 | HUM_FLAG_ACHIEVE_50) )
    {
        Set_Relay(&g_RelayTypdef[RELAY_HEAT_STATE], RELAY_MODE_OFF);    /*烘干加热指示继电器关闭*/
        Set_Relay(&g_RelayTypdef[RELAY_EX_HEAT], RELAY_MODE_OFF);       /*烘干加热器关闭*/
        Set_Led(&g_LedTypdef[EXTERNAL_YELLOW], LED_MODE_OFF, 0);        /*外部黄色指示灯熄灭*/
        /*停止计时*/
        STOP_TIME();
    }
    
    /*达到停止时间*/
    if((*timeCount > HUM_HEAT_OVER_TIME) && (GET_FLAG(flag, HUM_FLAG_HEAT_START)) )
    {
        Set_Relay(&g_RelayTypdef[RELAY_HEAT_STATE], RELAY_MODE_OFF);    /*烘干加热指示继电器关闭*/
        Set_Relay(&g_RelayTypdef[RELAY_EX_HEAT], RELAY_MODE_OFF);       /*烘干加热器关闭*/
        Set_Led(&g_LedTypdef[EXTERNAL_YELLOW], LED_MODE_OFF, 0);        /*外部黄色指示灯熄灭*/
        REST_FLAG(*humFlag, HUM_FLAG_CLEAR); /*标志位复位*/
        /*加热停止*/
        STOP_TIME();
        *timeCount = 0;    /*初始化*/
    }
}
/**
 * @fn Task_GetParam
 * @brief 获取传感器信息等
 * @param argument
 * 
 */
void TaskGetParam(void const * argument)
{
    /*用于烘干使用*/
    static float humValue = 0.0f;
    HumFlag flagHum = *(HumFlag*)argument;      /*用于保存烘干判断的各种命令*/
    BaseType_t err = 0;
    static struct QueueMessage receiveQueueMessage;
    for(;;)
    {
		/*等待事件到达*/
        err = xQueueReceive(g_HumValueMsg, &receiveQueueMessage, portMAX_DELAY);
        if(err == pdPASS)
        {
            humValue = *receiveQueueMessage.value;
            /*烘干启动条件判断*/
            if(humValue > HUM_VALUE_50)
            {
                SET_FLAG(flagHum, HUM_FLAG_ACHIEVE_50);
            }
            else if((GET_FLAG(flagHum, HUM_FLAG_ACHIEVE_50)) && (humValue < HUM_VALUE_40))
            {
                SET_FLAG(flagHum, HUM_FLAG_HEAT_START | HUM_FLAG_ACHIEVE_40);
            }
            /*保护条件判断*/
            if((humValue > HUM_VALUE_40) && (GET_FLAG(flagHum, HUM_FLAG_HEAT_START)))
            {
                /*满足开始加热 && 湿度大于50% && 湿度小于40%  && 湿度此时小于40% */
                SET_FLAG(flagHum, HUM_FLAG_HEAT_STOP);
                CLEAR_FLAG(flagHum, HUM_FLAG_HEAT_START);
            }
            else if(GET_FLAG(flagHum, (HUM_FLAG_HEAT_STOP | HUM_FLAG_ACHIEVE_50 | HUM_FLAG_ACHIEVE_40)) \
                    == (HUM_FLAG_HEAT_STOP | HUM_FLAG_ACHIEVE_50 | HUM_FLAG_ACHIEVE_40) && (humValue < HUM_VALUE_40))
            {
                /*满足停止加热 && 湿度大于50% && 湿度小于40%  && 湿度此时小于40% */
                CLEAR_FLAG(flagHum, HUM_FLAG_HEAT_STOP);
                SET_FLAG(flagHum, HUM_FLAG_HEAT_START);
            }
            if(g_FlagStartTime == true)
            {
                (*receiveQueueMessage.count) ++;
                (*receiveQueueMessage.count) += receiveQueueMessage.error_count;    /*增加信息丢失计数*/
            }
            DryingProcess(&flagHum, receiveQueueMessage.count);        /*烘干处理函数*/
        }
    }
}





