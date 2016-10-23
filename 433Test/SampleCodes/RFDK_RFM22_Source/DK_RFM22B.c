// ========================================================
/// @file       DK_RFM22B.c
/// @brief      RFM22B basic application
/// @version    V2.1
/// @date       2013/12/25
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "DK_RFM.h"

/************************Description************************
                      ________________
                     /                \
                    /      HOPERF      \
                   /____________________\
                  |      DK_RFM22B       |
    (MCU)DIO1 --- | GPIO0(TxANT)     GND | --- GND
    (MCU)DIO0 --- | GPIO2(RxANT)      A7 | --- 
  (MCU)RFData --- | GPIO1             A6 | ---  
   (MCU)nIRQ1 --- |                   A5 | --- 
   (MCU)nIRQ0 --- | nIRQ              A4 | --- 
    (MCU)MISO --- | SDO               A3 | --- 
    (MCU)MOSI --- | SDI               A2 | --- 
     (MCU)SCK --- | SCK               A1 | --- 
     (MCU)nCS --- | nSEL              A0 | --- 
  (MCU)PORChk --- | SDN              VCC | --- VCC             
                  |______________________|
                  
               

//  RF module:           RFM22B
//  Carry Frequency:     315MHz/434MHz/868MHz/915MHz
//  Bit Rate:            1.2Kbps/2.4Kbps/4.8Kbps/9.6Kbps
//  Tx Power Output:     20dbm/17dbm/14dbm/11dbm
//  Frequency Deviation: +/-35KHz
//  Receive Bandwidth:   80KHz
//  Coding:              NRZ
//  Packet Format:       0x5555555555+0xAA2DD4+"HopeRF RFM COBRFM22BS" (total: 29 Bytes)
//  Tx Current:          about 85mA  (RFOP=+20dBm,typ.)
//  Rx Current:          about 18.5mA  (typ.)                 
**********************************************************/

/**********************************************************
**Parameter table define
**********************************************************/
const u16 RFM22BFreqTbl[4][3] = 
{ 
  {0x7547, 0x767D, 0x7700}, //315MHz
  {0x7553, 0x7664, 0x7700}, //434MHz
  {0x7573, 0x7664, 0x7700}, //868MHz
  {0x7575, 0x76BB, 0x7780}, //915MHz
};

const u16 RFM22BRateTbl[4][10] = 
{
  {0x1C1B,0x2083,0x21C0,0x2213,0x23A9,0x2400,0x2503,0x2A18,0x6E09,0x6FD5}, //1.2Kbps, Fdev=35KHz
  {0x1C1B,0x2041,0x2160,0x2227,0x2352,0x2400,0x2507,0x2A18,0x6E13,0x6FA9}, //2.4Kbps, Fdev=35KHz
  {0x1C1B,0x20A1,0x2120,0x224E,0x23A5,0x2400,0x2518,0x2A18,0x6E27,0x6F52}, //4.8Kbps, Fdev=35KHz
  {0x1C1C,0x20D0,0x2100,0x229D,0x2349,0x2400,0x2558,0x2A1A,0x6E4E,0x6FA5}  //9.6Kbps, Fdev=35KHz
};

const u16 RFM22BPowerTbl[4] = 
{
  0x6D0F,                   //20dbm  
  0x6D0E,                   //17dbm
  0x6D0D,                   //14dbm
  0x6D0C,                   //11dbm 
};

const u16 RFM22BConfigTbl[21] = 
{ 
  0x0803,                   //Reset FIFO
  0x0800, 
  0x097F,                   //C = 12.5p
  0x0BD2,                   //GPIO0 TxANT
  0x0C14,                   //GPIO1 RFData, RF Input
  0x0DD5,                   //GPIO2 RxANT
  0x1D40,                   //Enable AFC 
  0x6960,                   //Enable AGC(0 dB)
            
  0x3088,                   //Enable packet Rx&Tx; NoCRC
  0x3200,                   //No received header check
  0x330C,                   //Sync: 3Byte; No header
  0x340A,                   //Preamble length=5Byte
  0x36AA,                   //SyncWord = AA2DD4   
  0x372D,
  0x38D4,
  0x3E15,                   //Packet Length 21Byte
  0x7e15,                   //Rx receive threshold 21Byte
  
  0x7024,                   //Disable manchester
  0x7122,                   //FSK, FIFO mode
  0x7238,                   //Fdev= +/-35KHz
  0x0701                    //Enter standby mode
};

