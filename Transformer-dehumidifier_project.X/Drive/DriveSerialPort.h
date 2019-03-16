/**
 * @file DriveSerialPort.h
 * @brief 串口模块配置
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */


#ifndef USART_H
#define	USART_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"

/*宏定义 ----------------------------------------------------------------------*/
#define TX   LATFbits.LATF5
#define TX_T TRISFbits.TRISF5
#define RX   PORTFbits.RF4
#define RX_T TRISFbits.TRISF4

#define RX_TX_MODE  LATFbits.LATF1
#define RX_TX_MODE_T TRISFbits.TRISF1
#define RX_MODE 0
#define TX_MODE 1
    
#define ON_UART_INT()  {IEC1bits.U2RXIE = 1;	}
#define OFF_UART_INT()  {IEC1bits.U2RXIE = 0;	}    

/*函数定义 -------------------------------------------------------------------*/
void Init_SerialPort();
void Usart2Send(uint8_t data);

/*End ------------------------------------------------------------------------*/

#endif	/* USART_H */

