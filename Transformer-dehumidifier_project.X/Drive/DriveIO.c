/**
 * @file DriveIO.c
 * @brief IO口驱动，控制IO口输出输入，提供全局的IO口控制宏定义
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "DriveIO.h"

/*全局变量定义 ----------------------------------------------------------------*/
LedTypdef g_LedTypdef[LED_OVER];        //LED指示灯操作
RelayTypdef g_RelayTypdef[RELAY_OVER];  //继电器操作

/*内部函数定义 ----------------------------------------------------------------*/
static void Init_Led_Data();
static void Led_In_Green_On();
static void Led_In_Green_Off();
static void Led_In_Yellow_On();
static void Led_In_Yellow_Off();
static void Led_In_Red_On();
static void Led_In_Red_Off();
static void Led_Ex_Green_On();
static void Led_Ex_Green_Off();
static void Led_Ex_Yellow_On();
static void Led_Ex_Yellow_Off();
static void Led_Ex_Red_On();
static void Led_Ex_Red_Off();

static void Init_Relay_Data(void);
static void Relay_Error_On(void);
static void Relay_Error_Off(void);
static void Relay_Display_On(void);
static void Relay_Display_Off(void);
static void Relay_Ex_On(void);
static void Relay_Ex_Off(void);
static void Relay_In_On(void);
static void Relay_In_Off(void);
static void Relay_Ln_On(void);
static void Relay_Ln_Off(void);
/*内部函数实现 ----------------------------------------------------------------*/
/**
 * @fn Led_In_Green_On
 * @brief 内部绿色指示灯亮
 * 
 */
inline static void Led_In_Green_On(void)
{
    RUN_LED = ON;
}
/**
 * @fn Led_In_Green_Off
 * @brief 内部绿色指示灯熄灭
 * 
 */
inline static void Led_In_Green_Off(void)
{
    RUN_LED = OFF;
}
/**
 * @fn Led_In_Yellow_On
 * @brief 内部黄色指示灯亮
 * 
 */
inline static void Led_In_Yellow_On(void)
{
    IN_Y_LED = ON;
}
/**
 * @fn Led_In_Yellow_Off
 * @brief 内部黄色指示灯熄灭
 * 
 */
inline static void Led_In_Yellow_Off(void)
{
    IN_Y_LED = OFF;
}
/**
 * @fn Led_In_Red_On
 * @brief 内部红色指示灯亮
 * 
 */
inline static void Led_In_Red_On(void)
{
    IN_R_LED = ON;
}
/**
 * @fn Led_In_Red_Off
 * @brief 内部红色指示灯熄灭
 * 
 */
inline static void Led_In_Red_Off(void)
{
    IN_R_LED = OFF;
}
/**
 * @fn Led_Ex_Green_On
 * @brief 内部绿色指示灯亮
 * 
 */
inline static void Led_Ex_Green_On(void)
{
    OUT_G_LED = ON;
}
/**
 * @fn Led_Ex_Green_Off
 * @brief 外部绿色指示灯熄灭
 * 
 */
inline static void Led_Ex_Green_Off(void)
{
    OUT_G_LED = OFF;
}
/**
 * @fn Led_Ex_Yellow_On
 * @brief 外部黄色指示灯亮起
 * 
 */
inline static void Led_Ex_Yellow_On(void)
{
    OUT_Y_LED = ON;
}
/**
 * @fn Led_Ex_Yellow_Off
 * @brief 外部黄色指示灯熄灭
 * 
 */
inline static void Led_Ex_Yellow_Off(void)
{
    OUT_Y_LED = OFF;
}
/**
 * @fn Led_Ex_Red_On
 * @brief 外部红色指示灯亮
 * 
 */
inline static void Led_Ex_Red_On(void)
{
    OUT_R_LED = ON;
}
/**
 * @fn Led_Ex_Red_Off
 * @brief 外部红色指示灯熄灭
 * 
 */
