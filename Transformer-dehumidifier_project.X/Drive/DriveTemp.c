/**
 * @file DriveTemp.c
 * @brief DS18B20温度传感器初始化，获取温度值等
 * @date 2017.06.30
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */
#include "../SysParam.h"
#include "DriveTemp.h"
#include "DriveIO.h"

/*函数定义 ----------------------------------------------------------------------*/
static uint8_t DS18B20Reset(uint8_t ch);

/*函数实现 ----------------------------------------------------------------------*/
inline static void OutputHigh(uint8_t ch)
{
    switch (ch)
    {
        case TEMP_0_INTERNAL:
        {
            TERM0_OUT = 1;
            return;
        }
        case TEMP_1_EXTERNAL:
        {
            TERM1_OUT = 1;
            return;
        }
        
#ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {
            TERM2_OUT = 1;
            return;
        }
#endif
        
        default:
        {
            break;
        }
    }
}
inline static void OutputLow(uint8_t ch)
{
    switch (ch)
    {
        case TEMP_0_INTERNAL:
        {
            TERM0_OUT = 0;
            return;
        }
        case TEMP_1_EXTERNAL:
        {
            TERM1_OUT = 0;
            return;
        }
        
#ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {
            TERM2_OUT = 0;
            return;
        }
#endif
        
        default:
        {
            break;
        }
    }
}
inline static void SetOutput(uint8_t ch)
{
    switch (ch)
    {
        case TEMP_0_INTERNAL:
        {
            TERM0_DIR = 0;
            return;
        }
        case TEMP_1_EXTERNAL:
        {
            TERM1_DIR = 0;
            return;
        }
        
#ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {
            TERM2_DIR = 0;
            return;
        }
#endif
        
        default:
        {
            break;
        }
    }
}
inline static void SetInput(uint8_t ch)
{
    switch (ch)
    {
        case TEMP_0_INTERNAL:
        {
            TERM0_DIR = 1;
            return;
        }
        case TEMP_1_EXTERNAL:
        {
            TERM1_DIR = 1;
            return;
        }
#ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {
            TERM2_DIR = 1;
            return;
        }
#endif
        default:
        {
            break;
        }
    }
}
inline static uint8_t GepInput(uint8_t ch)
{
    switch (ch)
    {
        case TEMP_0_INTERNAL:
        {
            return TERM0_IN;
        }
        case TEMP_1_EXTERNAL:
        {
            return TERM1_IN;
        }
#ifdef USE_TEMP_2_EXTERNAL
        case TEMP_2_EXTERNAL:
        {
            return TERM2_IN;
        }
#endif
        default:
        {
            break;
        }
    }
    return 0;
}

/**
 * @fn DS18B20Reset
 * @brief DS18B20复位重新初始化，并检查
 * @param ch  需要转换的通道
 * @return 0    成功
 *          1    失败
 */
uint8_t static DS18B20Reset(uint8_t ch)
{
    SetOutput(ch);      /*设置为输出*/
    OutputLow(ch);      /*拉低*/

    __delay_us(480);
    //产生复位脉冲后，微处理器释放总线,让总线处于空闲状态
    OutputHigh(ch);      /*输出高电平*/
    __delay_us(20);

    uint8_t result = 0;
    /*检测是否复位成功*/
    SetInput(ch);        /*设置为输入*/
    while(GepInput(ch) && (result < 200))
    {
        result++;
        __delay_us(1);
    }
    if(result >= 200)
    {
        return 1;   
    }
    result=0;
    while(!GepInput(ch) && (result < 240))
    {
        result++;
        __delay_us(1);
    }
    if(result >= 240)
    {
        return 1;	   
    }
    return 0;
}

/**
 * @fn DS18B20ReadBit
 * @brief 从DS18B20读取一个位
 * @param ch  需要转换的通道
 * @return 0 / 1
 */
inline static uint8_t DS18B20ReadBit(uint8_t ch)    //读一位
{
   uint8_t dat;
   SetOutput(ch);       /*设置为输出*/
   OutputLow(ch);       /*拉低*/
   __delay_us(2);
   OutputHigh(ch);      /*拉高*/
   SetInput(ch);        /*设置为输入*/
   __delay_us(3);
   dat = GepInput(ch);  /*获取*/
   __delay_us(80);
   return dat;          /*返回有效数据*/
}

