/***************************************************************
           RFM2xbmodule.c
***************************************************************/
#include "DK-rfm2xb.h"
#include "Keyscan.h"
#include "RFM-mode.h"

//*********************************************************
const byte RFM23BData[21] = {
	                  'H', 'o', 'p', 'e', 'R', 'F', ' ', 'R', 'F', 'M', 
	                  ' ', 'C', 'O', 'B', 'R', 'F', 'M', '2', '3', 'B', 'S', 
	                  };

const byte RFM22BData[21] = {
	                  'H', 'o', 'p', 'e', 'R', 'F', ' ', 'R', 'F', 'M', 
	                  ' ', 'C', 'O', 'B', 'R', 'F', 'M', '2', '2', 'B', 'S', 
	                  };

const byte RFM43BData[21] = {
	                  'H', 'o', 'p', 'e', 'R', 'F', ' ', 'R', 'F', 'M', 
	                  ' ', 'C', 'O', 'B', 'R', 'F', 'M', '4', '3', 'B', 'S', 
	                  };

const byte RFM42BData[21] = {
	                  'H', 'o', 'p', 'e', 'R', 'F', ' ', 'R', 'F', 'M', 
	                  ' ', 'C', 'O', 'B', 'R', 'F', 'M', '4', '2', 'B', 'S', 
	                  };
byte RxData[32];

const word RF2xFreqTbl[5][3] = {
                               {0x7547, 0x767D, 0x7700},	//0	
							   {0x7547, 0x767D, 0x7700},	//315MHz	
							   {0x7553, 0x7664, 0x7700}, 	//434MHz
							   {0x7573, 0x7664, 0x7700}, 	//868MHz	
							   {0x7575, 0x76BB, 0x7780}, 	//915MHz
	                           };



const word RF2xConfigTbl[35] = {
						0x0506,
						0x0600,

					  	0x0803, 		//software reset FIFO
					  	0x0800, 

					    0x097F, 		//09	config crystal carry cap value
					    0x0BD2, 		//0B	GPIO0 Txen
					    0x0C14, 		//0C	GPIO1	RfData	RF Input
					    0x0DD5,  		//0D	GPIO2 Rxen 

						0x1C1B,			//Rx para. 2.4Kbps +/-35KHz
						0x2041,
						0x2160,
						0x2227,
						0x2352,
						0x2400,
						0x2507,
						0x1E0A,
						0x2A1E,
						0x1F03,
						0x6960,
										//Packet Setting
					    0x3088, 		//30  	config the data package function,NoCRC
					    0x3200, 		//32	No Header check
					    0x330C, 		//33    Sync 3Byte£»No Header
					    0x340A, 		//34	5Byte Preamble
					    0x3600+0xAA,	//36	SyncWord = AA2DD4		
					    0x3700+0x2D,	//37
					    0x3800+0xD4,	//38
					    0x3E00+0x15,	//3E	PK Length 21Byte
	
						0x5880, 		//58
						0x6D0F, 		//5D 	Max Power 
						0x6E13,			//6E    2.4Kbps Tx
						0x6FA9,

					    0x7024, 		//70	disable mancester encode
					    0x7122, 		//71	FIFO FSK
						0x7238,			//72    +/-35KHz
						0x0701			//07    Standby Mode
							};

/******************************************************************************
**Name£º          RFM2xB_EntryRx
**Function£º      config RFM2xB module into Rx mode
**Input£º         none
**Output£º        0¡ª¡ªabnormal ; 1--normal into Rx mode
******************************************************************************/
byte RFM2xB_EntryRx(void)
{
 TRISA2 = 1;		        //RxStatus
 RFM2xB_Config();
 SPIWrite(0x0705);		    //config RX state
 for(systime=0;systime<3;)  //wait for state stable
 	{
 	Keyscan();
	if(key_down)
		break;
 	if(RxANT)
 		break;
 	}
 if(systime>=3)	
 	return(0);				//return abnormal
 else
 	return(1);				//return normal
}