inline static void Led_Ex_Red_Off(void)
{
    OUT_R_LED = OFF;
}

/**
 * @fn Init_Led_Data
 * @brief 初始化LED数据
 * 
 */
static void Init_Led_Data(void)
{
    g_LedTypdef[INTERNAL_GREEN].mode = LED_MODE_OFF;
    g_LedTypdef[INTERNAL_GREEN].status = LED_STATUS_OFF;
    g_LedTypdef[INTERNAL_GREEN].count = 0;
    g_LedTypdef[INTERNAL_GREEN].freq = 0;
    g_LedTypdef[INTERNAL_GREEN].LedOn = Led_In_Green_On;
    g_LedTypdef[INTERNAL_GREEN].LedOff = Led_In_Green_Off;
    
    g_LedTypdef[INTERNAL_YELLOW].mode = LED_MODE_OFF;
    g_LedTypdef[INTERNAL_YELLOW].status = LED_STATUS_OFF;
    g_LedTypdef[INTERNAL_YELLOW].count = 0;
    g_LedTypdef[INTERNAL_YELLOW].freq = 0;
    g_LedTypdef[INTERNAL_YELLOW].LedOn = Led_In_Yellow_On;
    g_LedTypdef[INTERNAL_YELLOW].LedOff = Led_In_Yellow_Off;
    
    g_LedTypdef[INTERNAL_RED].mode = LED_MODE_OFF;
    g_LedTypdef[INTERNAL_RED].status = LED_STATUS_OFF;
    g_LedTypdef[INTERNAL_RED].count = 0;
    g_LedTypdef[INTERNAL_RED].freq = 0;
    g_LedTypdef[INTERNAL_RED].LedOn = Led_In_Red_On;
    g_LedTypdef[INTERNAL_RED].LedOff = Led_In_Red_Off;
    
    g_LedTypdef[EXTERNAL_GREEN].mode = LED_MODE_OFF;
    g_LedTypdef[EXTERNAL_GREEN].status = LED_STATUS_OFF;
    g_LedTypdef[EXTERNAL_GREEN].count = 0;
    g_LedTypdef[EXTERNAL_GREEN].freq = 0;
    g_LedTypdef[EXTERNAL_GREEN].LedOn = Led_Ex_Green_On;
    g_LedTypdef[EXTERNAL_GREEN].LedOff = Led_Ex_Green_Off;
    
    g_LedTypdef[EXTERNAL_YELLOW].mode = LED_MODE_OFF;
    g_LedTypdef[EXTERNAL_YELLOW].status = LED_STATUS_OFF;
    g_LedTypdef[EXTERNAL_YELLOW].count = 0;
    g_LedTypdef[EXTERNAL_YELLOW].freq = 0;
    g_LedTypdef[EXTERNAL_YELLOW].LedOn = Led_Ex_Yellow_On;
    g_LedTypdef[EXTERNAL_YELLOW].LedOff = Led_Ex_Yellow_Off;
    
    g_LedTypdef[EXTERNAL_RED].mode = LED_MODE_OFF;
    g_LedTypdef[EXTERNAL_RED].status = LED_STATUS_OFF;
    g_LedTypdef[EXTERNAL_RED].count = 0;
    g_LedTypdef[EXTERNAL_RED].freq = 0;
    g_LedTypdef[EXTERNAL_RED].LedOn = Led_Ex_Red_On;
    g_LedTypdef[EXTERNAL_RED].LedOff = Led_Ex_Red_Off;
    
    Set_Led(&g_LedTypdef[INTERNAL_GREEN], LED_MODE_OFF, 0);
    Set_Led(&g_LedTypdef[INTERNAL_YELLOW], LED_MODE_OFF, 0);
    Set_Led(&g_LedTypdef[INTERNAL_RED], LED_MODE_OFF, 0);
    Set_Led(&g_LedTypdef[EXTERNAL_GREEN], LED_MODE_OFF, 0);
    Set_Led(&g_LedTypdef[EXTERNAL_YELLOW], LED_MODE_OFF, 0);
    Set_Led(&g_LedTypdef[EXTERNAL_RED], LED_MODE_OFF, 0);
}

