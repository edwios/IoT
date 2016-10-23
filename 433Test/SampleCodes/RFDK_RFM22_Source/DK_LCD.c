// ========================================================
/// @file       DK_LCD.c
/// @brief      CK1622 display driver
/// @version    V1.0
/// @date       2013/06/17
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "DK_LCD.h"


FlagSTR _DispSeg;                                          //LCD seg display bit
u8 gb_DispFlashEnableFlag=0;                               //DispSeg flash enable flag
u8 gb_DispFlashFlag=C_FlashDisp_None;
u8 gb_DispFlashFlagBackup=C_FlashDisp_None;
u8 DispBuf[32];                                            //LCD display cache
u8 DispBufCache[32];                                       //LCD display cache memory backup

/**********************************************************
**Name:     CK1622_WriteByte
**Function: Write one byte data to CK1622
**Input:    addr, seg address
            dat, data
**Output:   none
**********************************************************/
void CK1622_WriteByte(u8 addr,u8 dat)
{
  u8 i; 
  u16 temp=addr;
  CKCSB_L;
  CKWR_H;
  temp|=0x140;                                             //Write mode
  
  for(i=9; i!=0; i--)
  {
    CKCLK_L;
    if(temp&0x100)
      CKDAT_H;
    else
      CKDAT_L;
    CKCLK_H;
    temp <<= 1;
  } 
  for(i=8; i!=0; i--)                                      //seg
  {
    CKCLK_L;
    if(dat&0x01)
      CKDAT_H;
    else
      CKDAT_L;
    CKCLK_H;
    dat >>= 1;
  }
  CKCLK_L;
  CKDAT_H;
  CKCSB_H;
}

/**********************************************************
**Name:     CK1622_WriteBuf
**Function: Write some byte data to CK1622
**Input:    addr, seg address
            *dat, data buf
            length, buf length
**Output:   none
**********************************************************/
void CK1622_WriteBuf(u8 addr,u8 *dat,u8 length)
{
  u8 i,j; 
  u16 temp=addr;
  CKCSB_L;
  CKWR_H;
  temp|=0x140;                                             //Write mode
  
  for(i=9; i!=0; i--)
  {
    CKCLK_L;
    if(temp&0x100)
      CKDAT_H;
    else
      CKDAT_L;
    CKCLK_H;
    temp <<= 1;
  }
  for(j=0;j<length;j++)
  { 
    temp=dat[j];
    for(i=4; i!=0; i--)
    {
      CKCLK_L;
      if(temp&0x08)
        CKDAT_H;
      else
        CKDAT_L;
      CKCLK_H;
      temp <<= 1;
    }
  }
  CKCLK_L;
  CKDAT_H;
  CKCSB_H;
}

/**********************************************************
**Name:     CK1622_WriteCmd
**Function: Write command to CK1622
**Input:    cmd, command
**Output:   none
**********************************************************/
void CK1622_WriteCmd(u8 cmd)
{
  u8 i;
  u16 temp=cmd; 
  CKCSB_L;
  CKWR_H;
  temp|=0x400;                                             //Command mode
  temp<<=1;
  
  for(i=12; i!=0; i--)
  {
    CKCLK_L;
    if(temp&0x800)
      CKDAT_H;
    else
      CKDAT_L;
    CKCLK_H;
    temp <<= 1;
  }
  CKCLK_L;
  CKDAT_H;
  CKCSB_H;
}

/**********************************************************
**Name:     Clr_LCD
**Function: Clear LCD display
**Input:    none
**Output:   none
**********************************************************/
void Clr_LCD(void)
{
  u8 i,j=0;
  for(i=0;i<64;i+=2)
  {
    CK1622_WriteByte(i,0x00);
    DispBuf[j++]=LCDDigTable[36];
  }
}

/**********************************************************
**Name:     LCD_Init
**Function: Initial CK1622
**Input:    none
**Output:   none
**********************************************************/
void LCD_Init(void)
{ 
  CK1622_WriteCmd(CK1622_CMD_ENSYS);
  CK1622_WriteCmd(CK1622_CMD_LCDON);
  CK1622_WriteCmd(CK1622_CMD_NORMAL);
  CK1622_WriteCmd(CK1622_CMD_DISWDT);
  CK1622_WriteCmd(CK1622_CMD_RC32K);
  CK1622_WriteCmd(CK1622_CMD_DISIRQ);
  CK1622_WriteCmd(CK1622_CMD_BZOFF);
  CK1622_WriteCmd(CK1622_CMD_CLRTMR);
  CK1622_WriteCmd(CK1622_CMD_ENTMR);
  CK1622_WriteCmd(CK1622_CMD_F1);
  
  Clr_LCD();                                               //Clear LCD display  
}

