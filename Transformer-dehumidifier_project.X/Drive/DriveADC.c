/**
 * @file DriveADC.c
 * @brief ADC采样通道频率等参数配置
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "DriveADC.h"

/*宏定义 - --------------------------------------------------------------------*/
#define NUM_CHS2SCAN 1  /*转换通道数量定义*/

/*外部函数定义 ----------------------------------------------------------------*/

/**
 * @fn Init_ADC
 * @brief 初始化ADC采样引脚与配置
 * @note AN0-HUM1 AN1-HUM2 AN3-U_
 * AN0->ADCBUF1.ADCBUF1连续采样两次 AN1->ADCBUF2 AN2-ADCBUF3 AN3->ADCBUF0
 * 
 */
void Init_ADC(void)
{
    ADCON1bits.ADON = 0;    //关闭ADC
    ADCON1bits.FORM   = 0;  //整数
    ADCON1bits.SSRC   = 0;  // 清零 SAMP 位时结束采样并启动转换
    ADCON1bits.ASAM   = 0;  // ADC Sample Control: Sampling begins immediately after conversion
    ADCON1bits.SIMSAM = 1;
    ADCON2bits.CSCNA = 0;   // Scan Input Selections for CH0+ during Sample A bit
    ADCON2bits.SMPI  = (NUM_CHS2SCAN - 1);	// 1 ADC Channel is scanned
    //同时采样 CH0、 CH1 （当 CHPS = 01 时） ADD++++++++++++++
    ADCON2bits.CHPS = 0b10;
    /*ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us*/ 
    ADCON2bits.VCFG = 0;        //参考电源
    ADCON3bits.ADRC = 0;		// ADC Clock is derived from Systems Clock
    ADCON3bits.ADCS = 15;		// ADC Conversion Clock Tad=Tcy/2*(ADCS+1)= (1/4M/2)*12 = 
                                // ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us
    ADCHSbits.CH0SA = 3;        //CH0+ = AN3
    ADCHSbits.CH0NA = 0;        //选择vref作为CH0-输入
    ADCHSbits.CH123NA = 0b01;   //选择vref作为CH1-、CH2-、CH3-输入
    ADCHSbits.CH123SA = 0;      //CH1+ = AN0， CH2+ = AN1， CH3+ = AN2   
    ADCSSL = 0x0000;    
    ADCSSL = 0x000D;        //0x0000 0000 0000 1101 关闭AN1不作为ADC端口
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG0 = 0;   //AN0
    ADPCFGbits.PCFG1 = 1;   //AN1
    ADPCFGbits.PCFG2 = 0;   //AN2
    ADPCFGbits.PCFG3 = 0;   //AN3
    
    TRISBbits.TRISB0 = 1;   //AN0
    TRISBbits.TRISB1 = 1;   //AN1
    TRISBbits.TRISB2 = 1;   //AN2
    TRISBbits.TRISB3 = 1;   //AN3
    
    IFS0bits.ADIF = 0;      // Clear the A/D interrupt flag bit
    IEC0bits.ADIE = 0;      // Disable A/D interrupt
    ADCON1bits.ADON = 0;    // Turn on the A/D converter
}

/**
 * @fn SoftSampleOnce
 * @brief 软件启动转换并获取ADC值
 * 
 */
void SoftSampleOnce(void)
{
    ADCON1bits.ADON = 1; //启动转换
    ADCON1bits.SAMP = 1;
    __delay_us(20);
    ADCON1bits.SAMP = 0;
    __delay_ms(3);
    /*3ms 若不能完成，说明转换异常，触发看门狗复位*/
    while(!ADCON1bits.DONE);
    ADCON1bits.ADON = 0;
}

/*End - ----------------------------------------------------------------------*/

