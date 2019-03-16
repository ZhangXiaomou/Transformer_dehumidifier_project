/**
 * @file RtuFrame.c
 * @brief 通信帧处理
 * @date 2017.07.03
 * @note 该通信协议采用自定义协议，且为最简易的协议，校验码采用累加和校验，不考虑进位
 *        且都为无符号八位数据（校验码计算为数据长度加地址跟功能码累加和）
 *        格式：
 *        address    funcode    dataLength    data    crc
 *        一字节      一字节     一字节        可变    一字节
 *        地址       功能       数据长度      数据    累加和校验
 *    eg： 0xA5       0x20      0             NO      0xC5    上位机下发，读取数据
 *    eg:  0x55       0x20      0x01          0x01    0xC7    下位机应答
 * 
 * @author zhangxiaomou
 * @version ver 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef RTUFRAME_H
#define	RTUFRAME_H

/* Includes ------------------------------------------------------------------*/
#include "../SysParam.h"
  

/* 外部函数定义----------------------------------------------------------------*/
extern void  Init_ReciveFrameData();
extern void RecivFrameDispose(FarmValue *frameValue);

/*全局变量 --------------------------------------------------------------------*/
/* End -----------------------------------------------------------------------*/

#endif	/* XC_HEADER_TEMPLATE_H */

