/****************************************************
          RFM-DK-rfm2xb  communicate with EVB
*****************************************************/
//*******************************************************************************
//  Software Description: 
//        Using RFM22b communicate with RF-EVB 
//  Software Function:  
//      Press the Key SW1,change the module in different mode:
//       1)first press SW1, config module in normal RX mode and RX led_on, 
//                if receive valid data and it is BeepOn; 
//       2)second press SW1,config module in normal TX mode, 
//                and it will transmit one package datas every 1S times and TX leds flashing;
//       3)third press SW1, config module in Test RX mode and RX led_on; 
//       4)fourth press SW1, config module in Test TX mode and all TX leds on;
//       5)fifth press SW1, config module in sleep mode, and all leds off, SW1 frame clear and loop from the step 1);
//  Hardware: 
//        Base on "HopeRF RF-DK for PIC16"
//  Description: 
/***********************************************************

                   ---------------------
         MCLR --- | VPP             RB7 | --- PGD
     nSEL_CMD --- | RA0             RB6 | --- PGC
       RFdata --- | RA1             RB5 | --- IRQ_0
        RxANT --- | RA2             RB4 | --- IRQ_1
        TxANT --- | RA3             RB3 | --- KEY4
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
          SDN --- | RD0             RD3 | --- D3
           D1 --- | RD1             RD2 | --- D2
                   ---------------------
                        PIC16LF1519

***********************************************************/
//
//  RF module:           RFM22b
//  Carry Frequency:     315MHz/433MHz/868MHz/915MHz
//  Frequency Deviation: +/-35KHz
//  Bit Rate:            2.4Kbps
//  Coding:              NRZ
//  Packet Format:       0x5555555555+0xAA2DD4+"HopeRF RFM COBRFM22bS" (total: 29 Bytes)
//  Tx Power Output:     about 20dBm (max.)
//  
//	File Description:
//         1) main.c--------------- Main program 
//         2) Keyscan.c------------ Key Press Scan
//         3) Beepled.c------------- Buzzer and leds On or Off
//         4) SPI.c---------------- SPI interface (Software simulate)
//         5) rfm2xbmodule.c-------------- RFM2xb config interface
//         6) RFM-mode.c---------- mode choose of the rfm2xb module
//	       7) DK-rfm2xb.h-------- Header file
//         8) Keyscan.h-------- Header file 
//         9) RFM-mode.h-------- Header file
//
//  Author:  Meng Fanbing
//  HOPE MICROELECTRONICS CO.,LTD. 
//  May 2013
//
//  Built with Hi-Tech PICC Version: 9.83 and MPLAB Version: 8.86
//******************************************************************************
#include "DK-rfm2xb.h"
#include "Keyscan.h"
#include "RFM-mode.h"


unsigned int systime;
unsigned int systime1;

byte 	FreqSel;			//Frequency choose
                            // 1 --> 315MHz ; 2 --> 433MHz 
                            // 3 --> 868MHz ; 4 --> 915MHz    
byte    TypeID;             //rfm22b --> 0x22

__CONFIG(FOSC_INTOSC & WDTE_OFF & MCLRE_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_ON);


main()
{
	Port_init();
	Config_init();		
	Beep_init();	
	key_init();
	timer0_init();
	RFM_modeclear();
	TypeID = 0x22;         //rfm22b module
	FreqSel = 2;           // choose the frequency band 433MHz

	while(1)
	{
		Keyscan();	
		RFM_mode();
		if((RFM_modeframe==1)&&(RFM_TXframe==0)&&(RFM_RXframe==1))
		{
            RFM2xB_NormalRX();
			for(systime=0;systime<300;);
		}
		else if((RFM_modeframe==2)&&(RFM_TXframe==1)&&(RFM_RXframe==0))
		{
            RLED1 = 0;
            RFM2xB_NormalTX();
			for(systime=0;systime<300;);
		}
		else if((RFM_modeframe==3)&&(RFM_TXframe==1)&&(RFM_RXframe==1))
		{
			led_alltx();
			RLED1 = 1;
			for(systime=0;systime<100;);
			if(1 == RFM2xB_TestRx())
			{
				while(1)
				{
					Keyscan();
                    for(systime=0;systime<200;);
					if(key_down)
						break;
				}
			}
			for(systime=0;systime<300;);
		}
		else if((RFM_modeframe==4)&&(RFM_TXframe==0)&&(RFM_RXframe==0))
		{
			led_alltx();
			for(systime=0;systime<100;);
			if(1 == RFM2xB_TestTx(0))
			{
				while(1)
				{
					Keyscan();
                    for(systime=0;systime<100;);
					if(key_down)
						break;
				}
			}
			for(systime=0;systime<300;);
		}
		else if(RFM_modeframe == 0)
		{
			led_off();
			for(systime=0;systime<100;);
            RFM2xB_Sleep();
			RFM_modeclear();
			while(1)
			{
				Keyscan();
				for(systime=0;systime<200;);
				if(key_down)
					break;
			}
			for(systime=0;systime<300;);
		}
	}
	for(systime=0;systime<200;);
}
void Port_init(void)
{
	OPTION_REG &= 0x7F;	

	ANSELA = 0;
	PORTA = 0;
	LATA = 0;

	ANSELB = 0x00;
	PORTB = 0;
	LATB = 0;
	WPUB = 0x0F;

	ANSELC = 0;
	PORTC = 0;
	LATC = 0;

	ANSELD = 0;
	PORTD = 0;
	LATD = 0;

	ANSELE = 0;
	PORTE = 0;
	LATE = 0;
	WPUE = 0;

	TRISA = 0x00;  		// RA0-5 output;
	TRISB = 0x3F;		// RB1-5 input;
	TRISC = 0x20;		// RC2-7 output;RC5 input
	TRISD = 0x00;       // RD1-7 output
	TRISE = 0x00;       // RE0-2 output
}
void Config_init(void)
{
	OSCCON = 0x78;		// 16M crystal;
	//OSCCON = 0x70  ;		// 8M crystal;
	WDTCON = 0;		// ;
 	INTCON = 0xC0;		//disable extern interrupt,rising interrupt;

}
void timer0_init()
{
	INTCON = 0xE0;
	OPTION_REG = 0b0011;	// prescale by 16
	TMR0CS = 0;			// select internal clock
	TMR0IE = 1;			// enable timer interrupt
	GIE = 1;			// enable global interrupts
	systime = 0;
	systime1 = 0;
}

void interrupt ISR()
{

	if(TMR0IF)
	{
		systime++;
		systime1++;
		TMR0IF = 0;
	}		
}
	
