/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "../SysParam.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			 1                              //1使用抢占式内核，0使用协程
#define configUSE_IDLE_HOOK				 1                              //1，使用空闲钩子；0，不使用
#define configUSE_TICK_HOOK				 0                              //1，使用时间片钩子；0，不使用
#define configTICK_RATE_HZ				 ( ( TickType_t ) 1000 )        //时钟节拍频率，这里设置为1000，周期就是1ms
#define configCPU_CLOCK_HZ				 ( ( unsigned long ) 4000000 )  //CPU频率 /* Fosc / 2 */
#define configMAX_PRIORITIES			 ( 3 )                          //可使用的最大优先级(可使用的有2-6)
#define configMINIMAL_STACK_SIZE		 ( 128 )                        //空闲任务使用的堆栈大小
#define configTOTAL_HEAP_SIZE			 ( ( size_t ) 1280 )            //系统所有总的堆大小
#define configMAX_TASK_NAME_LEN			 ( 5 )                          //任务名字字符串长度
#define configUSE_TRACE_FACILITY		 0                              //为1启用可视化跟踪调试
#define configUSE_16_BIT_TICKS			 1                              //系统节拍计数器变量数据类型，
#define configIDLE_SHOULD_YIELD			 1                              //为1时空闲任务放弃CPU使用权给其他同优先级的用户任务
#define configCHECK_FOR_STACK_OVERFLOW   2                              //大于0时启用堆栈溢出检测功能，如果使用此功能用户必须提供一个栈溢出钩子函数，如果使用的话
                                                                        //此值可以为1或者2，因为有两种栈溢出检测方法

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		    0                               //为1时启用协程，启用协程以后必须添加文件croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )                           //协程的有效优先级数目

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1       /*接收消息时，指定时间portMAX_DELAY则为无限期等待*/
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/**
 * tskIDLE_PRIORITY    0  中断优先级比空闲任务高一级即可
 */
#define configKERNEL_INTERRUPT_PRIORITY	0x01

/**
 * 此处需要定义，在port.c文件中
 */
#define MPLAB_DSPIC_PORT                1

#endif /* FREERTOS_CONFIG_H */