/******************************************************************************
**Name£º          RFM2xB_EntryTx
**Function£º      config RFM2xB module into Tx mode
**Input£º         none
**Output£º        0¡ª¡ªabnormal ; 1--normal into Tx mode
******************************************************************************/
byte RFM2xB_EntryTx(void)
{
    TRISA3 = 1;                    //TxStatus
    RFM2xB_Config();
    switch(TypeID)
 	{
 	    case RFM43B:
 		              BurstWrite(0x7F, (byte *)RFM43BData, 21);
 		              break;
 	    case RFM42B:
 		              BurstWrite(0x7F, (byte *)RFM42BData, 21);
 		              break;
 	    case RFM22B:	
 		              BurstWrite(0x7F, (byte *)RFM22BData, 21);
 		              break;
 	    case RFM23B:
 	    default:
 		              BurstWrite(0x7F, (byte *)RFM23BData, 21);	
 		              break;
 	}
    SPIWrite(0x0709);				//config Tx state	
    for(systime=0;systime<3;)		//wait for state stable
 	{
        Keyscan();	
 	    if(key_down)
 		    break;
	    if(TxANT)
		    break;
 	}	
    if(systime>=3)	
 	    return(0);               //return abnormal
    else
 	    return(1);               //return normal
}

/******************************************************************************
**Name£º        RFM2xB_ClearFIFO
**Function£º    clear FIFO
**Input£º       none
**Output£º      none
******************************************************************************/
void RFM2xB_ClearFIFO(void)
{
 SPIRead(0x03);		//Clr nIRQ
 SPIRead(0x04);	
 SPIWrite(0x0701);	//to Standby
 SPIWrite(0x0803);	//Clr FIFO
 SPIWrite(0x0800);	
 SPIWrite(0x0705);	//to Rx
}

/******************************************************************************
**Name£º         RFM2xB_Sleep
**Function£º     config RFM2xB module into sleep
**Input£º        none
**Output£º       none
******************************************************************************/
void RFM2xB_Sleep(void)
{
 SPIWrite(0x0803);	//Clr FIFO
 SPIWrite(0x0800);	
 SPIRead(0x03);		//Clr nIRQ
 SPIRead(0x04);	
 SPIWrite(0x0700);	//Sleep
}

/******************************************************************************
**Name£º           RFM2xB_Standby
**Function£º       config RFM2xB module into Standby mode
**Input£º          none
**Output£º         none
******************************************************************************/
void RFM2xB_Standby(void)
{
 SPIWrite(0x0803);	//Clr FIFO
 SPIWrite(0x0800);	
 SPIRead(0x03);		//Clr nIRQ
 SPIRead(0x04);	
 SPIWrite(0x0701);	//Standby
}

/******************************************************************************
**Name£º          RFM2xB_Config
**Function£º      Initialization: into Standby mode
**Input£º         none
**Output£º        none
******************************************************************************/
void RFM2xB_Config(void)
{
 byte i;
 
 TRISD0 = 0;
 SDN = 1;
 for(systime=0;systime<100;);
 SDN = 0;
 for(systime=0;systime<30;)
 
 SPIRead(0x03);		//Clr nIRQ
 SPIRead(0x04);	
 
 for(i=0;i<3;i++)
 	SPIWrite(RF2xFreqTbl[FreqSel][i]);
 for(i=0;i<35;i++)			//config module basc parameters,then into the StandbyMode
	SPIWrite(RF2xConfigTbl[i]); 
}

/******************************************************************************
**Name£º          RFM2xB_RxChk
**Function£º      be sure module in Rx state
**Input£º         none
**Output£º        0¡ª¡ªError ; 1--normal
******************************************************************************/
byte RFM2xB_RxChk(void)
{
 TRISA2 = 1;
 if(RxANT)
 	return(1);
 else
 	return(0);
}

/******************************************************************************
**Name£º           RFM2xB_RxPacket
**Function£º       receive one package datas
**Input£º          none
**output£º         1¡ª¡ªreceive sucessful
**                 0¡ª¡ªno receive or receive wrong
******************************************************************************/
byte RFM2xB_RxPacket(void)
{
 byte i;
 if(!IRQ_0)
 	{
 	for(i=0;i<21;i++)	
 		RxData[i] = 0x00; 	
 	SPIBurstRead(0x7F, RxData, 21);			
 	RFM2xB_ClearFIFO();
 	for(i=0;i<14;i++)
 		{
 		if(RxData[i]!=RFM23BData[i])
 			break;	
 		}
 	if(i>=14)		//check sucessful
		return(1);
 	else
 		return(0);
 	
 	}	
 return(0);
}

