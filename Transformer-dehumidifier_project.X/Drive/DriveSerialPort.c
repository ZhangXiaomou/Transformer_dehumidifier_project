/**
 * @file DriveSerialPort.c
 * @brief 串口模块配置
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "DriveSerialPort.h"

#include "../FreeRTOS/FreeRTOSConfig.h"
#include "../App/RtuFrame.h"

/* 外部函数实现----------------------------------------------------------------*/
#if 0
#include "stdio.h"	
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
    U2TXREG = (uint8_t)ch;
    while(!U2STAbits.TRMT)
    {
    } 
	return ch;
}
#endif 
/**
 * @fn Init_UART2
 * @brief 初始化串口2的引脚与控制器
 * @param baud 波特率
 */
static void Init_UART2(uint16_t baud)
{
    float value = 0;
    /**IO口初始化*/
    RX_T = 1;
    TX_T = 0;
    RX_TX_MODE_T = 0;
    RX_TX_MODE = RX_MODE;
    /**控制器初始化*/
    U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
    U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
    U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
    U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
    U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit
    value = (float)configCPU_CLOCK_HZ /(float)(16 * baud) - 1; //= FCY/(16 * (BRG + 1))
    U2BRG = 25;             //25-9600
    U2STAbits.UTXBRK = 0;	//Bit11 Disabled
    U2STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
    U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
    U2STAbits.TRMT = 0;     //Bit8 *Read Only bit*
    U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
    U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
    U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
    U2STAbits.PERR = 0;		//Bit3 *Read Only Bit*
    U2STAbits.FERR = 0;		//Bit2 *Read Only Bit*
    U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
    U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*
    
    IPC6bits.U2RXIP = PRIO_USART;    /*设置串口优先级*/
    IFS1bits.U2TXIF = 0;	/* Clear the Transmit Interrupt Flag*/
    IFS1bits.U2RXIF = 0;	/* Clear the Recieve Interrupt Flag*/
    IEC1bits.U2RXIE = 1;	/*使能接收中断*/ 
    
    U2MODEbits.UARTEN = 1;	// And turn the peripheral on
    U2STAbits.UTXEN = 1;
}

/**
 * @fn Init_SerialPort
 * @brief 初始化串口
 * 
 */
void Init_SerialPort(void)
{
    Init_UART2(9600);
    RX_TX_MODE = RX_MODE;
}

/**
 * @fn UsartSend
 * @brief 串口2发送一个字节
 * @param data 要发送的数据
 */
void Usart2Send(uint8_t data)
{
    U2TXREG = data;
    while(!U2STAbits.TRMT);
}


/* 中断函数 -------------------------------------------------------------------*/

/* End -----------------------------------------------------------------------*/