/**
 * @fn Relay_Error_On
 * @brief 错误指示继电器闭合
 * 
 */
inline static void Relay_Error_On(void)
{
    ERROR_RELAY5 = ON;
}
/**
 * @fn Relay_Error_Off
 * @brief 错误指示继电器断开
 * 
 */
inline static void Relay_Error_Off(void)
{
    ERROR_RELAY5 = OFF;
}

/**
 * @fn Relay_Display_On
 * @brief 加热指示继电器闭合
 * 
 */
inline static void Relay_Display_On(void)
{
    HEATING_DISPALY = ON;
}
/**
 * @fn Relay_Display_Off
 * @brief 加热指示继电器断开
 * 
 */
inline static void Relay_Display_Off(void)
{
    HEATING_DISPALY = OFF;
}

/**
 * @fn Relay_Ex_On
 * @brief 烘干继电器闭合
 * 
 */
inline static void Relay_Ex_On(void)
{
    OUT_HEART_A = ON;
    OUT_HEART_B = ON;
}
/**
 * @fn Relay_Ex_Off
 * @brief 烘干继电器断开
 * 
 */
inline static void Relay_Ex_Off(void)
{
    OUT_HEART_A = OFF;
    OUT_HEART_B = OFF;
}

/**
 * @fn Relay_In_On
 * @brief 盒内加热继电器闭合
 * 
 */
inline static void Relay_In_On(void)
{
    IN_HEART = ON;
}
/**
 * @fn Relay_In_Off
 * @brief 盒内加热继电器断开
 * 
 */
inline static void Relay_In_Off(void)
{
    IN_HEART = OFF;
}

/**
 * @fn Relay_Ln_On
 * @brief 防冷凝加热开启
 * 
 */
inline static void Relay_Ln_On(void)
{
    LN_HEART = ON;
}
/**
 * @fn Relay_Ln_Off
 * @brief 防冷凝加热关闭
 * 
 */
inline static void Relay_Ln_Off(void)
{
    LN_HEART = OFF;
}


/**
 * @fn Init_Relay_Data
 * @brief 继电器控制参数初始化
 * 
 */
static void Init_Relay_Data(void)
{
    g_RelayTypdef[RELAY_ERROR_STATE].mode = RELAY_MODE_OFF;
    g_RelayTypdef[RELAY_ERROR_STATE].RelayOn = Relay_Error_On;
    g_RelayTypdef[RELAY_ERROR_STATE].RelayOff = Relay_Error_Off;
    
    g_RelayTypdef[RELAY_HEAT_STATE].mode = RELAY_MODE_OFF;
    g_RelayTypdef[RELAY_HEAT_STATE].RelayOn = Relay_Display_On;
    g_RelayTypdef[RELAY_HEAT_STATE].RelayOff = Relay_Display_Off;
    
    g_RelayTypdef[RELAY_EX_HEAT].mode = RELAY_MODE_OFF;
    g_RelayTypdef[RELAY_EX_HEAT].RelayOn = Relay_Ex_On;
    g_RelayTypdef[RELAY_EX_HEAT].RelayOff = Relay_Ex_Off;
    
    g_RelayTypdef[RELAY_IN_HEAT].mode = RELAY_MODE_OFF;
    g_RelayTypdef[RELAY_IN_HEAT].RelayOn = Relay_In_On;
    g_RelayTypdef[RELAY_IN_HEAT].RelayOff = Relay_In_Off;
    
    g_RelayTypdef[RELAY_LN_HEAT].mode = RELAY_MODE_OFF;
    g_RelayTypdef[RELAY_LN_HEAT].RelayOn = Relay_Ln_On;
    g_RelayTypdef[RELAY_LN_HEAT].RelayOff = Relay_Ln_Off;
    
    Set_Relay(&g_RelayTypdef[RELAY_ERROR_STATE], RELAY_MODE_OFF);
    Set_Relay(&g_RelayTypdef[RELAY_HEAT_STATE], RELAY_MODE_OFF);
    Set_Relay(&g_RelayTypdef[RELAY_EX_HEAT], RELAY_MODE_OFF);
    Set_Relay(&g_RelayTypdef[RELAY_IN_HEAT], RELAY_MODE_OFF);
    Set_Relay(&g_RelayTypdef[RELAY_LN_HEAT], RELAY_MODE_OFF);
}
/*外部函数实现 ----------------------------------------------------------------*/
/**
 * @fn InitDeviceIO
 * @brief IO初始化函数
 * 
 */