const u8  RFM22BData[] = {"HopeRF RFM COBRFM22BS"};

/**********************************************************
**Variable define
**********************************************************/
u8 gb_WaitStableFlag=0;                                    //State stable flag


/**********************************************************
**Name:     RFM22B_Config
**Function: Initialize RFM22B & set it entry to standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_Config(void)
{
  u8 i;
  
  SPIRead(0x03);                                           //Clear nIRQ
  SPIRead(0x04);  
  for(i=0;i<3;i++)                      
    SPIWrite(RFM22BFreqTbl[gb_FreqBuf_Addr][i]);           //setting frequency parameter
  for(i=0;i<10;i++)
    SPIWrite(RFM22BRateTbl[gb_RateBuf_Addr][i]);           //setting rf rate parameter
  SPIWrite(RFM22BPowerTbl[gb_PowerBuf_Addr]);              //Setting output power parameter
  for(i=0;i<21;i++)
    SPIWrite(RFM22BConfigTbl[i]);                          //setting base parameter
}

/**********************************************************
**Name:     RFM22B_EntryRx
**Function: Set RFM22B entry Rx_mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_EntryRx(void)
{
  Input_DIO0();                                            //RxStatus
  RFM22B_Config();                                         //Module parameter setting  
  
  SPIWrite(0x0502);                                        //Packet received interrupt
  SPIWrite(0x0705);                                        //Define to Rx mode 
 
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusTx=0;                                           //Clear Tx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}

/**********************************************************
**Name:     RFM22B_EntryTx
**Function: Set RFM22B entry Tx_mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_EntryTx(void)
{
  Input_DIO1();                                            //TxStatus
  RFM22B_Config();                                         //Module parameter setting
 
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusRx=0;                                           //Clear Rx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}

/**********************************************************
**Name:     RFM22B_TxWaitStable
**Function: Determine whether the state of stable Tx
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_TxWaitStable(void)
{ 
  u8 temp=0;
  if(gb_WaitStableFlag==0x0f) 
  {    
    if(gb_SysTimer_1S!=0)
    {
      temp=SPIRead(0x76);                                  //Read "76H" reg
      if(temp==(RFM22BFreqTbl[gb_FreqBuf_Addr][1]&0xff))   //SPI read success
      {
        gb_WaitStableFlag=1;        
      }
    }
    else
    {
      gb_ErrorFlag=1;
      gb_WaitStableFlag=0;
    }
  }
}

/**********************************************************
**Name:     RFM22B_RxWaitStable
**Function: Determine whether the state of stable Rx
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_RxWaitStable(void)
{ 
  u8 temp=0;
  if(gb_WaitStableFlag==0x0f) 
  {
    if(gb_SysTimer_1S!=0)
    {
      temp=SPIRead(0x76);                                  //Read "76H" reg
      if(DIO0==1 && temp==(RFM22BFreqTbl[gb_FreqBuf_Addr][1]&0xff)) //Receive ready and SPI read success
      {
        gb_WaitStableFlag=1;        
      }
    }
    else
    {
      gb_ErrorFlag=1;
      gb_WaitStableFlag=0;
    }
  }
}

/**********************************************************
**Name:     RFM22B_ClearFIFO
**Function: Change to RxMode from StandbyMode, can clear FIFO buffer
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_ClearFIFO(void)
{
  SPIRead(0x03);                                           //Clear nIRQ
  SPIRead(0x04);  
  SPIWrite(0x0701);                                        //Standby
  SPIWrite(0x0803);                                        //Clear FIFO
  SPIWrite(0x0800); 
  SPIWrite(0x0705);                                        //entry RxMode
}

/**********************************************************
**Name:     RFM22B_Sleep
**Function: Set RFM22B to sleep mode 
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_Sleep(void)
{
  SPIWrite(0x0803);                                        //Clear FIFO
  SPIWrite(0x0800); 
  SPIRead(0x03);                                           //Clear nIRQ
  SPIRead(0x04);  
  SPIWrite(0x0700);                                        //Sleep
}

/**********************************************************
**Name:     RFM22B_Standby
**Function: Set RFM22B to Standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_Standby(void)
{
  SPIWrite(0x0803);                                        //Clear FIFO
  SPIWrite(0x0800); 
  SPIRead(0x03);                                           //Clear nIRQ
  SPIRead(0x04);  
  SPIWrite(0x0701);                                        //Standby
}

/**********************************************************
**Name:     RFM22B_ReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
u8 RFM22B_ReadRSSI(void)
{
  u8 temp=0xff;

  temp=SPIRead(0x26);
  
  return temp;
}

/**********************************************************
**Name:     RFM22B_RxPacket
**Function: Check for receive one packet
**Input:    none
**Output:   "!0"-----Receive one packet
**          "0"------Nothing for receive
**********************************************************/
u8 RFM22B_RxPacket(void)
{
  u8 i; 
 
  RFM22B_RxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    gb_StatusRx=1;                                         //Rx state stable
    RFM22B_ClearFIFO();
  }       
 
  if((nIRQ0==0) && (gb_StatusRx==1))
  {
    for(i=0;i<21;i++) 
      gb_RxData[i] = 0x00;
    SPIBurstRead(0x7F, gb_RxData, 21);  
    RFM22B_ClearFIFO();
    for(i=0;i<14;i++)
    {
      if(gb_RxData[i]!=RFM22BData[i])
        break;  
    }
    if(i>=14)   
      return(1);                                           //Rx success
    else
      return(0);
  }
  else
    return(0);
}

