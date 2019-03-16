/**
 * @file RtuFrameCRC16.h
 * @brief CRC16位校验
 * @date 2018.08.30
 * @author zhangxiaomou
 * @version ver 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _RTUFRAME_CRC16_H
#define	_RTUFRAME_CRC16_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"
  

/* 外部函数定义----------------------------------------------------------------*/
uint16_t FrameCRC16(uint8_t *puchMsg, uint8_t usDataLen);

/*全局变量 --------------------------------------------------------------------*/


/* End -----------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

