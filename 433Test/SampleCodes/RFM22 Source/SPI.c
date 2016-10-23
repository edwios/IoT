/**************************************************************
             SPI interface
***************************************************************/

#include "DK-rfm2xb.h"

/******************************************************************************
**函数名称：SPICmd8bit
**函数功能：SPI写入参数8bit
**输入参数：WrPara
**输出参数：无
**注意：    保留nCS输出Low
******************************************************************************/
void SPICmd8bit(byte WrPara)
{
    byte bitcnt;	
    nSEL_CMD = 0;
    MSCK = 0;

    for(bitcnt=8; bitcnt!=0; bitcnt--)
 	{
 	    MSCK = 0;
 	    if(WrPara&0x80)
 		    MOSI = 1;
 	    else
 		    MOSI = 0;
	    NOP();
 	    MSCK = 1;
	    NOP();
 	    WrPara <<= 1;
 	}
    MSCK = 0;
    MOSI = 1;
    //nSEL_CMD = 1;		//*此处不关闭nCS，使用连续模式*
}

/******************************************************************************
**函数名称：SPIRead8bitt
**函数功能：SPI读取参数8bit
**输入参数：读取8bit数据——RdPara
**输出参数：无
**注意：    保留nCS输出Low
******************************************************************************/
byte SPIRead8bit(void)
{
    byte RdPara = 0;
    byte bitcnt;	
    nSEL_CMD = 0;
    MOSI = 1;
 
    for(bitcnt=8; bitcnt!=0; bitcnt--)
 	{
        MSCK = 0;
        RdPara <<= 1;
        MSCK = 1;
        if(MISO)
            RdPara |= 0x01;
        else
            NOP(); 
        NOP();
 	}
    MSCK = 0;
    //nSEL_CMD = 1;		//*此处不关闭nCS，使用连续模式*
    return(RdPara);
}

/******************************************************************************
**函数名称：SPIRead
**函数功能：SPI读取一个地址数据
**输入参数：adr
**输出参数：无
******************************************************************************/
byte SPIRead(byte adr)
{
    byte tmp;	
    SPICmd8bit(adr);	//读状态标志
    tmp = SPIRead8bit();	
    nSEL_CMD = 1;
    return(tmp);
}

/******************************************************************************
**函数名称：SPIWrite
**函数功能：SPI写入一个16数据（高8位地址，低8位数据）
**输入参数：WrPara
**输出参数：无
******************************************************************************/
void SPIWrite(word WrPara)								
{                                                       
    byte bitcnt;    
    MSCK = 0;
    nSEL_CMD = 0;
 
    WrPara |= 0x8000;		//写数据高位置1
 
    for(bitcnt=16; bitcnt!=0; bitcnt--)
 	{
        MSCK = 0;
        if(WrPara&0x8000)
            MOSI = 1;
        else
            MOSI = 0;
        NOP();
        MSCK = 1;
        WrPara <<= 1;
 	}
    MSCK = 0;
    MOSI = 1;
    nSEL_CMD = 1;
}         

/******************************************************************************
**函数名称：SPIBurstRead
**函数功能：SPI连续读取模式
**输入参数：adr——读取地址
**          ptr——存储数据指针
**          length 读取长度
**输出参数：无，数据存在ptr中
******************************************************************************/
void SPIBurstRead(byte adr, byte *ptr, byte length)
{
    byte i;
    if(length<=1)			//读取长度必须大于1
        return;
    else
 	{
        MSCK = 0;                //注意SCK先清0，保持低
        nSEL_CMD = 0;
        SPICmd8bit(adr);		//读取地址
        for(i=0;i<length;i++)
            ptr[i] = SPIRead8bit();	
        nSEL_CMD = 1;
	}
}

/******************************************************************************
**函数名称：SPIBurstWrite
**函数功能：SPI连续写入模式
**输入参数：adr——写入地址
**          ptr——存储数据指针
**          length 写入长度
**输出参数：无
******************************************************************************/
void BurstWrite(byte adr, byte *ptr, byte length)
{ 
    byte i;
    if(length<=1)			//读取长度不为0或1
 	    return;
    else	
 	{ 		
        MSCK = 0;              //注意SCK先清0，保持低
        nSEL_CMD = 0;
        SPICmd8bit(adr|0x80);	//连续写
        for(i=0;i<length;i++)
            SPICmd8bit(ptr[i]);	
        nSEL_CMD = 1;
	}
}