/**********************************************************
**Name:     RFM22B_TxPacket
**Function: Check RFM22B send over & send next packet
**Input:    none
**Output:   TxFlag=1, Send success
**********************************************************/
u8 RFM22B_TxPacket(void)
{
  u8 TxFlag=0;
  
  RFM22B_TxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    if(gb_StatusTx==0)                                     //First entry Tx mode
    {
      gb_StatusTx=1;                                       //Rx state stable                        
      gw_TxTimer=gw_RF_SentInterval;                       //Send time interval
      BurstWrite(0x7F, (u8 *)RFM22BData, 21);              //Send one packet data
      SPIWrite(0x0709);                                    //Entry Tx mode
      TxFlag=1;
      gb_WaitStableFlag=3;
    }
  }
  if(gb_StatusTx==1)
  {
    if(gb_WaitStableFlag==3 && nIRQ0==0)                   //Packet send over
    {
      gb_WaitStableFlag=0;
      RFM22B_Standby();                                    //Entry Standby mode
    }   
    if(gw_TxTimer==0)                                      //It's time to Sending
    {
      gw_TxTimer=gw_RF_SentInterval;                       //Send time interval
      gb_SysTimer_1S=3;
      gb_WaitStableFlag=0x0f;                              //Promised to call mode stable decide
    }
    if(gb_WaitStableFlag==2)                               //Mode stable
    {
      SPIRead(0x03);                                       //Clear nIRQ
      SPIRead(0x04);  
      BurstWrite(0x7F, (u8 *)RFM22BData, 21);              //Send data
      SPIWrite(0x0709);                                    //Entry Tx mode
      TxFlag=1;
      gb_WaitStableFlag=3;
    }
  }
  return TxFlag;
}

/**********************************************************
**Parameter table define in test mode
**********************************************************/


/**********************************************************
**Name:     RFM22B_TestConfig
**Function: Initialize RFM22B in test mode & set it entry to direct mode
**Input:    none
**Output:   none
**********************************************************/
void RFM22B_TestConfig(void)
{
  RFM22B_Config();
  SPIWrite(0x7102);                                        //Direct Mode
}

