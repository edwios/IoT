#ifndef __DK_LCD_h__
#define __DK_LCD_h__

#include "DK_PIC16_App.h"


#define CK1622_CMD_DISSYS         0x00                     //Turn off the system oscillator
#define CK1622_CMD_ENSYS          0x01                     //Turn on the system oscillator
#define CK1622_CMD_LCDOFF         0x02                     //Turn off the LCD bias generator
#define CK1622_CMD_LCDON          0x03                     //Turn on the LCD bias generator
#define CK1622_CMD_DISTMR         0x04                     //Disable time base output
#define CK1622_CMD_DISWDT         0x05                     //Disalbe WDT time-out flag output
#define CK1622_CMD_ENTMR          0x06                     //Enable time base output
#define CK1622_CMD_ENWDT          0x07                     //Enable WDT time-out flag output
#define CK1622_CMD_BZOFF          0x08                     //Buzzer off
#define CK1622_CMD_CLRTMR         0x0D                     //Clear time-out
#define CK1622_CMD_CLRWDT         0x0F                     //Clear WDT
#define CK1622_CMD_RC32K          0x18                     //The system clock source = internal RC clock
#define CK1622_CMD_EXT32K         0x1C                     //The system clock source = external clock source
#define CK1622_CMD_BZ4K           0x40                     //Fbuzzer = 4kHz
#define CK1622_CMD_BZ2K           0x60                     //Fbuzzer = 2kHz
#define CK1622_CMD_DISIRQ         0x80                     //Disable IRQ
#define CK1622_CMD_ENIRQ          0x88                     //Enable IRQ
#define CK1622_CMD_F1             0xA0                     //1Hz,4s
#define CK1622_CMD_F2             0xA1                     //2Hz,2s
#define CK1622_CMD_F4             0xA2                     //4Hz,1s
#define CK1622_CMD_F8             0xA3                     //8Hz,1/2s
#define CK1622_CMD_F16            0xA4                     //16Hz,1/4s
#define CK1622_CMD_F32            0xA5                     //32Hz,1/8s
#define CK1622_CMD_F64            0xA6                     //64Hz,1/16s
#define CK1622_CMD_F128           0xA7                     //128Hz,1/32s
#define CK1622_CMD_TEST           0xE0                     //Test mode
#define CK1622_CMD_NORMAL         0xE3                     //Normal mode



const u8 LCDDigTable[38] = 
{ 
    0x77,0x24,0x5d,0x6d,0x2e,0x6b,0x7b,0x25,0x7f,0x6f,
//  '0'  '1'  '2'  '3'  '4'  '5'  '6'  '7'  '8'  '9'
    0x3f,0x7a,0x53,0x7c,0x5b,0x1b,0x6f,0x3e,0x24,0x64,
//  'A'  'B'  'C'  'D'  'E'  'F'  'G'  'H'  'I'  'J' 
    0x1a,0x52,0x37,0x38,0x78,0x1f,0x2f,0x18,0x6b,0x5a,
//  'K'  'L'  'M'  'N'  'O'  'P'  'Q'  'R'  'S'  'T'
    0x70,0x76,0x00,0x00,0x00,0x00,0x00,0x08,
//  'U'  'V'  'W'  'X'  'Y'  'Z'  ' '  '-'
};

//Display address define
#define C_DispAddr_Time           25  
#define C_DispAddr_Tx             29
#define C_DispAddr_Rx             0
#define C_DispAddr_RFM            18
#define C_DispAddr_PWR            14
#define C_DispAddr_Fdev           5
#define C_DispAddr_BW             8
#define C_DispAddr_BR             12

//Flash display flag define
#define C_FlashDisp_None          0
#define C_FlashDisp_Freq          1
#define C_FlashDisp_Time          2
#define C_FlashDisp_Tx            3
#define C_FlashDisp_Rx            4
#define C_FlashDisp_RFM           5
#define C_FlashDisp_PWR           6
#define C_FlashDisp_BR            7
#define C_FlashDisp_Fdev          8
#define C_FlashDisp_BW            9

