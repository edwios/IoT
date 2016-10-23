/*
 * SI4432 driver
 */

#include "SI4432.h"

void init(void)
{
	FreqSel = FREQSEL433M;			// 433MHz
}


/******************************************************************************
**Name:          SI4432_Reset
**Function:      Software Reset SI4432 module
**Input:         none
**Output:        none
******************************************************************************/
void SI4432_Reset(void)
{
	SPIWrite(0x0708);		    	//Software reset
	while (nIRQ);					// Wait for chip ready
	SPIRead(0x03);					//Clr nIRQ
	SPIRead(0x04);
}

/******************************************************************************
**Name:          SI4432_SetRx
**Function:      config SI4432 module into Rx mode
**Input:         none
**Output:        0--abnormal ; 1--normal into Rx mode
******************************************************************************/
byte SI4432_SetRx(void)
{
	int retry = N_RETRY;

	digitalWrite(TRISA2,1);			//RxStatus
	while ((retry-- > 0) && (!SI4432_SetRx())) {
		SI4432_Config();
		delay(100);
	}
	if (retry > 0) {
		SPIWrite(0x0705);		    //config RX state
		return 1;
	}
	return 0;
}

/******************************************************************************
**Name:          SI4432_SetTx
**Function:      config SI4432 module into Tx mode
**Input:         none
**Output:        0--abnormal ; 1--normal into Tx mode
******************************************************************************/
byte SI4432_SetTx(void)
{
	int retry = N_RETRY;

    digitalWrite(TRISA3,1);				//TxStatus
	while ((retry-- > 0) && (!SI4432_SetTx())) {
		SI4432_Config();
		delay(100);
	}
	if (retry > 0) {
	    SPIWrite(0x0709);				//config Tx state	
	    return 1;
	}
	return 0;
}

