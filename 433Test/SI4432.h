/*
 *
 * SI4432 driver for Particle Photon
 * Version 1.0
 *
 */
#ifndef SI4432_H
#define SI4432_H

#define N_CONFIG 35
#define N_RETRY 3
#define FREQSEL433M 2
#define FIFOSIZE 64
#define Tx_WAIT_TIMEOUT 3000	// 3s timeout for Tx

const uint16_t SI4432FreqTable[5][3] = {
   {0x7547, 0x767D, 0x7700},	//0	
   {0x7547, 0x767D, 0x7700},	//315MHz	
   {0x7553, 0x7664, 0x7700}, 	//434MHz
   {0x7573, 0x7664, 0x7700}, 	//868MHz	
   {0x7575, 0x76BB, 0x7780}, 	//915MHz
};



const uint16_t SI4432ConfigTable[N_CONFIG] = {
	0x0507,			//05 Interrupt events: pksend, pkvalid, crcerror
	0x0600,

  	0x0803, 		//software reset FIFO
  	0x0800, 

    0x097F, 		//09	Crystal Oscillator Load Capacitance matching value
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
    0x3528,			//35	Preamble detetion threshold
    0x3600+0xAA,	//36	SyncWord = AA2DD4		
    0x3700+0x2D,	//37
    0x3800+0xD4,	//38
    0x3E00+0x15,	//3E	PK Length 21Byte

	0x5880, 		//58
//	0x6A0B,			//6A	write 0x0B to the AGC Override 2 register
//	0x6804,			//68	write 0x04 to the Deltasigma ADC Tuning 2 register
	0x6D0F, 		//5D 	Max Power 
	0x6E13,			//6E    2.4Kbps Tx
	0x6FA9,

    0x7024, 		//70	disable mancester encode
    0x7122, 		//71	FIFO FSK
	0x7238			//72    +/-35KHz
};

int irqStatus1;
int irqStatus2;

byte SPIBurstRead(byte reg, byte *buffer, int len);
byte SPIBurstWrite(byte reg, byte *buffer, int len);
byte SPIWrite(uint16_t data);
byte SPIRead(byte reg);

void SI4432_Reset(void);
byte SI4432_SetRx(void);
byte SI4432_SetTx(void);
void SI4432_ClearFIFO(void);
void SI4432_Sleep(void);
void SI4432_Standby(void);
void SI4432_Config(void);
byte SI4432_RxChk(void);
byte SI4432_RxPacket(byte *buffer, int len);
byte SI4432_TxPacket(byte *buffer, int len);
//byte RFM2xB_TestRx(void);
//byte RFM2xB_TestTx(byte unmodulator);
//void RFM2xB_TestConfig(void);
byte SI4432_NormalTX(byte *buffer, int len);
byte SI4432_NormalRX(byte *buffer, int len);

#endif

