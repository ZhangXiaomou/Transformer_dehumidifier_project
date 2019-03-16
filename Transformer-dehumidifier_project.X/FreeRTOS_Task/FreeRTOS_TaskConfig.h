/* 
 * File:   FreeRTOS_TaskConfig.h
 * Author: zhangxiaomou
 * brief:  作为FreeRTOS任务的配置库函数
 *
 * Created on June 30, 2018, 1:40 PM
 */

#ifndef _FREERTOS_TASKCONFIG_H
#define	_FREERTOS_TASKCONFIG_H

/* Includes ------------------------------------------------------------------*/

/**
 * @brief Sys And CPU Includes
 */
#include "../SysParam.h" 

/**
 * @brief FreeRTOS Includes
 */
#include "../FreeRTOS/Source/include/FreeRTOS.h"
#include "../FreeRTOS/Source/include/task.h"
#include "../FreeRTOS/Source/include/queue.h"
#include "../FreeRTOS/Source/include/event_groups.h"

#include "../FreeRTOS/FreeRTOSConfig.h"

/*宏定义 ----------------------------------------------------------------------*/
/*数据结构 --------------------------------------------------------------------*/

/*任务参数定义 ----------------------------------------------------------------*/

/**
 * @brief 主任务 
 */
#define TASK_MAIN_PRIO		PRIO_MAIN   /*任务优先级*/
#define TASK_MAIN_SIZE 		224         /*任务堆栈大小*/
void TaskMain(void const * argument);  /*任务函数*/

/**
 * @brief 获取传感器值的任务 
 */
#define TASK_GET_PARAM_PRIO		PRIO_GET_PARAM  /*任务优先级*/
#define TASK_GET_PARAM_SIZE     128             /*任务堆栈大小*/
void TaskGetParam(void const * argument);     /*任务函数*/

#define HUM_VALUE_MSG_NUM       2       /*湿度值消息队列数量*/

/*全局变量定义 ----------------------------------------------------------------*/

extern QueueHandle_t g_HumValueMsg;             /*湿度值消息队列句柄*/

#define SET_FLAG(flag, value)       {(flag |= value);}      /*设置标志位*/
#define GET_FLAG(flag, value)       (flag & value)          /*获取标志位*/
#define CLEAR_FLAG(flag, value)     {(flag &= ~value);}     /*清除标志位*/
#define REST_FLAG(flag, value)      {(flag &= value);}      /*复位标志位*/

/**
 * 创建一个消息队列的结构体，内部包含传输的数据类型等
 */
struct QueueMessage
{
    uint16_t error_count;   /*失败计数*/
    uint16_t *count;        /*用于时间计数,该计数值不会被复位*/
    float *value;           /*传递的参数值*/
};
/* End ------------------------------------------------------------------*/     

#endif	/* FREERTOS_TASKCONFIG_H */