/******************************************************************************
**Name:        SI4432_ClearFIFO
**Function:    clear FIFO
**Input:       none
**Output:      none
******************************************************************************/
void SI4432_ClearFIFO(void)
{
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	SPIWrite(0x0701						//to Standby
	SPIWrite(0x0803						//Clr FIFO
	SPIWrite(0x0800);	
}

/******************************************************************************
**Name:         SI4432_Sleep
**Function:     config SI4432 module into sleep
**Input:        none
**Output:       none
******************************************************************************/
void SI4432_Sleep(void)
{
	SPIWrite(0x0803);					//Clr FIFO
	SPIWrite(0x0800);	
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	SPIWrite(0x0700);					//Sleep
}

/******************************************************************************
**Name:           SI4432_Standby
**Function:       config SI4432 module into Standby mode
**Input:          none
**Output:         none
******************************************************************************/
void SI4432_Standby(void)
{
	SPIWrite(0x0803);					//Clr FIFO
	SPIWrite(0x0800);	
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	SPIWrite(0x0701);					//Standby
}

/******************************************************************************
**Name:          SI4432_Config
**Function:      Initialization: into Standby mode
**Input:         none
**Output:        none
******************************************************************************/
void SI4432_Config(void)
{
	byte i;

	digitalWrite(TRISD0,0);
	digitalWrite(SDN,1);
	delay(100);
	digitalWrite(SDN,0);				// PWRDN pin low to turn on radio
	delay(30);							// wait at least 15ms

	SPIRead(0x03);						//Clear any pending nIRQ
	SPIRead(0x04);	

	for(i=0;i<3;i++)					// Config center frequency
		SPIWrite(RF2xFreqTbl[FreqSel][i]);
	for(i=0;i<N_CONFIG;i++)				//config module basc parameters
	SPIWrite(RF2xConfigTbl[i]); 
}

/******************************************************************************
**Name:          SI4432_RxChk
**Function:      be sure module in Rx state
**Input:         none
**Output:        0--Error ; 1--normal
******************************************************************************/
byte SI4432_RxChk(void)
{
	digitalWrite(TRISA2,1);
	return (digitalRead(RxANT));
}

/******************************************************************************
**Name:           SI4432_RxPacket
**Function:       receive package data in FIFO to buffer
**Input:          receive buffer
**				  len =0--dont care
**				  >0--max bytes to read
**output:         1--receive sucessful
**                0--no receive or receive wrong
******************************************************************************/
byte SI4432_RxPacket(byte *buffer, int len)
{
	byte i;
	int recvLen;

	if (!buffer) return 0;				// no nil buffer!

	SPIWrite(0x0701);					// Disable Rx chain
	recvLen = SPIRead(0x4B);			// How many bytes do we have?
	if (((len > 0) && (recvLen < len)) || (len == 0)) {
		len = recvLen;
	}
	if (len > FIFOSIZE)
		len = FIFOSIZE;					// so that we won't BOF
	for(i=0;i<len;i++)	
		buffer[i] = 0x00; 	
	SPIBurstRead(0x7F, buffer, len);			
	return len;
}

/******************************************************************************
**Name:          SI4432_TxPacket
**Function:      transmit package data in buffer to FIFO
**Input:         buffer, bytes to sent
**Output:        0--error
**				 1--ok
******************************************************************************/
byte SI4432_TxPacket(byte *buffer, int len)
{
	if (!buffer) return 0;				// no nil buffer!
	if (len > FIFOSIZE) return 0;		// no over size ego
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	BurstWrite(0x7F, buffer, len);
	SPIWrite(0x0709);					//Tx state
	while(digitalRead(nIRQ) && (millis()-lastTime<Tx_WAIT_TIMEOUT));
	irqStatus1 = SPIRead(0x03);			//Clr nIRQ
	irqStatus2 = SPIRead(0x04);
	if (irqStatus1 & 0x04 == 0x04)
		return 1;
	return 0;
}

/******************************************************************************
**Name:            SI4432_TestRx
**Funtion:         config SI4432 module into Rx Test mode
**Input:           none
**Output:          0--abnormal ; 1--normal
******************************************************************************/
/*
byte SI4432_TestRx(void)
{
 TRISA2 = 1;
 TRISA1 = 1;
 SI4432_TestConfig();
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
*/

/******************************************************************************
**Name:            SI4432_TestTx
**Funtion:         config SI4432 module into Tx Test mode
**Input:           none
**Output:          0--abnormal ; 1--normal
******************************************************************************/
/*
byte SI4432_TestTx(byte unmodulator)
{
 TRISA3 = 1;
 SI4432_TestConfig();
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
*/

/******************************************************************************
**Name:         SI4432_TestConfig
**Function:     initialization module Test mode
**Input:        none
**Output:       none
******************************************************************************/
void SI4432_TestConfig(void)
{
	SI4432_Config();
	SPIWrite(0x0500);
	SPIWrite(0x0600);
	SPIWrite(0x0C10); 		//0C	GPIO1	RfData	RF Output Tx
	SPIWrite(0x3000);
	SPIWrite(0x7102);		//Direct Mode
}

/******************************************************************************
**Name:      SI4432_NormalTX
**Function:  config SI4432 module in Normal TX mode
**Input:     buffer, max bytes to send
**Output:    0--error
**			 1--ok
******************************************************************************/
byte SI4432_NormalTX(byte *buffer, int len)
{
	byte result = 0;
	if (SI4432_SetTx()) {
		result = SI4432_TxPacket(buffer, len);
	}
	return result;
}
/******************************************************************************
**Name:      SI4432_NormalRX
**Function:  config SI4432 module in Normal RX mode
**Input:     buffer, max bytes to read
**Output:    0--error
**			 >0--received data length
******************************************************************************/
byte SI4432_NormalRX(byte *buffer, int len)
{
	byte result = 0;
	if (SI4432_SetRx()) {
		while(digitalRead(nIRQ) && (millis()-lastTime<Tx_WAIT_TIMEOUT));
		irqStatus1 = SPIRead(0x03);				// Read irq status
		irqStatus2 = SPIRead(0x04);
		if (irqStatus1 & 0x02 == 0x02) {		// We have packets waiting
			result = SI4432_RxPacket(buffer, len);
			SI4432_ClearFIFO();
			SPIWrite(0x0705);					//to Rx
		} else if (irqStatus1 & 0x01 == 0x01) {
			SPIWrite(0x0701);					//disable receiver chain
			SPIWrite(0x0803);					// Reset FIFO
			SPIWrite(0x0800);
		}
	}
	return result;
}