/**********************************************************
**Name:     RFM22B_EntryTestRx
**Function: Set RFM22B entry Rx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_EntryTestRx(void)
{
  Input_DIO0();                                            //RxStatus
  Input_RFData();    
  RFM22B_TestConfig();                                     //Module parameter setting
  SPIWrite(0x0705);                                        //Define to Rx mode 
  
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusTx=0;                                           //Clear Tx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}

/**********************************************************
**Name:     RFM22B_EntryTestTx
**Function: Set RFM22B entry Tx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_EntryTestTx(void)
{
  Input_DIO1();                                            //TxStatus
  RFM22B_TestConfig();                                     //Module parameter setting
  SPIWrite(0x0709);                                        //Define to Tx mode 
    
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusRx=0;                                           //Clear Rx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}

/**********************************************************
**Name:     RFM22B_TestRx
**Function: RFM22B Rx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_TestRx(void)
{
  RFM22B_RxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    gb_StatusRx=1;    
  }
}

/**********************************************************
**Name:     RFM22B_TestTx
**Function: RFM22B Tx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM22B_TestTx(void)
{ 
  RFM22B_TxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    gb_StatusTx=1;  
    
    Output_RFData();                               
    RFData=1;
  }
}

u8 gb_WorkmodeBackup=0;
/**********************************************************
**Name:     RFM22B_Running
**Function: RFM22B running function
**Input:    mode, work mode(FS-T,FS-R,T-T,T-R)
            WorkStatus, work status(normal,standby,sleep)
            ParaChangeFlag, Parameter changed when ParaChangeFlag=1
            *TxFlag, Tx success flag affect Tx count
            *RxFlag, Rx success flag affect Rx count
            *RSSI, RSSI value
**Output:   none
**********************************************************/
void RFM22B_Running(u8 mode,u8 WorkStatus,u8 ParaChangeFlag,u8 *TxFlag,u8 *RxFlag,u8 *RSSI)
{ 
  u8 temp;  
  if(WorkStatus==C_ModuleWorkMode_FSK)                  	 //Normal status
  {
    switch(mode)
    {
      case C_SysMode_FSKTx:                                //Normal send mode(Abbreviation:FS-T)
        if(ParaChangeFlag==1)                              //Parament changed
        {                   
          RFM22B_EntryTx();
        }         
        temp=RFM22B_TxPacket(); 
        if(temp==1){*TxFlag=1;}
        break;
      case C_SysMode_FSKRx:                                //Normal receive mode(Abbreviation:FS-R)      
        if(ParaChangeFlag==1)
        {           
          RFM22B_EntryRx();
        }
        temp=RFM22B_RxPacket(); 
        if(temp==1)
        {
          *RxFlag=1;
          BeepOn((u16*)MusicTab_RxSuccess);                //Buzzer on When received data success
        }
        
        *RSSI=RFM22B_ReadRSSI();                            //Read RSSI
        break;
      case C_SysMode_TestTx:                               //Test send mode(Abbreviation:T-T)   
        if(ParaChangeFlag==1)
        {           
          RFM22B_EntryTestTx();
        }
        RFM22B_TestTx();
        break;
      case C_SysMode_TestRx:                               //Test receive mode(Abbreviation:T-R)  
        if(ParaChangeFlag==1)
        {       
          RFM22B_EntryTestRx();
        }
        RFM22B_TestRx();
        
        *RSSI=RFM22B_ReadRSSI();                            //Read RSSI
        break;
    } 
  }
  else if(WorkStatus==C_ModuleWorkMode_Standby)            //Standby status
  {
    if(gb_WorkmodeBackup!=C_ModuleWorkMode_Standby)
    	RFM22B_Standby();
  }
  else if(WorkStatus==C_ModuleWorkMode_Sleep)              //Sleep status
  {
    if(gb_WorkmodeBackup!=C_ModuleWorkMode_Sleep)
    	RFM22B_Sleep();
  }
  gb_WorkmodeBackup=WorkStatus;
}