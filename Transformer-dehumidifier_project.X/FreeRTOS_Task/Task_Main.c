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
#include "../FreeRTOS/Source/include/croutine.h"

/*include Drive*/
#include "../Drive/DriveIO.h"
#include "../Drive/DriveSerialPort.h"
#include "../Drive/DriveTemp.h"
#include "../Drive/DriveADC.h"

/*include App*/
#include "../App/RtuFrame.h"
#include "../App/DigitalFilter.h"
#include "../App/ErrorProcess.h"
#include "../App/ReadADC.h"

/* The execution period of the check task. */
#define MAIN_CHECK_TASK_PERIOD				( ( TickType_t ) 10 / portTICK_PERIOD_MS )

#define OUT_PUT_4MA     0.20f       /*模拟量输出4mA*/
#define OUT_PUT_20MA    1.15f       /*模拟量输出20mA*/
#define MAX_OUT_PUT_V   3.45f       /*模拟量输出最大值，由检测电路测得的电压得到*/
#define MIN_OUT_PUT_V   0.68f       /*模拟量输出最小值，由检测电路测得的电压得到*/
/**
 * @fn CAL_PWM_OUT_PUT
 * @brief 根据湿度值计算PWM输出占空比
 * @param humValue 湿度值
 * @return 返回占空比比率值
 */
#define CAL_PWM_OUT_PUT(humValue)       ((0.0095f * humValue) + AnalogCal.minOutPut)
/**
 * @fn EN_PWM_OUT_PUT
 * @brief 根据参赛使能输出PWM
 * @param proportion 占空比比例
 */
#define EN_PWM_OUT_PUT(proportion)      {(PDC2 = proportion * PTPER);}

/*加热阈值*/
#define START_IN_HEAT_TEMP_VALUE  (-15.0f)  /*盒内开始加热阈值*/
#define STOP_IN_HEAT_TEMP_VALUE  (15.0f)    /*盒内停止加热阈值*/
#define WARNING_IN_HEAT_TEMP_VALUE  (START_IN_HEAT_TEMP_VALUE * 2)  /*报警温度*/
#define CALIBRATION_ALL_OUTPUT      0xA0
#define CALIBRATION_MAX_OUTPUT      0xA1
#define CALIBRATION_MIN_OUTPUT      0xA2
#define ALANOG_CALI_TIME            604800      /*一周进行一次检查*/

struct AnalogCalibration
{
    float minOutPut;
    float maxOutPut;
};

static FarmValue SendFarmValue;         /*发送数据结构体*/
static struct AnalogCalibration AnalogCal = {
    .minOutPut = OUT_PUT_4MA,
    .maxOutPut = OUT_PUT_20MA
};              /*模拟量输出校验*/
/*-----------------------------------------------------------*/

static void PWM_output(float montor, float hum, struct AnalogCalibration *analogCal);
static void Led_Scan(LedTypdef* led);
static void Relay_Scan(RelayTypdef* relay);
static void AnalogCalibration_fun(struct AnalogCalibration *analogCal, float monitorValue, uint8_t state);

/**
 * @fn PWM_output
 * @brief  PWM输出函数
 * @param montor 4-20mA模拟量输出反馈值
 * @param hum 湿度值
 */
inline static void PWM_output(float montor, float hum, struct AnalogCalibration *analogCal)
{
    static double proportion = 0.00;
    static int8_t errorCount_1 = 0;  /*错误计数*/
    static int8_t errorCount_2 = 0;  /*错误计数*/
    static int8_t errorCount_3 = 0;  /*错误计数*/
    int ahum = 0;
    ahum = (int)hum;
    
    /*湿度传感器不在出现问题*/
    if(ahum == 0)
    {
        errorCount_3 ++;
        if(errorCount_3 > 5)
        {
            SET_FLAG(g_ErrorState, ERROR_HIH_1);    /*报警*/
        }
    }
    else
    {
        errorCount_3 --;
        if(errorCount_3 <= 0)
        {
            errorCount_3 = 0;
            CLEAR_FLAG(g_ErrorState, ERROR_HIH_1);  /*消除错误*/
        }
    }
    
    /*输出PWM计算*/
    proportion = CAL_PWM_OUT_PUT(hum);    //计算输出比例
    if(proportion > analogCal->maxOutPut)
    {
        proportion = analogCal->maxOutPut;
    }
    else if(proportion < analogCal->minOutPut)
    {
        proportion = analogCal->minOutPut;
    }
    /*对模拟量输出进行错误检测，加入对端口电压值*/
    if(montor > MAX_OUT_PUT_V)
    {
        errorCount_1 ++;
        if(errorCount_1 > 5)
        {
            SET_FLAG(g_ErrorState, ERROR_MONITOR_H);    /*报警*/
        }
        proportion = analogCal->minOutPut;
    }
    else if(montor < MIN_OUT_PUT_V)
    {
        errorCount_2 ++;
        if(errorCount_2 > 5)
        {
            SET_FLAG(g_ErrorState, ERROR_MONITOR_L);    /*报警*/
        }
        proportion = analogCal->maxOutPut;
    }
    else
    {
        errorCount_1 --;
        errorCount_2 --;
        if(errorCount_1 <= 0)
        {
            errorCount_1 = 0;
            CLEAR_FLAG(g_ErrorState, ERROR_MONITOR_H);  /*消除错误*/
        }
        if(errorCount_2 <= 0)
        {
            errorCount_2 = 0;
            CLEAR_FLAG(g_ErrorState, ERROR_MONITOR_L);  /*消除错误*/
        }
    }
    EN_PWM_OUT_PUT(proportion);     /*输出PWM*/
}

