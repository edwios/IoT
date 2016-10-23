#ifndef __DK_PIC16_Define_h__
#define __DK_PIC16_Define_h__

#include <pic.h>
#include <htc.h>
#include <stdio.h>




/**********************************************************
                   ---------------------
                  | VPP             RB7 | 
          nCS --- | RA0             RB6 | 
       RFData --- | RA1             RB5 | --- nIRQ0
         DIO0 --- | RA2             RB4 | --- nIRQ1 
         DIO1 --- | RA3             RB3 | --- KEY4
        CKIRQ --- | RA4             RB2 | --- KEY3
        CKDAT --- | RA5             RB1 | --- KEY2
        CKCLK --- | RE0             RB0 | --- KEY1
         CKWR --- | RE1             VDD |
        CKCSB --- | RE2             VSS |
                  | VDD             RD7 | --- LED7
                  | VSS             RD6 | --- LED6
                  | RA7             RD5 | --- LED5
                  | RA6             RD4 | --- LED4
                  | RC0             RC7 | --- RxD
                  | RC1             RC6 | --- TxD
       Buzzer --- | RC2             RC5 | --- MISO
          SCK --- | RC3             RC4 | --- MOSI
       PORchk --- | RD0             RD3 | --- LED3
         LED1 --- | RD1             RD2 | --- LED2
                   ---------------------
               RF_DK for PIC16LF1519 (ver1.0)
**********************************************************/
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long lword;

typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;
typedef signed char     s8;
typedef signed int      s16;
typedef signed long     s32;

/**********************************************************
IO port define
**********************************************************/
//                        //WPUx TRISx PORTx 
#define   nCS       RA0   //  X    0     1    RFMxx module CS pin
#define   RFData    RA1   //  X    0     0    RFMxx Data out
#define   DIO0      RA2   //  X    0     0    RFMxx DIO0  
#define   DIO1      RA3   //  X    0     0    RFMxx DIO1
#define   CKIRQ     RA4   //  X    1     0    LCD Driver(CK1622)
#define   CKDAT     RA5   //  X    0     0    LCD Driver(CK1622)
//#define           RA6   //  X    0     0
//#define           RA7   //  X    0     0

#define   KEY1      RB0   //  1    1     0    Down key
#define   KEY2      RB1   //  1    1     0    Left key
#define   KEY3      RB2   //  1    1     0    Up key
#define   KEY4      RB3   //  1    1     0    Right key
#define   nIRQ1     RB4   //  0    1     0    RFMxx nIRQ1
#define   nIRQ0     RB5   //  0    1     0    RFMxx nIRQ0
//#define           RB6   //  0    0     0  
//#define           RB7   //  0    0     0  

//#define           RC0   //  X    0     0  
//#define           RC1   //  X    0     0  
#define   Buzzer    RC2   //  X    0     0    Buzzer drvier
#define   SCK       RC3   //  X    0     0    RFMxx sck
#define   MOSI      RC4   //  X    0     1    RFMxx MOSI
#define   MISO      RC5   //  X    1     0    RFMxx MISO
#define   TxD       RC6   //  X    0     0    uart TxD 
#define   RxD       RC7   //  X    1     0    uart RxD

#define   PORchk    RD0   //  X    0     0    RFMxx power
#define   LED1      RD1   //  X    0     0    LED 
#define   LED2      RD2   //  X    0     0    LED
#define   LED3      RD3   //  X    0     0    LED
#define   LED4      RD4   //  X    0     0    LED
#define   LED5      RD5   //  X    0     0    LED
#define   LED6      RD6   //  X    0     0    LED
#define   LED7      RD7   //  X    0     0    LED

#define   CKCLK     RE0   //  0    0     0    LCD Driver(CK1622)
#define   CKWR      RE1   //  0    0     0    LCD Driver(CK1622)
#define   CKCSB     RE2   //  0    0     1    LCD Driver(CK1622)
  
/**********************************************************
PIC16F1519 IO initional(TRISx=1:input)(WPUx=1:pullup)
**********************************************************/ 
#define   PIC16_PORTA   0x01
#define   PIC16_LATA    0x01
#define   PIC16_TRISA   0x10
#define   PIC16_ANSELA  0x00

#define   PIC16_PORTB   0x00
#define   PIC16_LATB    0x00
#define   PIC16_TRISB   0x3F
#define   PIC16_WPUB    0x0F
#define   PIC16_ANSELB  0x00

#define   PIC16_PORTC   0x10
#define   PIC16_LATC    0x10
#define   PIC16_TRISC   0xA0
#define   PIC16_ANSELC  0x00

#define   PIC16_PORTD   0x00
#define   PIC16_LATD    0x00
#define   PIC16_TRISD   0x00
#define   PIC16_ANSELD  0x00

#define   PIC16_PORTE   0x04
#define   PIC16_LATE    0x04
#define   PIC16_TRISE   0x00
#define   PIC16_WPUE    0x00
#define   PIC16_ANSELE  0x00


#define   Input_RFData()      TRISA1=1          //RFData input
#define   Output_RFData()     TRISA1=0          //RFData output
#define   Input_DIO0()        TRISA2=1          //DIO0 input
#define   Output_DIO0()       TRISA2=0          //DIO0 output
#define   Input_DIO1()        TRISA3=1          //DIO1 input
#define   Output_DIO1()       TRISA3=0          //DIO1 output
#define   PullUp_nIRQ0()      WPUB5=1           //nIRQ0 pullup
#define   PullDown_nIRQ0()    WPUB5=0           //nIRQ0 pulldown


//LCD IO define
#define   CKCSB_L             TRISE2=0,CKCSB=0
#define   CKCSB_H             TRISE2=1
#define   CKWR_L              TRISE1=0,CKWR=0
#define   CKWR_H              TRISE1=1
#define   CKCLK_L             TRISE0=0,CKCLK=0
#define   CKCLK_H             TRISE0=1
#define   CKDAT_L             TRISA5=0,CKDAT=0
#define   CKDAT_H             TRISA5=1


#endif





