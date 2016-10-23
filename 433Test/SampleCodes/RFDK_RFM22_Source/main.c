// ========================================================
/// @file       main.c
/// @brief      Main function of the RFDK_PIC16_LCD_ver1.1
/// @version    V2.1
/// @date       2013/12/25
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "DK_PIC16_App.h"

u16 gw_TimerCount=0;

void InitPort(void);
void InitSfr(void);
void timer1_init(void);
void timer2_init(void);
void Uart_init(void);
void interrupt ISR_timer(void);
void PowerUpDelay(void);
void PowerOnMusic(void);  


__CONFIG(0x3FFF29E4);
void main(void)
{
  u8 TxFlag=0;
  u8 RxFlag=0;

  InitSfr();                                               //PIC16 Register Initialization
  PowerUpDelay();                                          //Power on delay  
  InitPort();                                              //PIC16 IO port Initialization
  timer1_init();
  timer2_init();
  LCD_Init();                                              //LCD initialize
  BeepOff();                                               //Close buzzer
  ModuleSelectModeEntryCheck();                            //Confirm whether you can enter module select mode
  Uart_init();
  PowerOnMusic();                                          //Power on music
  
  while(1)
  { 
    MenuConfig();                                          //Menu config & display
    if(gb_ModuleWorkEnableFlag)
    {
      RFM22B_Running(gb_SystemMode,gb_ModuleWorkMode,gb_ParameterChangeFlag,&TxFlag,&RxFlag,&gb_RF_RSSI);
        
      if(TxFlag==1)                                        //Sent successfully
      {
        TxFlag=0; 
        gw_SendDataCount++;
        if(gw_SendDataCount>9999){gw_SendDataCount=0;}
      }
      if(RxFlag==1)                                        //Successfully received
      {
        RxFlag=0; 
        gw_ReceiveDataCount++;
        if(gw_ReceiveDataCount>9999){gw_ReceiveDataCount=0;}
      }
          
            
      if(gb_ParameterChangeFlag==1){gb_ParameterChangeFlag=0;}    //clear parameter flag
      if(gb_ModuleWorkMode!=C_ModuleWorkMode_FSK && gb_ModuleWorkMode!=C_ModuleWorkMode_OOK && gb_ModuleWorkMode!=C_ModuleWorkMode_LoRa)
      {
        gb_StatusTx=0;
        gb_StatusRx=0;
      }
    }
    else
    {
      gb_StatusTx=0;
      gb_StatusRx=0;
    }
    if(gb_ErrorFlag!=1)
    {  
      TxLED_Deal();                                        //Tx LED display deal
      RxLED_Deal();                                        //Rx LED display deal 
    }
  }
}

/**********************************************************
**Name:     InitPort
**Function: Initialize Port
**Input:    none
**Output:   none
**********************************************************/
void InitPort(void)
{ 
  PORTA = PIC16_PORTA;
  LATA = PIC16_LATA;
  TRISA = PIC16_TRISA;
  ANSELA= PIC16_ANSELA;
  
  PORTB = PIC16_PORTB;
  LATB = PIC16_LATB;
  TRISB = PIC16_TRISB;
  WPUB = PIC16_WPUB;
  ANSELB= PIC16_ANSELB;
  
  PORTC = PIC16_PORTC;
  LATC = PIC16_LATC;
  TRISC = PIC16_TRISC;
  ANSELC= PIC16_ANSELC;
  
  PORTD = PIC16_PORTD;
  LATD = PIC16_LATD;
  TRISD = PIC16_TRISD;
  ANSELD= PIC16_ANSELD;
  
  PORTE = PIC16_PORTE;
  LATE = PIC16_LATE;
  TRISE = PIC16_TRISE;
  WPUE = PIC16_WPUE;
  ANSELE= PIC16_ANSELE;
}

/**********************************************************
**Name:     InitSfr
**Function: Register Initialization
**Input:    none
**Output:   none
**********************************************************/
void InitSfr(void)
{
  OSCCON = 0x78;                                           //Internal crystal(16M)
  OPTION_REG = 0x06;
  WDTCON = 0x02;                                           //Close WDT, DivideRatio=1:128
  PCON = 0x00;            

  FVRCON = 0;                                              //Clear FVRCON     
  APFCON = 0;                                              //Clear APFRCON 
  BORCON = 0;                                              //Clear BORCON     
  ANSELA = 0;                                              //Clear PORTA AD channel
  ANSELB = 0;                                              //Clear PORTB AD channel
  ANSELC = 0;                                              //Clear PORTC AD channel
  ANSELD = 0;                                              //Clear PORTD AD channel
  ANSELE = 0;                                              //Clear PORTE AD channel

  INTCON = 0;                                              //Clear Interrupt
  PIR1 = 0;
  PIR2 = 0;
  ADCON0 = 0;
  
  di();                                                    //Display all interrupt
}

