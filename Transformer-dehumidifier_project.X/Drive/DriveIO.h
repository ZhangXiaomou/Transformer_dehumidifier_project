/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/**
 * @file DriveIO.c
 * @brief IO口驱动，控制IO口输出输入，提供全局的IO口控制宏定义
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_DEVICEIO_H
#define	XC_DEVICEIO_H

/*Include --------------------------------------------------------------------*/
#include "../SysParam.h"

/*宏定义 - --------------------------------------------------------------------*/

#define ON  1
#define OFF 0

/*ADC相关 - -------------------------------------------------------------------*/
/*湿度传感器*/ 
#define HUM1        PORTBbits.RB0
#define HUM1_DIR    TRISBbits.TRISB0
#ifdef USE_TEMP_2_EXTERNAL
#define HUM2        PORTBbits.RB1
#define HUM2_DIR    TRISBbits.TRISB1
#endif
/*24V 系统电压监控*/
#define U_ADC        PORTBbits.RB2
#define U_ADC_DIR    TRISBbits.TRISB2
/*模拟输出口检测*/
#define U_MONITOR        PORTBbits.RB3
#define U_MONITOR_DIR    TRISBbits.TRISB3

/*温度传感器 ------------------------------------------------------------------*/
#define TERM0_IN    PORTEbits.RE5
#define TERM0_OUT   LATEbits.LATE5
#define TERM0_DIR   TRISEbits.TRISE5
#define TERM1_IN    PORTEbits.RE4
#define TERM1_OUT   LATEbits.LATE4
#define TERM1_DIR   TRISEbits.TRISE4
#define TERM2_IN    PORTEbits.RE3
#define TERM2_OUT   LATEbits.LATE3
#define TERM2_DIR   TRISEbits.TRISE3
    
/*普通IO输出引脚 --------------------------------------------------------------*/
/*继电器控制*/
#define OUT0     LATFbits.LATF6
#define OUT0_DIR TRISFbits.TRISF6
#define OUT1     LATDbits.LATD0
#define OUT1_DIR TRISDbits.TRISD0
#define OUT4     LATDbits.LATD1
#define OUT4_DIR TRISDbits.TRISD1
#define OUT2     LATDbits.LATD2
#define OUT2_DIR TRISDbits.TRISD2
#define OUT3     LATDbits.LATD3
#define OUT3_DIR TRISDbits.TRISD3
#define OUT5     LATEbits.LATE8
#define OUT5_DIR TRISEbits.TRISE8
/*内部指示灯*/
#define RUN_LED       LATCbits.LATC14
#define RUN_LED_DIR   TRISCbits.TRISC14
#define IN_Y_LED      LATCbits.LATC13
#define IN_Y_LED_DIR  TRISCbits.TRISC13
#define IN_R_LED      LATBbits.LATB8
#define IN_R_LED_DIR  TRISBbits.TRISB8
/*外部指示灯*/
#define OUT_G_LED     LATBbits.LATB7
#define OUT_G_LED_DIR TRISBbits.TRISB7
#define OUT_Y_LED     LATBbits.LATB6
#define OUT_Y_LED_DIR TRISBbits.TRISB6
#define OUT_R_LED     LATBbits.LATB5
#define OUT_R_LED_DIR TRISBbits.TRISB5

/*控制输出，方便程序维护修改*/
#define OUT_HEART_A   OUT0    //外部加热器
#define OUT_HEART_B   OUT1    //外部加热器
#define IN_HEART      OUT2    //内部加热器
#define LN_HEART      OUT3    //防冷凝加热器
#define HEATING_DISPALY   OUT5     //加热指示继电器
#define ERROR_RELAY5      OUT4     //故障指示继电器


/*数据结构定义 ----------------------------------------------------------------*/
typedef struct tagLedStruct
{
    uint8_t mode;   //模式（常亮，常灭，闪烁）
    uint8_t status;  //当前状态
    uint16_t freq;   //闪烁频率 (freq * LED_COUNT_PERIOD 毫秒)
    uint16_t count; //LED状态计数
    
    void (*LedOn) (void);   //LED开启函数指针
    void (*LedOff) (void);  //LED关闭函数指针
    
}LedTypdef;
typedef struct tagRelayStruct
{
    uint8_t mode;   /*模式（闭合，断开）*/
    
    void (*RelayOn) (void);   /*继电器开启函数指针*/
    void (*RelayOff) (void);  /*继电器关闭函数指针*/
    
}RelayTypdef;


/*枚举定义 --------------------------------------------------------------------*/
enum LedTypdef_enum
{
    INTERNAL_GREEN = 0,     //内部绿灯
    INTERNAL_YELLOW   ,     //内部黄灯
    INTERNAL_RED      ,     //内部红灯
    
    EXTERNAL_GREEN    ,     //外部绿灯
    EXTERNAL_YELLOW   ,     //外部黄灯
    EXTERNAL_RED      ,     //外部红灯
    
    LED_OVER          ,     //定义结束
    
    LED_MODE_ON       ,     //LED常亮模式
    LED_MODE_OFF      ,     //LED常灭模式
    LED_MODE_FLICKER  ,     //LED闪烁模式
    
    LED_STATUS_ON     ,     //LED亮状态
    LED_STATUS_OFF    ,     //LED灭状态
    
    LedTypdef_enum_OVER = 0xFF
};

enum RelayTypdef_enum
{
    RELAY_ERROR_STATE = 0,     //错误指示继电器
    RELAY_HEAT_STATE     ,     //加热指示继电器
    RELAY_EX_HEAT        ,     //外部加热继电器
    RELAY_IN_HEAT        ,     //内部加热继电器
    RELAY_LN_HEAT        ,     //防冷凝加热继电器
    
    RELAY_OVER           ,     //定义结束
    
    RELAY_MODE_ON       ,     //继电器常闭模式
    RELAY_MODE_OFF      ,     //继电器常开模式
    
    RelayTypdef_enum_OVER = 0xFF
};

/*函数定义 -------------------------------------------------------------------*/
void Init_IO(void);
void Set_Led(LedTypdef *ledTypdef, uint8_t mode, uint16_t freq);
void Set_Relay(RelayTypdef* relayTypdef, uint8_t mode);

/*全局变量定义 ----------------------------------------------------------------*/
extern LedTypdef g_LedTypdef[LED_OVER];         //LED指示灯操作
extern RelayTypdef g_RelayTypdef[RELAY_OVER];   //继电器操作

/*End ------------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