/**********************************************************
**Name:     AsciiToDisplay
**Function: ASCII letter conver to display data
**Input:    Letter, ASCII char
**Output:   DispData, Display char
**********************************************************/
u8 AsciiToDisplay(u8 Letter)              
{
  u8 DispData;
  if(Letter>=0x30 && Letter<=0x39){DispData=LCDDigTable[Letter-0x30];}    //'0'-'9'
  if(Letter>=0x41 && Letter<=0x5A){DispData=LCDDigTable[Letter-0x41+10];} //'A'-'Z'
  if(Letter>=0x61 && Letter<=0x7A){DispData=LCDDigTable[Letter-0x61+10];} //'a'-'z'
  if(Letter==0x20){DispData=LCDDigTable[36];}                             //' '
  if(Letter==0x2D){DispData=LCDDigTable[37];}                             //'-'
  
  return DispData;
}

/**********************************************************
**Name:     SendStringToDispBuf
**Function: Send string to display buffer
**Input:    addr, display position
            *datBuf, String to be displayed
**Output:   none
**********************************************************/
void SendStringToDispBuf(u8 addr, u8 *datBuf)
{
  u8 i,j=0; 
  u8 length=0;
  
  switch(addr)
  {         
    case C_DispAddr_Time:
      length=6;   
      break;    
    case C_DispAddr_Tx:
      length=4;     
      break;
    case C_DispAddr_Rx:
      length=4;
      break;
    case C_DispAddr_RFM:
      length=4;
      break;
    case C_DispAddr_PWR:
      length=2;
      break;
    case C_DispAddr_BR:
      length=3;
      break;
    case C_DispAddr_Fdev:
      length=3;
      break;
    case C_DispAddr_BW:
      length=3;
      break;
  }
  if(addr==C_DispAddr_BR)
  {
    DispBuf[addr]=AsciiToDisplay(datBuf[0]);
    DispBuf[addr-1]=AsciiToDisplay(datBuf[1]);
    DispBuf[4]=AsciiToDisplay(datBuf[2]);   
  }
  else if(addr==C_DispAddr_Tx || addr==C_DispAddr_Time || addr==C_DispAddr_RFM || addr==C_DispAddr_PWR)
  {
    for(i=addr;i>addr-length;i--){DispBuf[i]=AsciiToDisplay(datBuf[j++]);}
  }
  else if(length!=0)
  {
    for(i=addr;i<addr+length;i++){DispBuf[i]=AsciiToDisplay(datBuf[j++]);}
  }
}

/**********************************************************
**Name:     SendNumberToDispBuf
**Function: Send number to display buffer
**Input:    addr, display position
            dat, Data to be displayed
**Output:   none
**********************************************************/
void SendNumberToDispBuf(u8 addr, u16 dat)
{
  u8 length=0;
  u8 temp=0;  
  
  switch(addr)
  {
//    case C_DispAddr_Time:
//      length=4;   
//      break;    
    case C_DispAddr_Tx:
      length=4;   
      break;
    case C_DispAddr_Rx:
      length=4;
      break;
    case C_DispAddr_RFM:
      length=4;
      break;
    case C_DispAddr_PWR:
      length=2;
      break;
    case C_DispAddr_BR:
      length=3;
      break;
    case C_DispAddr_Fdev:
      length=3;
      break;
    case C_DispAddr_BW:
      length=3;
      break;
  } 
  if(length==4 && addr==C_DispAddr_Rx)
  {   
    temp=(u8)(dat%10);     DispBuf[addr+3]=LCDDigTable[temp];
    temp=(u8)(dat/10%10);  DispBuf[addr+2]=LCDDigTable[temp];
    temp=(u8)(dat/100%10); DispBuf[addr+1]=LCDDigTable[temp];
    temp=(u8)(dat/1000);   DispBuf[addr]=LCDDigTable[temp];       
    //Vanishing zero befor number
    if(dat<1000){DispBuf[addr]=AsciiToDisplay(' ');}
    if(dat<100){DispBuf[addr+1]=AsciiToDisplay(' ');}
    if(dat<10){DispBuf[addr+2]=AsciiToDisplay(' ');}    
  }
  else if(length==4)
  {   
    temp=(u8)(dat%10);     DispBuf[addr-3]=LCDDigTable[temp];
    temp=(u8)(dat/10%10);  DispBuf[addr-2]=LCDDigTable[temp];
    temp=(u8)(dat/100%10); DispBuf[addr-1]=LCDDigTable[temp];
    temp=(u8)(dat/1000);   DispBuf[addr]=LCDDigTable[temp];       
    //Vanishing zero befor number
    if(dat<1000){DispBuf[addr]=AsciiToDisplay(' ');}
    if(dat<100){DispBuf[addr-1]=AsciiToDisplay(' ');}
    if(dat<10){DispBuf[addr-2]=AsciiToDisplay(' ');}  
  }
  else if(length==3 && addr==C_DispAddr_BR)
  {   
    temp=(u8)(dat%10);     DispBuf[4]=LCDDigTable[temp];
    temp=(u8)(dat/10%10);  DispBuf[addr-1]=LCDDigTable[temp];
    temp=(u8)(dat/100);    DispBuf[addr]=LCDDigTable[temp];
    
    //Vanishing zero befor number
    if(dat<100){DispBuf[addr]=AsciiToDisplay(' ');}
    if(dat<10){DispBuf[addr-1]=AsciiToDisplay(' ');}    
  }
  else if(length==3 && addr!=C_DispAddr_BR)
  {   
    temp=(u8)(dat%10);     DispBuf[addr+2]=LCDDigTable[temp];
    temp=(u8)(dat/10%10);  DispBuf[addr+1]=LCDDigTable[temp];
    temp=(u8)(dat/100);    DispBuf[addr]=LCDDigTable[temp];
    
    //Vanishing zero befor number
    if(dat<100){DispBuf[addr]=AsciiToDisplay(' ');}
    if(dat<10){DispBuf[addr+1]=AsciiToDisplay(' ');}    
  }
  else if(length==2)
  {   
    temp=(u8)(dat%10);     DispBuf[addr-1]=LCDDigTable[temp];
    temp=(u8)(dat/10);     DispBuf[addr]=LCDDigTable[temp];
    
    //Vanishing zero befor number1
    if(dat<10){DispBuf[addr]=AsciiToDisplay(' ');}    
  } 
}

