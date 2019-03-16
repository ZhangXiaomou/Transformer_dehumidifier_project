/**
 * @file RtuFrame.c
 * @brief 通信帧处理
 * @date 2017.07.03
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "../Drive/DriveSerialPort.h"
#include "../Drive/DriveTemp.h"
#include "../Drive/DriveADC.h"
#include "../Drive/DriveIO.h"

#include "ReadADC.h"
#include "RtuFrame.h"
#include "ErrorProcess.h"
#include "DigitalFilter.h"
#include "RtuFrameCRC16.h"

/* 宏定义----------------------------------------------------------------------*/
#define LOCAL_ADDRESS   0xA5    /*本机地址与上位机地址相同*/
#define READ_DATA       0x20    /*功能代码*/
#define RECIVDATALEN    0x00    /*数据长度*/
#define FRAME_QUENE_LEN 128      /*队列长度*/
    
/* 结构体定义------------------------------------------------------------------*/
typedef struct tagFrameRtu
{
    uint8_t address;        /*地址*/
    uint8_t funcode;        /*功能代码*/
    uint8_t len;        /*数据长度*/
    uint8_t completeFlag;   /*完成标志位*/
} FrameRtu;
    
/* 内部变量定义----------------------------------------------------------------*/
static FrameRtu recvFrame;
static uint8_t *FrameBuffer;     /*临时存放串口接收数据*/
static uint8_t FrameBufferLen = 0;  /*未处理接收数据长度*/

static uint8_t sendBuffer[24] = {0};
static PointFloat FloatData;
/* 内部函数定义----------------------------------------------------------------*/
static void SendFrame(uint8_t *FrameBuffer, uint8_t len);

/* 内部函数实现----------------------------------------------------------------*/
/**
 * @fn _U2RXInterrupt
 * @brief 串口2接收中断
 * 
 */
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    if(U2STAbits.OERR == 1)  
    {
        U2STAbits.OERR = 0;      // Clear Overrun Error to receive data
    }
    else if ((U2STAbits.FERR ==0) && (U2STAbits.PERR ==0) )  
    { 
        FrameBuffer[FrameBufferLen++] = U2RXREG;
    }
    IFS1bits.U2RXIF = 0;    // clear RX interrupt flag
}

/**
 * 发送帧
 * @param FrameBuffer 生成的帧指针
 * @param len 帧长度
 */
static void SendFrame(uint8_t *buffer, uint8_t len)
{
    uint8_t i = 0;
    RX_TX_MODE = TX_MODE;
    __delay_us(2); //稳定的过渡时间
    for ( i = 0; i < len; i++)
    {
        Usart2Send(buffer[i]);
    }
    __delay_us(2); //稳定的过渡时间
    RX_TX_MODE = RX_MODE;
}

/**
 * @fn ：TestSend()
 * @brief ： 读取数据
 * 
 */
static void TestSend(FarmValue *frameValue)
{
    uint8_t index = 0;
    uint8_t i = 0; 
    uint16_t crc = 0;
    index = 3;
    sendBuffer[0] = LOCAL_ADDRESS;
    sendBuffer[1] = READ_DATA;
    
    FloatData.fData = frameValue->temp_InValue;
    for(i = 0; i < 4; i ++)
    {
        sendBuffer[index++] = FloatData.uData[i];
    }
    FloatData.fData = frameValue->temp_ExValue;
    for(i = 0; i < 4; i ++)
    {
        sendBuffer[index++] = FloatData.uData[i];
    }
    FloatData.fData = frameValue->humValue;
    for(i = 0; i < 4; i ++)
    {
        sendBuffer[index++] = FloatData.uData[i];
    }
    FloatData.fData = frameValue->montorValue;
    for(i = 0; i < 4; i ++)
    {
        sendBuffer[index++] = FloatData.uData[i];
    }
    FloatData.fData = frameValue->powerValue;
    for(i = 0; i < 4; i ++)
    {
        sendBuffer[index++] = FloatData.uData[i];
    }
    sendBuffer[index++] = (uint8_t)(g_ErrorState & 0x00FF);             /*错误标识位*/
    sendBuffer[index++] = (uint8_t)((g_ErrorState & 0xFF00) >> 8);      /*错误标识位*/
    sendBuffer[2] = index + 2;              /*数据长度*/
    crc = FrameCRC16(sendBuffer, index);    /*计算CRC*/
    sendBuffer[index++] = (uint8_t)(crc & 0x00FF);        /*获得校验码 低位*/
    sendBuffer[index++] = (uint8_t)((crc & 0xFF00) >> 8); /*获得校验码 高位*/
    
    SendFrame(sendBuffer, index);
}