/**
 * @fn AnalogCalibration_fun
 * @brief  4-20mA模拟量输出校准函数
 * @param analogCal 指向校准值
 */
static void AnalogCalibration_fun(struct AnalogCalibration *analogCal, float monitorValue, uint8_t state)
{
    static float adc1Value[6] = {0};
    int cn = 0;
    if((state == CALIBRATION_MIN_OUTPUT) || (state == CALIBRATION_ALL_OUTPUT))
    {
        while((monitorValue < (MIN_OUT_PUT_V - 0.005f)) || (monitorValue > (MIN_OUT_PUT_V + 0.005f)))
        {
            if(monitorValue > MIN_OUT_PUT_V)
            {
                if(analogCal->maxOutPut < (OUT_PUT_4MA - 0.19))
                {
                    return;
                }
                analogCal->minOutPut -= 0.001;
            }
            else
            {
                if(analogCal->minOutPut > 1.9)
                {
                    return;
                }
                analogCal->minOutPut += 0.001;
            }
            EN_PWM_OUT_PUT(analogCal->minOutPut);     /*输出PWM*/
            __delay_ms(10);
            for(cn = 0; cn < 6; cn++)
            {
                SoftSampleOnce();
                adc1Value[cn] = Get_MONITOR(); /*模拟量*/
            }
            monitorValue = DigitalFilter(adc1Value);    /*获取平均值*/
        }
    }
    if((state == CALIBRATION_MAX_OUTPUT) || (state == CALIBRATION_ALL_OUTPUT))
    {
        while((monitorValue < (MAX_OUT_PUT_V - 0.005f)) || (monitorValue > (MAX_OUT_PUT_V + 0.005f)))
        {
            if(monitorValue > MAX_OUT_PUT_V)
            {
                if(analogCal->maxOutPut < (OUT_PUT_20MA - 1))
                {
                    return;
                }
                analogCal->maxOutPut -= 0.001;
            }
            else
            {
                if(analogCal->maxOutPut > 1.9)
                {
                    return;
                }
                analogCal->maxOutPut += 0.001;
            }
            EN_PWM_OUT_PUT(analogCal->maxOutPut);     /*输出PWM*/
            __delay_ms(10);
            for(cn = 0; cn < 6; cn++)
            {
                SoftSampleOnce();
                adc1Value[cn] = Get_MONITOR(); /*模拟量*/
            }
            monitorValue = DigitalFilter(adc1Value);    /*获取平均值*/
        }
    }
    monitorValue = 10.02;
}

/**
 * @fn Led_Scan
 * @brief LED扫描函数，修改状态
 * 
 */
static void Led_Scan(LedTypdef* led)
{
    switch(led->mode)
    {
        case LED_MODE_ON:
        {
            led->status = LED_STATUS_ON;     //状态置为亮
            led->LedOn();   //灯亮
            break;
        }
        case LED_MODE_FLICKER:
        {
            if(led->status == LED_STATUS_OFF)
            {
                led->count++;
                if(led->count >= led->freq)
                {
                    led->count = 0;
                    led->status = LED_STATUS_ON; //状态置为亮
                    led->LedOn();   //灯亮
                }
            }
            else if(led->status == LED_STATUS_ON)
            {
                led->count++;
                if(led->count >= led->freq)
                {
                    led->count = 0;
                    led->status = LED_STATUS_OFF; //状态置为灭
                    led->LedOff();   //灯灭
                }
            }
            else
            {
                led->status = LED_STATUS_OFF;    //状态置为灭
            }
            break;
        }
        case LED_MODE_OFF:
        default:
        {
            led->status = LED_STATUS_OFF;     //状态置为灭
            led->LedOff();   //灯灭
            break;
        }
    }
}

/**
 * @fn Relay_Scan
 * @brief 
 * 
 */
static void Relay_Scan(RelayTypdef* relay)
{
    switch(relay->mode)
    {
        case RELAY_MODE_ON:
        {
            relay->RelayOn();   /*继电器吸合*/
            break;
        }
        case RELAY_MODE_OFF:
        default:
        {
            relay->RelayOff();  /*继电器断开*/
            break;
        }
    }
}

/**
 * @fn In_HeatProcess
 * @brief 盒内加热片处理
 * @param tempValue 盒内温度值
 */
