 /*
 * File:   MAIN.c
 * Author: zhangxiaomou
 *
 * Created on December 6, 2016, 9:56 AM
 */
// DSPIC30F4011 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FPR = XT_PLL4            // Primary Oscillator Mode (XT w/PLL 4x)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_ON    // Clock Switching and Monitor (Sw Enabled, Mon Enabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_64        // WDT Prescaler A (1:64)
#pragma config WDT = WDT_ON             // Watchdog Timer (Enabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV42          // Brown Out Voltage (4.2V)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#include "SysParam.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "FreeRTOS/Source/include/FreeRTOS.h"
#include "FreeRTOS/Source/include/task.h"
#include "FreeRTOS/Source/include/queue.h"
#include "FreeRTOS/Source/include/croutine.h"

#include "FreeRTOS_Task/FreeRTOS_TaskConfig.h"

#include "Drive/DriveADC.h"
#include "Drive/DriveIO.h"
#include "Drive/DrivePWM.h"
#include "Drive/DriveSerialPort.h"
#include "Drive/DriveTemp.h" 

#include "App/RtuFrame.h"
#include "App/Initialization.h"
#include "App/ErrorProcess.h"

/***********************************************************************************
 * 创建日期：2016.12.06
 * 版本：V1.0
 * 作者：ZDom
 * RUN_LED           内部绿灯  
 * IN_Y_LED           内部黄灯
 * IN_R_LED           内部红灯  
 * OUT_G_LED P7       外部绿灯  
 * OUT_Y_LED P10      外部黄灯  
 * OUT_R_LED P11      外部红灯  
 * HEATING_DISPALY         加热指示继电器K2
 * ERROR_RELAY5         故障指示继电器K4
 * LN_HEART       防冷凝加热器
 * IN_HEART       内部加热继电器
 * OUT_HEART_A    外部烘干继电器
 * OUT_HEART_B    外部烘干继电器
 * 冷凝加热器更改为定时器中断控制
 * 2017/08/03: 问题系统时钟运行不正确。500ms的延时在后期的运行过程中变化为超过1s的延时。
 **********************************************************************************/

/*-----------------------------------------------------------*/

QueueHandle_t g_HumValueMsg = NULL;     /*湿度值消息队列句柄*/
static HumFlag _PERSISTENT g_FlagHum;          /*用于保存烘干判断的各种命令，不能看门狗复位的变量*/
static uint16_t _PERSISTENT g_DryingTimeCount; /*烘干时间计数，不能看门狗复位的变量*/

int main(void) 
{
    g_ErrorState = ERROR_NO;    /*初始化错误标识符*/
    /* 设备初始化 -------------------------------------------------------------*/
    Init_IO();                  /*初始化IO口*/
    Init_ADC();                 /*初始化ADC*/
    Init_PWM();                 /*初始化PWM*/
    Init_SerialPort();          /*初始化USART*/
    
    Init_ReciveFrameData();     /*通信帧初始化*/
    Init_DS18B20();             /*初始化DS18B20*/
    
    /*温度传感器初始化检测*/
    Init_Temp(TEMP_0_INTERNAL);
    Init_Temp(TEMP_1_EXTERNAL);
#ifdef USE_TEMP_2_EXTERNAL
    Init_Temp(TEMP_2_EXTERNAL);
#endif
    
    ResetDetection(&g_FlagHum, &g_DryingTimeCount);  /*初始化自检测*/
    /*自检流程结束*/
    
    ErrorProcess(g_ErrorState); /*错误处理*/
    
    /*测试发送*/
    RX_TX_MODE = TX_MODE;
    __delay_us(2);
    Usart2Send(0xAA);
    Usart2Send(0x55);
    RX_TX_MODE = RX_MODE;
    
    /*LED*/
    Set_Led(&g_LedTypdef[INTERNAL_GREEN], LED_MODE_FLICKER, 100);    /*设置内部绿色指示灯*/
    Set_Led(&g_LedTypdef[EXTERNAL_GREEN], LED_MODE_ON, 0);           /*设置外部绿色指示灯*/
    
    /* 任务初始化 -------------------------------------------------------------*/
    /*创建消息队列*/
    g_HumValueMsg = xQueueCreate(HUM_VALUE_MSG_NUM, (sizeof(struct QueueMessage) + 1));
    if(g_HumValueMsg == NULL)   /*未能创建队列，不能使用，复位*/
    {
        Reset();
    }
    FEED_DOG();                 /*喂狗*/
    /**
     * @brief 创建主函数
     */
	xTaskCreate((TaskFunction_t )TaskMain,              /*任务函数*/
                (const char*    )"Main",                /*任务名称*/
                (uint16_t       )TASK_MAIN_SIZE,        /*任务堆栈大小*/
                (uint16_t*      )&g_DryingTimeCount,    /*传递给任务的参数*/
                (UBaseType_t    )TASK_MAIN_PRIO,        /*任务优先级*/
                NULL );   /*任务句柄*/
    /**
     * @brief 创建获取传感器参数的任务
     */
#if 1
	xTaskCreate((TaskFunction_t )TaskGetParam,              /*任务函数*/
                (const char*    )"Get",                     /*任务名称*/
                (uint16_t       )TASK_GET_PARAM_SIZE,       /*任务堆栈大小*/
                (void*          )&g_FlagHum,                /*传递给任务的参数*/
                (UBaseType_t    )TASK_GET_PARAM_PRIO,       /*任务优先级*/
                NULL );   /*任务句柄*/    
#endif

    /* 开始任务调度 -----------------------------------------------------------*/
	vTaskStartScheduler();
	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}

/*-----------------------------------------------------------*/

/**
 * @fn vApplicationStackOverflowHook
 * @brief 内存溢出检查
 * @param pxTask
 * @param pcTaskName
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; )
    {
        Reset();    /*复位*/
    }
}


/**
 * @fn Delay_ms
 * @brief 毫秒延时，不进行任务切换
 * @note 该延时不会进行任务切换
 * @param ms 0-65535
 * 
 */
inline void Delay_ms(uint16_t ms)
{
    while(ms--)
    {
        __delay_ms(1);
        FEED_DOG();
    }
}