/**
 * @fn DS18B20ReadByte
 * @brief 从DS18B20读取一个字节
 * @param ch  需要转换的通道
 * @return 读到的数据
 */
uint8_t static DS18B20ReadByte(uint8_t ch) //读一字节
{
	uint8_t value = 0;
    uint8_t i = 0;
    uint8_t j = 0;
	for(i = 0; i < 8; i++)
    {
        j = DS18B20ReadBit(ch);   //先读的是低位
        value = (j << 7) | (value >> 1);  		  //j只能等于0或1，把j向左移７位
    }
	return value;        //返回一个字节的数据
}

/**
 * @fn DS18B20WriteByte
 * @brief 从DS18B20写入一个字节
 * @param ch  需要转换的通道
 * @param dat 要写入的数据
 * 
 */
void static DS18B20WriteByte(uint8_t ch, uint8_t dat) //写一个字节
{
    uint8_t i = 0;
    uint8_t onebit = 0;
    SetOutput(ch);
    /*写周期开始*/
    OutputLow(ch);
    __delay_us(1);
    
    for(i = 1; i <= 8; i++) 
    {
        onebit= dat & 0x01;
        dat = dat>>1;
        if(onebit)      //写 1
        {
            OutputLow(ch);
            __delay_us(1);
            OutputHigh(ch);
            __delay_us(60);
        }
        else         //写 0
        {
            OutputLow(ch);
            __delay_us(80);
            OutputHigh(ch);
            __delay_us(2);
        }
    }
}

/**
 * @fn DS18B20GetTemperature
 * @brief 获取温度值
 * @note 在上电时需要将delay设置为750，这是DS18B20特有的特性，不然后期不能正常转换得
 * 到正确的温度值
 * 
 * @param ch 选择要转换的通道（即哪个传感器）
 * @param tempValue 指向转换得到的温度值的指针
 * @param delay 转换延时ms
 * @return 0    成功
 *          其他 失败
 */
uint8_t DS18B20GetTemperature(uint8_t ch, float* tempValue, uint16_t delay)
{
    uint8_t ret = 0;
    uint8_t tempL = 0;
    uint8_t tempH = 0;
    uint16_t TemperateValue = 0;
    ret = DS18B20Reset(ch);    //初始化z
    if(ret)
    {
        return 1;
    }
    DS18B20WriteByte(ch, 0xCC); //忽略ROM指令，理由只有一个DS没必要对ROM进行操作
    DS18B20WriteByte(ch, 0x44); //温度转化指令 
    if(delay <= 0)
    {
        delay = 0;  //默认最小值为0
    }
    Delay_ms(delay);           //可变延时，方便初始化使用
    
    ret = DS18B20Reset(ch);    //初始化z
    if(ret)
    {
        return 1;
    }
    DS18B20WriteByte(ch, 0xCC); //跳过ROM指令
    DS18B20WriteByte(ch, 0xBE); //温度转化指令
    
    tempL = DS18B20ReadByte(ch);    //读取温度 第一个字节	这两个是连续的字节，且是bit0 与bit1,即它会在初始化前都是安顺序读取，
    tempH = DS18B20ReadByte(ch);    //读取第二个字节
    /*转换为16位*/
    TemperateValue = tempH;		   
    TemperateValue <<= 8;
    TemperateValue = TemperateValue | tempL;		 
    *tempValue = TemperateValue * 0.0625f;     //获取温度
    /*处理负温度*/
    tempH >>= 4;
    if(tempH == 0x0F)
    {
        *tempValue -= 4095.5;
    }
    return 0;
}
/**
 * @fn Init_DS18B20
 * @brief 初始化DS18B20
 * 
 */
uint8_t Init_DS18B20()
{
    uint8_t ret = 0;
    ret = DS18B20Reset(TEMP_0_INTERNAL);    //初始化z
    if(ret)
    {
        return ret;
    }
    ret = DS18B20Reset(TEMP_1_EXTERNAL);    //初始化z
    if(ret)
    {
        return ret;
    }
#ifdef USE_TEMP_2_EXTERNAL
    ret = DS18B20Reset(TEMP_2_EXTERNAL);    //初始化z
    if(ret)
    {
        return ret;
    }
#endif
    return ret;
}