/**********************************************************
**Name:     timer1_init
**Function: timer1 Initialization
**Input:    none
**Output:   none
**********************************************************/
void timer1_init(void)
{
  INTCON = 0xc0;                                           //Enable interrupt
  T1CON = 0x4d;                                            //
  
  TMR1H = 0x00;                                            //Clear reload value
  TMR1L = 0x00;
  TMR1ON = 0;                                              //Close timer1
  TMR1IE = 1;                                              //Enable timer1 interrupt 
}

/**********************************************************
**Name:     timer2_init
**Function: timer2 Initialization
**Input:    none
**Output:   none
**********************************************************/
void timer2_init(void)
{
  INTCON  = 0xc0;                                          //Enable interrupt
  T2CON = 0x1d;                                            //Timer2 on, Prescaler:1:4, Postscale1:4
  PR2 = 250;                                               //1ms=16m/4 /4/4/1000(1KHZ)
  TMR2IE = 1;                                              //Enable timer1 interrupt 
}

/**********************************************************
**Name:     Uart_init
**Function: Uart Initialization
**Input:    none
**Output:   none
**********************************************************/
void Uart_init(void)
{
  TRISC7 = 1;
  TRISC6 = 1;
  SPBRGL = 0x19;     //16M crystal
  //SPBRGL = 0x20;   //20M crystal
  SPBRGH = 0x00;
  BAUDCON = 0x00;
  RCSTA = RCSTA|0x90;
  TXSTA = TXSTA|0x20;
  RCIE = 1;
  INTCON = 0xC0;
  TXIE=0; 
}

/**********************************************************
**Name:     ISR_timer
**Function: Timer interrupt subroutine
**Input:    none
**Output:   none
**********************************************************/
void interrupt ISR_timer(void)
{
  //Timer2 interrupt
  if(TMR2IF)
  {     
    //Time to 1ms
    if(gw_KeyDebounceTimer){gw_KeyDebounceTimer--;}        //Debounce timer for key
    if(gw_TxLedTimer){gw_TxLedTimer--;}                    //Tx LED timer
          
    gw_LCDFlashTimer++;                                    //LCD flash display timer
    if(gw_LCDFlashTimer>=300)
    {
      gw_LCDFlashTimer=0;
      gb_DispFlashEnableFlag^=1;
    }
          
    if(gw_TxTimer){gw_TxTimer--;}                          //Send timer
    
    gw_TimerCount++;
    if(gw_TimerCount>=1000)                                //time to 1S
    {
      gw_TimerCount=0;
      
      if(gb_SysTimer_1S){gb_SysTimer_1S--;}                //System timer for 1S          
    } 
    
    
    //Buzzer deal
    if(gw_BeepTimer){gw_BeepTimer--;}                      //Buzzer timer
    if((gb_BeepOnFlag==1)&&(gw_BeepTimer==0))
    {
      if(gw_ToneBuf[gb_BeepOnCount+1]!=0)
      {       
        gb_BeepTimerH = (gw_ToneBuf[gb_BeepOnCount]>>8) & 0xff;   
        gb_BeepTimerL = gw_ToneBuf[gb_BeepOnCount] & 0xff;
        gw_BeepTimer=gw_ToneBuf[gb_BeepOnCount+1];
        gw_ToneBuf[gb_BeepOnCount]=0;
        gw_ToneBuf[gb_BeepOnCount+1]=0;
        gb_BeepOnCount+=2;
      }
      else
      {
        BeepOff();                                         //Close buzzer
      }
    }
    if(gb_BeepOnFlag==0)                                   //Forced shutdown buzzer
    {
      TMR1ON = 0;
      Buzzer=0;
    } 
    TMR2IF = 0;
  } 
  
  //Timer1 interrupt
  if(TMR1IF)
  {
    Buzzer^=1;                                             //Buzzer out
    TMR1H = gb_BeepTimerH;                                 //reload value
    TMR1L = gb_BeepTimerL;  
    TMR1IF =0 ;     
  }
}

/**********************************************************
**Name:     PowerUpDelay
**Function: Power on delay  
**Input:    none
**Output:   none
**********************************************************/
void PowerUpDelay(void)
{
  Delay_ms(200);
}
