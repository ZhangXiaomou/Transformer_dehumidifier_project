/**
 * @file DrivePWM.c
 * @brief PWM输出模块配置
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "DrivePWM.h"

/*宏定义 ----------------------------------------------------------------------*/

/*PWM周期*/
#define FPWM 2000   //1KHz

/*函数实现 --------------------------------------------------------------------*/
/**************************************************
 * 函数名：PWMInit()
 * 形参： void
 * 返回值：void
 * 功能： PWM模块初始化
 * 引用：
****************************************************/

/**
 * @fn Init_PWM
 * @brief PWM模块初始化并使能输出，周期为 FPWM*2 Hz
 * 
 */
void Init_PWM(void)
{
    TRISEbits.TRISE2 = 0;   // PWM pins as outputs, and FLTA as input
    PTPER = FPWM;           // Compute Period based on CPU speed and
                            // required PWM frequency (see defines)100Hz
    OVDCON = 0x0000;        // Disable all PWM outputs.
    DTCON1 = 0x0010;        // ~1 us of dead time
    PWMCON1 = 0x0202;       // Enable PWM output pins and configure them as 0x0202;
                            // independence mode
    PDC2 = PTPER;           // Initialize as 0 voltage
    SEVTCMP = 1;            // Enable triggering for ADC
    PWMCON2 = 0x0F02;       // 16 postscale values, for achieving 20 kHz
    IEC2bits.PWMIE = 0;     // Disable PWM interrupts
    IFS2bits.PWMIF = 0;     // Clear interrupt flag
    PTCON = 0x8002;         // start PWM as center aligned mode, 1:4后分频
    
    OVDCON = 0x0000;        // Turn ON low side transistors to charge03
    PWMCON2bits.UDIS = 1;
    PDC2 = 1.0f * PTPER;    // output 4mA
    
    OVDCON = 0x0400;        // Configure PWM0-5 to be governed by PWM module
    PWMCON2bits.UDIS = 0;
}