typedef union
{
  struct
  {
    u8 Bit0: 1;
    u8 Bit1: 1;
    u8 Bit2: 1;
    u8 Bit3: 1;
    u8 Bit4: 1;
    u8 Bit5: 1;
    u8 Bit6: 1;
    u8 Bit7: 1;
    u8 Bit8: 1;
    u8 Bit9: 1;
    u8 Bit10: 1;
    u8 Bit11: 1;
    u8 Bit12: 1;
    u8 Bit13: 1;
    u8 Bit14: 1;
    u8 Bit15: 1;
    u8 Bit16: 1;
    u8 Bit17: 1;
    u8 Bit18: 1;
    u8 Bit19: 1;
    u8 Bit20: 1;
    u8 Bit21: 1;
    u8 Bit22: 1;
    u8 Bit23: 1;
    u8 Bit24: 1;
    u8 Bit25: 1;
    u8 Bit26: 1;
    u8 Bit27: 1;
    u8 Bit28: 1;
    u8 Bit29: 1;
    u8 Bit30: 1;
    u8 Bit31: 1;
  }u32Bits;
  u32 u32Word;
}FlagSTR; 

extern FlagSTR _DispSeg;
  #define DispSeg_All      _DispSeg.u32Word
  #define DispSeg_Q5       _DispSeg.u32Bits.Bit0
  #define DispSeg_Q4       _DispSeg.u32Bits.Bit1
  #define DispSeg_Q3       _DispSeg.u32Bits.Bit2
  #define DispSeg_Q2       _DispSeg.u32Bits.Bit3
  #define DispSeg_Q1       _DispSeg.u32Bits.Bit4
  #define DispSeg_315      _DispSeg.u32Bits.Bit5
  #define DispSeg_434      _DispSeg.u32Bits.Bit6
  #define DispSeg_868      _DispSeg.u32Bits.Bit7
  #define DispSeg_915      _DispSeg.u32Bits.Bit8
  #define DispSeg_RFM      _DispSeg.u32Bits.Bit9
  #define DispSeg_HopeRF   _DispSeg.u32Bits.Bit10
  #define DispSeg_P1       _DispSeg.u32Bits.Bit11
  #define DispSeg_P2       _DispSeg.u32Bits.Bit12
  #define DispSeg_P3       _DispSeg.u32Bits.Bit13
  #define DispSeg_MHz      _DispSeg.u32Bits.Bit14
  #define DispSeg_PWR      _DispSeg.u32Bits.Bit15
  #define DispSeg_BR       _DispSeg.u32Bits.Bit16
  #define DispSeg_Fdev     _DispSeg.u32Bits.Bit17
  #define DispSeg_BW       _DispSeg.u32Bits.Bit18
  #define DispSeg_Tx       _DispSeg.u32Bits.Bit19
  #define DispSeg_Rx       _DispSeg.u32Bits.Bit20
  #define DispSeg_P4       _DispSeg.u32Bits.Bit21
  #define DispSeg_P5       _DispSeg.u32Bits.Bit22
  #define DispSeg_P6       _DispSeg.u32Bits.Bit23
  #define DispSeg_P7       _DispSeg.u32Bits.Bit24
  #define DispSeg_P8       _DispSeg.u32Bits.Bit25
  #define DispSeg_MV       _DispSeg.u32Bits.Bit26

extern u8 gb_DispFlashEnableFlag;                          //DispSeg flash enable flag
extern u8 gb_DispFlashFlag;                                //DispSeg flash flag
extern u8 DispBuf[32];                                     //LCD display cache

void LCD_Init(void);
void SendStringToDispBuf(u8 addr, u8 *datBuf);             //Send string to display buffer
void SendNumberToDispBuf(u8 addr, u16 dat);                //Send number to display buffer
void SendSegDataToDispBuf(void);                           //Send special seg to display buffer
void LCD_Display(void);
void LCD_DisplayAll(void);
void Clr_LCD(void);                                        //Clear LCD display

#endif