/**********************************************************
**Name:     SendSegDataToDispBuf
**Function: Send special symbols to display buffer
**Input:    none
**Output:   none
**********************************************************/
void SendSegDataToDispBuf(void)
{
  if(DispSeg_Q5){DispBuf[0]|=0x80;}
  if(DispSeg_Q4){DispBuf[29]|=0x80;}
  if(DispSeg_Q3){DispBuf[28]|=0x80;}
  if(DispSeg_Q2){DispBuf[27]|=0x80;}
  if(DispSeg_Q1){DispBuf[26]|=0x80;}
  if(DispSeg_315){DispBuf[25]|=0x80;}
  if(DispSeg_434){DispBuf[24]|=0x80;}
  if(DispSeg_868){DispBuf[20]|=0x80;}
  if(DispSeg_915){DispBuf[19]|=0x80;} else{DispBuf[19]&=0x7F;}
  if(DispSeg_RFM){DispBuf[19]|=0x01;} else{DispBuf[19]&=0xFE;}
  if(DispSeg_HopeRF){DispBuf[16]|=0x80;}
  if(DispSeg_P1){DispBuf[23]|=0x80;}
  if(DispSeg_P2){DispBuf[22]|=0x80;}
  if(DispSeg_P3){DispBuf[21]|=0x80;}
  if(DispSeg_MHz){DispBuf[19]|=0x02;} else{DispBuf[19]&=0xFD;}
  if(DispSeg_PWR){DispBuf[13]|=0x80;}
  if(DispSeg_BR){DispBuf[4]|=0x80;}
  if(DispSeg_Fdev){DispBuf[7]|=0x80;}
  if(DispSeg_BW){DispBuf[10]|=0x80;}
  if(DispSeg_Tx){DispBuf[12]|=0x80;}
  if(DispSeg_Rx){DispBuf[1]|=0x80;}
  if(DispSeg_P4){DispBuf[18]|=0x80;}
  if(DispSeg_P5){DispBuf[2]|=0x80;}
  if(DispSeg_P6){DispBuf[11]|=0x80;}
  if(DispSeg_P7){DispBuf[6]|=0x80;}
  if(DispSeg_P8){DispBuf[9]|=0x80;}
  if(DispSeg_MV){DispBuf[15]|=0x80;}
}