/* 外部函数实现----------------------------------------------------------------*/
/**
 * @fn Init_ReciveFrameData()
 * @brief 初始化此文件模块有关变量
 * 
 */
void Init_ReciveFrameData(void)
{
    uint8_t i = 0;
    FrameBufferLen = 0;
    if(FrameBuffer == NULL)
    {
        FrameBuffer = pvPortMalloc(FRAME_QUENE_LEN);
        if(FrameBuffer == NULL)
        {
            Reset();    /*复位*/
        }
    }
    else
    {
        for(i = 0; i < FRAME_QUENE_LEN; i++)
        {
            FrameBuffer[i] = 0;
        }
    }
}

/**
 * @fn FrameBufferDataDealing()
 * @brief 对接收数据进行处理
 * @param
 * @return 
 */
void FrameBufferDataDealing(FrameRtu* pReciveFrame)
{
    uint16_t crc = 0;
    RX_TX_MODE = RX_MODE;           /*刷新接收模式使能*/
    if (FrameBufferLen >= FRAME_QUENE_LEN)  /*如果接收错误*/
    {
        FrameBufferLen = 0;
        return ;                   /*重新初始化并返回*/
    }
    if(U2STAbits.OERR)
    {
        U2STAbits.OERR = 0;
    }
#if 0
    if(FrameBufferLen > 0)
    {
        SendFrame(FrameBuffer, FrameBufferLen);
        for(crc = 0; crc < FrameBufferLen; crc++)
        {
            FrameBuffer[crc] = 0;
        }
        FrameBufferLen = 0;
    }
#else
    if((FrameBufferLen >= 5) && (U2STAbits.RIDLE))  /*接收器空闲且已经收到数据*/
    {
        uint8_t head = 0;
        pReciveFrame->completeFlag = FALSE;
        while((FrameBufferLen - head) >= 5)
        {
            if(FrameBuffer[head] != LOCAL_ADDRESS)
            {
                head++;
                continue;
            }
            pReciveFrame->funcode = FrameBuffer[head + 1];
            pReciveFrame->len = FrameBuffer[head + 2];
            /*计算CRC*/
            crc = FrameCRC16(&FrameBuffer[head], pReciveFrame->len);
            uint8_t crc_H = ((crc & 0xFF00) >> 8);
            uint8_t crc_L = (crc & 0x00FF);
            /*若校验吻合,则执行下一步动作*/
            if ((crc_L == FrameBuffer[pReciveFrame->len + head]) && \
                (crc_H == FrameBuffer[pReciveFrame->len + 1 + head]))
            {
                pReciveFrame->completeFlag = TRUE;
                FrameBufferLen -= (pReciveFrame->len + 5);
                return ;
            }
            else
            {
                head ++;
                continue;
            }
        }
#if 0
        SendFrame(FrameBuffer, FrameBufferLen);
#endif
        if(FrameBufferLen > (head + 5))
        {
            FrameBufferLen -= (head + 5);
        }
        else
        {
            FrameBufferLen = 0;
        }
    }
#endif
}
/**
 * @fn ExecuteFunctioncode()
 * @brief 执行 @brief 代码
 * @param 接收帧指针 frameRtu* pRtu
 * @return void
 */
static void ExecuteFunctioncode(FrameRtu* pRtu, FarmValue *frameValue)
{
    /*该数据帧未处理过*/
    if (pRtu->completeFlag == TRUE)
    {
        switch(pRtu->funcode)
        {
            case READ_DATA:
            {
                TestSend(frameValue);
                break;
            }
            default:
            {
                break;
            }
        }
        pRtu->completeFlag = FALSE;
    }
}

/**
 * @fn RecivFrameDispose()
 * @brief 接收帧处理
 * @returnbrief 
 */
void RecivFrameDispose(FarmValue *frameValue)
{ 
    FrameBufferDataDealing(&recvFrame);    /*返回剩余长度*/
    if(IEC1bits.U2RXIE != 1)
    {
        IEC1bits.U2RXIE = 1;
        
    }
    if (recvFrame.completeFlag == TRUE)
    {
        taskENTER_CRITICAL();
        ExecuteFunctioncode(&recvFrame, frameValue);
        taskEXIT_CRITICAL();
    }
}

/* End -----------------------------------------------------------------------*/