/******************************************************************************
**Name£º          RFM2xB_TxPacket
**Function£º      transmit one package datas
**Input£º         none
**Output£º        none
******************************************************************************/
void RFM2xB_TxPacket(void)
{
     if(!IRQ_0)
     {
 		SPIRead(0x03);		//Clr nIRQ
 		SPIRead(0x04);	
 		switch(TypeID)
 			{
 			case RFM43B:
 				BurstWrite(0x7F, (byte *)RFM43BData, 21);
 				break;
 			case RFM42B:
 				BurstWrite(0x7F, (byte *)RFM42BData, 21);
 				break;
 			case RFM22B:	
 				BurstWrite(0x7F, (byte *)RFM22BData, 21);
 				break;
 			case RFM23B:
 			default:
 				BurstWrite(0x7F, (byte *)RFM23BData, 21);	
 				break;
 			}
 		SPIWrite(0x0709);		//Tx state	
     }
}

/******************************************************************************
**Name£º            RFM2xB_TestRx
**Funtion£º         config RFM2xB module into Rx Test mode
**Input£º           none
**Output£º          0¡ª¡ªabnormal ; 1--normal
******************************************************************************/
byte RFM2xB_TestRx(void)
{
 TRISA2 = 1;
 TRISA1 = 1;
 RFM2xB_TestConfig();
 SPIWrite(0x0705);		//Rx state
 for(systime=0;systime<5;)	//wait for state stable
 	{
 	Keyscan();	
 	if(RxANT)
 		break;
	if(key_down)
		break;
 	}
 if(systime>=5)	
 	return(0);				//return abnormal
 else
 	return(1);				//return normal
}

/******************************************************************************
**Name£º            RFM2xB_TestTx
**Funtion£º         config RFM2xB module into Tx Test mode
**Input£º           none
**Output£º          0¡ª¡ªabnormal ; 1--normal
******************************************************************************/
byte RFM2xB_TestTx(byte unmodulator)
{
 TRISA3 = 1;
 RFM2xB_TestConfig();
 if(!unmodulator)
 	SPIWrite(0x7100);		//unmodulator mode
 SPIWrite(0x0709);			//Tx state
 for(systime=0;systime<5;)	//wait for state stable
 	{
 	Keyscan();	
 	if(TxANT)
 		break;
 	}	
 if(systime>=5)	
 	return(0);				//return abnormal
 else
 	{
	TRISA1 = 0;
	RFdata = 1;
 	return(1);				//return normal
	}
}

/******************************************************************************
**Name£º         RFM2xB_TestConfig
**Function£º     initialization module Test mode
**Input£º        none
**Output£º       none
******************************************************************************/
void RFM2xB_TestConfig(void)
{
 RFM2xB_Config();
 SPIWrite(0x0500);
 SPIWrite(0x0600);
 SPIWrite(0x0C10); 		//0C	GPIO1	RfData	RF Output Tx
 SPIWrite(0x3000);
 SPIWrite(0x7102);		//Direct Mode
}
/******************************************************************************
**Name£º      RFM2xB_NormalTX
**Function£º  config RFM2xB module in Normal TX mode
**Input£º     none
**Output£º    none
******************************************************************************/
void RFM2xB_NormalTX(void)
{
	if(1 == RFM2xB_EntryTx())
	{
		systime1 = 0;
		while(1)
		{
			if(systime1>1000)
			{
				systime1 = 0;
				RFM2xB_TxPacket();
  				led_tx();
			}
			Keyscan();
				if(key_down)
					break;
		}
	}
	else
	{
		RFM2xB_Config();
	}
}
/******************************************************************************
**Name£º      RFM2xB_NormalRX
**Function£º  config RFM2xB module in Normal RX mode
**Input£º     none
**Output£º    none
******************************************************************************/
void RFM2xB_NormalRX(void)
{
	if(1 == RFM2xB_EntryRx())
	{
		led_rx();
		while(1)
		{
			if(1 == RFM2xB_RxPacket())
			{
				BeepOn();
				for(systime=0;systime<200;);
				BeepOff();
			}
			Keyscan();
			if(key_down)
				break;
		}
	}
}
