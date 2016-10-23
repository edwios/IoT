/***********************************************************
         DK-1519-rfm2xb module
************************************************************/
/**********************************************************
Total:            RFM-DK for 1519
Version:          V2.0
Prosessor:        PIC16F1519
***********************************************************/
/***********************************************************

                   ---------------------
         MCLR --- | VPP             RB7 | --- PGD
     nSEL_CMD --- | RA0             RB6 | --- PGC
       RFdata --- | RA1             RB5 | --- IRQ_0
     PLL_LOCK --- | RA2             RB4 | --- IRQ_1
     nSEL_DAT --- | RA3             RB3 | --- KEY4
          MMR --- | RA4             RB2 | --- KEY3
        MDSCK --- | RA5             RB1 | --- KEY2
       MSHOLD --- | RE0             RB0 | --- KEY1
        MDSEL --- | RE1             VDD |
        MDSDI --- | RE2             VSS |
                  | VDD             RD7 | --- D7
                  | VSS             RD6 | --- D6
                  | RA7             RD5 | --- D5
     		  --- | RA6             RD4 | --- D4
                  | RC0             RC7 | --- RXD
                  | RC1             RC6 | --- TXD
      	 Buzz --- | RC2             RC5 | --- MISO
      	  MSCK--- | RC3             RC4 | --- MOSI
          POR --- | RD0             RD3 | --- D3
           D1 --- | RD1             RD2 | --- D2
                   ---------------------
                        PIC16LF1519

***********************************************************/
#include<pic.h>
#include  <stdbool.h>

typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long lword;

typedef union 					//Frame bits
{
 	struct 
 	{
 		byte  _FlagBit0:1;			//bit0
 		byte  _FlagBit1:1;	 		//bit1
 		byte  _FlagBit2:1;			//bit2
 		byte  _FlagBit3:1;			//bit3
 		byte  _FlagBit4:1;			//bit4
 		byte  _FlagBit5:1;			//bit5
 		byte  _FlagBit6:1;			//bit6
 		byte  _FlagBit7:1;			//bit7
 	}BBITS;
 	byte BBYTE;
}FlagSTR;

#define  RFM42B  0x42
#define  RFM43B  0x43
#define  RFM31B  0x31
#define  RFM23B  0x23
#define  RFM22B  0x22

#define  RxANT   RA2
#define  TxANT   RA3
#define  SDN     RD0
#define  nSEL_CMD  RA0
#define  RFdata    RA1

//#define  MMR       RA4
//#define  MDSCK     RA5

//#define  MSHOLD    RE0
//#define  MDSEL     RE1
//#define  MDSDI     RE2

#define  KEY1      RB0
#define  KEY2      RB1
#define  KEY3      RB2
#define  KEY4      RB3
#define  IRQ_1     RB4
#define  IRQ_0     RB5

#define  Buzz      RC2
#define  MSCK      RC3
#define  MOSI      RC4
#define  MISO      RC5
#define  TXD       RC6
#define  RXD       RC7

#define  TLED1     RD1
#define  TLED2     RD2
#define  TLED3     RD3
#define  TLED4     RD4
#define  TLED5     RD5
#define  TLED6     RD6
#define  RLED1     RD7


extern unsigned int systime;
extern unsigned int systime1;

extern byte FreqSel;		//Frequency choose
extern byte TypeID;         //module type 


extern void Port_init();
extern void Config_init(void);
extern void timer0_init();

extern byte RFM2xB_EntryRx(void);
extern byte RFM2xB_EntryTx(void);
extern void RFM2xB_ClearFIFO(void);
extern void RFM2xB_Sleep(void);
extern void RFM2xB_Standby(void);
extern void RFM2xB_Config(void);
extern byte RFM2xB_RxChk(void);
extern byte RFM2xB_RxPacket(void);
extern void RFM2xB_TxPacket(void);
extern byte RFM2xB_TestRx(void);
extern byte RFM2xB_TestTx(byte unmodulator);
extern void RFM2xB_TestConfig(void);
extern void RFM2xB_NormalTX(void);
extern void RFM2xB_NormalRX(void);
extern void SPICmd8bit(byte WrPara);
extern byte SPIRead8bit(void);
extern byte SPIRead(byte adr);
extern void SPIWrite(word WrPara);
extern void SPIBurstRead(byte adr, byte *ptr, byte length);
extern void BurstWrite(byte adr, byte *ptr, byte length);


//extern void Delay_mS(unsigned int x);
extern void Beep_init(void);
extern void BeepOn(void);
extern void BeepOff(void);
extern void led_tx();
extern void led_rx();
extern void led_alltx();
extern void led_off();