void Init_IO(void)
{
    /**不允许中断嵌套*/
    INTCON1bits.NSTDIS = 1;
    /*将未用到的引脚默认为弱上拉输入*/
    TRISEbits.TRISE0 = 1;
    TRISEbits.TRISE1 = 1;
    TRISFbits.TRISF0 = 1;
    TRISBbits.TRISB4 = 1;
    
    /**继电器控制引脚，IO模式*/
    OUT0_DIR = 0;   //外部加热继电器A 输出
    OUT1_DIR = 0;   //外部加热继电器B 输出
    OUT2_DIR = 0;   //内部加热继电器 输出
    OUT3_DIR = 0;   //防冷凝加热器 输出
    OUT4_DIR = 0;   //加热指示继电器 输出
    OUT5_DIR = 0;   //故障指示继电器 输出
    /**继电器控制引脚，初始化*/
    IN_HEART = OFF;         //内部加热继电器 关闭
    LN_HEART = OFF;         //冷凝加热器     关闭
    OUT_HEART_A = OFF;      //外部加热继电器 关闭
    OUT_HEART_B = OFF;      //外部加热继电器 关闭
    HEATING_DISPALY = OFF;  //加热指示继电器
    ERROR_RELAY5 = OFF;     //故障指示继电器
    /**指示灯控制引脚，IO模式*/
    RUN_LED_DIR = 0;    //内部绿灯 输出
    IN_Y_LED_DIR = 0;   //内部黄灯 输出
    IN_R_LED_DIR = 0;   //内部红灯 输出
    OUT_G_LED_DIR = 0;  //外部绿灯 输出
    OUT_Y_LED_DIR = 0;  //外部黄灯 输出
    OUT_R_LED_DIR = 0;  //外部红灯 输出
    /**指示灯控制引脚，初始化*/
    RUN_LED = ON;       //内部绿灯 点亮
    IN_Y_LED = OFF;     //内部黄灯 熄灭
    IN_R_LED = OFF;     //内部红灯 熄灭
    OUT_G_LED = ON;     //外部绿灯 点亮
    OUT_Y_LED = OFF;    //外部黄灯 熄灭
    OUT_R_LED = OFF;    //外部红灯 熄灭
    
    Init_Led_Data();    //LED控制参数初始化
    Init_Relay_Data();  //继电器控制参数初始化
}

/**
 * @fn 
 * @param 
 * @param ledTypdef 指向LED结构的指针
 * @param mode LED模式
 * @param freq  LED闪烁频率 不是闪烁模式无作用
 * 
 */
inline void Set_Led(LedTypdef *ledTypdef, uint8_t mode, uint16_t freq)
{
    ledTypdef->mode = mode;
    ledTypdef->freq = freq;
}

/**
 * @fn 
 * @param 
 * @param relayTypdef 指向继电器结构的指针
 * @param mode 模式
 * @param onTime 闭合时间，任何模式可用，为0时默认不会断开
 * 
 */
inline void Set_Relay(RelayTypdef* relayTypdef, uint8_t mode)
{
    relayTypdef->mode = mode;
}
/*End - ----------------------------------------------------------------------*/

