// ========================================================
/// @file       DK_SPI.c
/// @brief      Software simulation SPI performance function
/// @version    V1.0
/// @date       2013/04/25
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "DK_SPI.h"


/**********************************************************
**Name:     SPICmd8bit
**Function: SPI Write one byte
**Input:    WrPara
**Output:   none
**note:     use for burst mode
**********************************************************/
void SPICmd8bit(u8 WrPara)
{
  u8 bitcnt;  
  nCS=0;
  SCK=0;
  
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    if(WrPara&0x80)
      MOSI=1;
    else
      MOSI=0;
    SCK=1;
    WrPara <<= 1;
  }
  SCK=0;
  MOSI=1;
}

/**********************************************************
**Name:     SPIRead8bit
**Function: SPI Read one byte
**Input:    None
**Output:   result byte
**Note:     use for burst mode
**********************************************************/
u8 SPIRead8bit(void)
{
 u8 RdPara = 0;
 u8 bitcnt;
 
  nCS=0;
  MOSI=1;                                                 //Read one byte data from FIFO, MOSI hold to High
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    RdPara <<= 1;
    SCK=1;
    if(MISO)
      RdPara |= 0x01;
    else
      RdPara |= 0x00;
  }
  SCK=0;
  return(RdPara);
}

/**********************************************************
**Name:     SPIRead
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
u8 SPIRead(u8 adr)
{
  u8 tmp; 
  SPICmd8bit(adr);                                         //Send address first
  tmp = SPIRead8bit();  
  nCS=1;
  return(tmp);
}

/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void SPIWrite(word WrPara)                
{                                                       
  u8 bitcnt;    
  
  SCK=0;
  nCS=0;
  
  WrPara |= 0x8000;                                        //MSB must be "1" for write 
  
  for(bitcnt=16; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    if(WrPara&0x8000)
      MOSI=1;
    else
      MOSI=0;
    SCK=1;
    WrPara <<= 1;
  }
  SCK=0;
  MOSI=1;
  nCS=1;
}

/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SPIBurstRead(u8 adr, u8 *ptr, u8 length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    SCK=0; 
    nCS=0;
    SPICmd8bit(adr); 
    for(i=0;i<length;i++)
    ptr[i] = SPIRead8bit();
    nCS=1;  
  }
}

/**********************************************************
**Name:     SPIBurstWrite
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void BurstWrite(u8 adr, u8 *ptr, u8 length)
{ 
  u8 i;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {   
    SCK=0;
    nCS=0;        
    SPICmd8bit(adr|0x80);
    for(i=0;i<length;i++)
    SPICmd8bit(ptr[i]);
    nCS=1;  
  }
}