static void In_HeatProcess(float tempValue)
{
    if(tempValue < START_IN_HEAT_TEMP_VALUE)
    {
        /*开启加热*/
        Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_ON);     /*盒内加热器开启*/
        Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_ON, 0);             /*内部黄色指示灯常亮*/
    }
    else if(tempValue > STOP_IN_HEAT_TEMP_VALUE)
    {
        /*停止加热*/
        Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_OFF);    /*盒内加热器关闭*/
        Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_OFF, 0);            /*内部黄色指示灯关闭*/
    }
    /*盒内温度低于启动加热温度两倍时启动报警*/
    if(tempValue < WARNING_IN_HEAT_TEMP_VALUE)
    {
        SET_FLAG(g_ErrorState, ERROR_IN_HEAT);    /*盒内加热器报警*/
    }
    else
    {
        CLEAR_FLAG(g_ErrorState, ERROR_IN_HEAT);    /*消除报警*/
    }
}

/**
 * @fn Ln_HeatProcess
 * @brief 防冷凝加热处理
 * @param tempValue 主体温度值
 */
static void Ln_HeatProcess(float tempValue)
{    
    if(tempValue < START_IN_HEAT_TEMP_VALUE)
    {
        /*开启加热*/
        Set_Relay(&g_RelayTypdef[RELAY_LN_HEAT], RELAY_MODE_ON);     /*防冷凝加热器开启*/
    }
    else if(tempValue > STOP_IN_HEAT_TEMP_VALUE)
    {
        /*停止加热*/
        Set_Relay(&g_RelayTypdef[RELAY_LN_HEAT], RELAY_MODE_OFF);    /*防冷凝加热器关闭*/
    }
}

/**
 * @fn vApplicationIdleHook
 * @brief 空闲任务钩子函数，内部处理通信
 * 
 */
void vApplicationIdleHook( void )
{
    /*处理通信*/
    if(SendFarmValue.powerValue != NULL)
    {
        RecivFrameDispose(&SendFarmValue);
    }
}

/**
 * @fn TaskMain
 * @brief 主任务，处理通信，处理消息队列，处理传感器值等主要功能
 * @param argument
 * 
 */
void TaskMain(void const *argument)
{
    static uint8_t index = 0;
    static uint8_t count = 1;
#if 1
    static uint32_t flagPowerOn = ALANOG_CALI_TIME;
#endif
    
    SendFarmValue.humValue = 0.0f;      /*湿度值*/
    SendFarmValue.montorValue = 0.0f;   /*模拟量输出值*/
    SendFarmValue.temp_ExValue = 0.0f;  /*外部温度值*/
    SendFarmValue.temp_InValue = 0.0f;  /*内部温度值*/
    SendFarmValue.powerValue = 0.0f;    /*电源电压值*/
    static struct QueueMessage sendQueueMessage;
    sendQueueMessage.count = (uint16_t*)argument;
    sendQueueMessage.value = &SendFarmValue.humValue;
    BaseType_t err = 0;
    /* 使用固定的处理频率来处理数据 */
    TickType_t xLastExecutionTime; 
	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();
    
    for(;;)
    {
        vTaskDelayUntil( &xLastExecutionTime, MAIN_CHECK_TASK_PERIOD);  /*10ms*/
        
        /*LED与继电器扫描*/
        for(index = 0; index < LED_OVER; index ++)
        {
            Led_Scan(&g_LedTypdef[index]);
        }
        for(index = 0; index < RELAY_OVER; index ++)
        {
            Relay_Scan(&g_RelayTypdef[index]);
        }
        FEED_DOG();    /*10ms 喂狗*/

        /*1s检测一次*/
        if(count++ >= 100)
        {
            count = 1;
            GetAverage_RH(&SendFarmValue);      /*获取湿度值与外部温度值*/
            if(SendFarmValue.humValue > 100.0f)
            {
                SendFarmValue.humValue = 0;     /*报告出错*/
            }
            SendFarmValue.temp_InValue = GetAverage_Temp(TEMP_0_INTERNAL);        /*获取盒内温度值*/
            
            /*进行逻辑判断，PWM输出、加热、烘干等操作*/
            PWM_output(SendFarmValue.montorValue, SendFarmValue.humValue, &AnalogCal);  /*PWM输出*/
            
            In_HeatProcess(SendFarmValue.temp_InValue);       /*内部加热处理*/
            Ln_HeatProcess(SendFarmValue.temp_ExValue);       /*防冷凝加热处理*/
            
            /*错误处理 放在最后进行处理*/
            ErrorProcess(g_ErrorState);
#if 1
            flagPowerOn ++;
            if(flagPowerOn >= ALANOG_CALI_TIME)
            {
                flagPowerOn = 1;
                AnalogCalibration_fun(&AnalogCal, 0, CALIBRATION_ALL_OUTPUT);      /*校准*/
            }
#endif
            /*发送消息*/
            err = xQueueSend(g_HumValueMsg, (void *)&sendQueueMessage, 2);
            if(err == pdFAIL)   /*在2个系统时间后依然不能发送消息*/
            {
                /*只有在开始进行了计数之后才会对错误计数进行累加*/
                if((*sendQueueMessage.count) > 0)
                {
                    sendQueueMessage.error_count++;
                }
            }
            else
            {
                sendQueueMessage.error_count = 0;   /*clear*/
            }
        }
    }
}