/**********************************************************
**Name:     FlashDisplay
**Function: Flashing display the specified location
**Input:    none
**Output:   none
**********************************************************/
void FlashDisplay(void)
{
  u8 i;
  
  if(gb_DispFlashFlagBackup!=gb_DispFlashFlag)
  {
    for(i=0;i<32;i++){DispBufCache[i]=DispBuf[i];}         //Don't flash of LCD when display flag change
  }
  switch(gb_DispFlashFlag)
  {
    case C_FlashDisp_None:
      break;
    case C_FlashDisp_Freq:
      if(gb_DispFlashEnableFlag)
      {
        if(DispSeg_315){DispBufCache[25]=DispBuf[25]&0x7F;}
        else if(DispSeg_434){DispBufCache[24]=DispBuf[24]&0x7F;}
        else if(DispSeg_868){DispBufCache[20]=DispBuf[20]&0x7F;}
        else if(DispSeg_915){DispBufCache[19]=DispBuf[19]&0x7F;}
      }
      else
      {
        if(DispSeg_315){DispBufCache[25]=DispBuf[25];}
        else if(DispSeg_434){DispBufCache[24]=DispBuf[24];}
        else if(DispSeg_868){DispBufCache[20]=DispBuf[20];}
        else if(DispSeg_915){DispBufCache[19]=DispBuf[19];}       
      }
      break;
    case C_FlashDisp_Time:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_Time;i>C_DispAddr_Time-6;i--){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
      }
      else
      {
        for(i=C_DispAddr_Time;i>C_DispAddr_Time-6;i--){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_Tx:
      if(gb_DispFlashEnableFlag!=0)
      {       
        for(i=C_DispAddr_Tx;i>C_DispAddr_Tx-4;i--){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
      }
      else
      {
        for(i=C_DispAddr_Tx;i>C_DispAddr_Tx-4;i--){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_Rx:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_Rx;i<C_DispAddr_Rx+4;i++){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
      }
      else
      {
        for(i=C_DispAddr_Rx;i<C_DispAddr_Rx+4;i++){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_RFM:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_RFM;i>C_DispAddr_RFM-4;i--){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
      }
      else
      {
        for(i=C_DispAddr_RFM;i>C_DispAddr_RFM-4;i--){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_PWR:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_PWR;i>C_DispAddr_PWR-2;i--){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
      }
      else
      {
        for(i=C_DispAddr_PWR;i>C_DispAddr_PWR-2;i--){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_BR:
      if(gb_DispFlashEnableFlag)
      { 
        DispBufCache[C_DispAddr_BR]=(DispBuf[C_DispAddr_BR]&0x80)|LCDDigTable[36];
        DispBufCache[C_DispAddr_BR-1]=(DispBuf[C_DispAddr_BR-1]&0x00)|LCDDigTable[36];
        DispBufCache[4]=(DispBuf[4]&0x80)|LCDDigTable[36];  
      }
      else
      {
        DispBufCache[C_DispAddr_BR]=DispBuf[C_DispAddr_BR];
        DispBufCache[C_DispAddr_BR-1]=DispBuf[C_DispAddr_BR-1];
        DispBufCache[4]=DispBuf[4];   
      }
      break;
    case C_FlashDisp_Fdev:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_Fdev;i<C_DispAddr_Fdev+3;i++){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
        DispBufCache[6]=DispBufCache[6]&0x7F;              //P7
      }
      else
      {
        for(i=C_DispAddr_Fdev;i<C_DispAddr_Fdev+3;i++){DispBufCache[i]=DispBuf[i];}       
      }
      break;
    case C_FlashDisp_BW:
      if(gb_DispFlashEnableFlag)
      {       
        for(i=C_DispAddr_BW;i<C_DispAddr_BW+3;i++){DispBufCache[i]=(DispBuf[i]&0x80)|LCDDigTable[36];}  
        DispBufCache[9]=DispBufCache[9]&0x7F;              //P8
      }
      else
      {
        for(i=C_DispAddr_BW;i<C_DispAddr_BW+3;i++){DispBufCache[i]=DispBuf[i];}       
      }
      break;
  }
  gb_DispFlashFlagBackup=gb_DispFlashFlag;
}

/**********************************************************
**Name:     LCD_Display
**Function: Send display buffer to CK1622
**Input:    none
**Output:   none
**********************************************************/
void LCD_Display(void)
{
  u8 i=0; 
  SendSegDataToDispBuf();
  for(i=0;i<32;i++){DispBufCache[i]=DispBuf[i];}
  FlashDisplay();                                          //Flash display LCD
  
  for(i=0;i<60;i+=2)
  {
    CK1622_WriteByte(i,DispBufCache[i/2]);
  }
}

/**********************************************************
**Name:     LCD_DisplayAll
**Function: Dispaly all LCD
**Input:    none
**Output:   none
**********************************************************/
void LCD_DisplayAll(void)
{
  u8 i;
  for(i=0;i<32;i++)                                        //display all
    DispBuf[i]=0xff;
}

