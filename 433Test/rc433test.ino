// This #include statement was automatically added by the Particle IDE.
#include "application.h"
#include "SI4432.h"
#include "common.h"

SYSTEM_THREAD(ENABLED);

byte gBuffer[FIFOSIZE];

void setup() {
    Serial.begin(9600);
    delay(1000);
    init();
    SI4432_Sleep();
}

void loop() {
    digitalWrite(LED, !digitalRead(LED));
    SI4432_NormalRX(gBuffer, 0);
    Serial.print("Recevied: ");
    String s = String("Hello");
    Serial.print(s);
    Serial.println();
    delay(1000);
}

void init(void) {
    nIRQ = NIRQPIN;
    nSEL = NSELPIN;
    SDN  = SDNPIN;
    DEBUG_PRINT("init()");
    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(DEFAULT_SPI_CLOCK_DIV);
    pinMode(nSEL, OUTPUT);
    pinMode(nIRQ, INPUT);
    pinMode(SDN, OUTPUT);
    digitalWrite(nSEL, 1);
    digitalWrite(SDN, 1);
    SI4432_Init();
}

/*
 * SPI APIs
 */

byte SPIBurstRead(byte reg, byte *buffer, int len)
{
	byte status = 0;

	if (!buffer) return 0;
	if (len > FIFOSIZE) return 0;

	DEBUG_PRINT("SPIBurstRead reg %x to buffer", reg);
	digitalWrite(nSEL, 0);
	status = SPI.transfer(reg & ~0x80);
	while (len--) {
		*buffer++ = SPI.transfer(0xFF);
	}
	digitalWrite(nSEL, 1);
	DEBUG_PRINT("SPIBurstRead status is %d", status);
	return status;
}

byte SPIBurstWrite(byte reg, byte *buffer, int len)
{
	byte status = 0;

	if (!buffer) return 0;
	if (len > FIFOSIZE) return 0;

	DEBUG_PRINT("SPIBurstWrite buffer to reg %x", reg);
	digitalWrite(nSEL, 0);
	status = SPI.transfer(reg | 0x80);
	while (len--) {
		SPI.transfer(*buffer++);
	}
	digitalWrite(nSEL, 1);
	DEBUG_PRINT("SPIBurstWrite status is %d", status);
	return status;
}

byte SPIWrite(uint16_t data)
{
	byte status = 0;

	byte val = (byte)(data & 0x00ff);
	byte reg = data >> 8;
	DEBUG_PRINT("SPIWrite reg %x with value %x", reg, val);
	digitalWrite(nSEL, 0);
	status = SPI.transfer(reg | 0x80);
	SPI.transfer(val);
	digitalWrite(nSEL, 1);
	DEBUG_PRINT("SPIWrite status is %d", status);
	return status;
}

byte SPIRead(byte reg)
{
	byte status = 0;
	byte val;

	DEBUG_PRINT("SPIRead from reg %x", reg);
	digitalWrite(nSEL, 0);
	status = SPI.transfer(reg & ~0x80);
	val = SPI.transfer(0xFF);
	digitalWrite(nSEL, 1);
	DEBUG_PRINT("SPIRead with result %x", val);
	return val;
}

 
/*
 * SI4432 driver
 */

void SI4432_Init(void)
{
    DEBUG_PRINT("SI4432 Init and Config");
	FreqSel = FREQSEL433M;			// 433MHz
	SI4432_Config();
	SI4432_ClearFIFO();
	byte deviceType = SPIRead(0x00);
	byte deviceVersion = SPIRead(0x01);
	DEBUG_PRINT("Device type %x, version %x", deviceType, deviceVersion);
}


/******************************************************************************
**Name:          SI4432_Reset
**Function:      Software Reset SI4432 module
**Input:         none
**Output:        none
******************************************************************************/
void SI4432_Reset(void)
{
	DEBUG_PRINT("Reset SI4432");
	SPIWrite(0x0708);		    	//Software reset
    unsigned long lastTime = millis();
	while(digitalRead(nIRQ) && (millis()-lastTime<Tx_WAIT_TIMEOUT));
	SPIRead(0x03);					//Clr nIRQ
	SPIRead(0x04);
	SI4432_Init();
}

/******************************************************************************
**Name:          SI4432_SetRx
**Function:      config SI4432 module into Rx mode
**Input:         none
**Output:        0--abnormal ; 1--normal into Rx mode
******************************************************************************/
byte SI4432_SetRx(void)
{
	DEBUG_PRINT("Set SI4432 to Rx");
	int retry = N_RETRY;
	byte status = 0;

//	digitalWrite(TRISA2,1);			//RxStatus
	while ((retry-- > 0) && (!SI4432_RxChk())) {
	    SI4432_Config();
		SPIWrite(0x0705);		    //config RX state
	    delay(100);
    }
	if (retry > 0) {
		status = 1;
	}
	DEBUG_PRINT("SI4432_SetRx has status %d", status);
}

/******************************************************************************
**Name:          SI4432_SetTx
**Function:      config SI4432 module into Tx mode
**Input:         none
**Output:        0--abnormal ; 1--normal into Tx mode
******************************************************************************/
byte SI4432_SetTx(void)
{
	DEBUG_PRINT("Set SI4432 to Tx");
	int retry = N_RETRY;
	byte status = 0;

//    digitalWrite(TRISA3,1);				//TxStatus
	while ((retry-- > 0) && (!SI4432_TxChk())) {
		SI4432_Config();
	    SPIWrite(0x0709);				//config Tx state	
		delay(100);
	}
	if (retry > 0) {
	    status = 1;
	}
	DEBUG_PRINT("SI4432_SetTx has status %d", status);
}

/******************************************************************************
**Name:        SI4432_ClearFIFO
**Function:    clear FIFO
**Input:       none
**Output:      none
******************************************************************************/
void SI4432_ClearFIFO(void)
{
	DEBUG_PRINT("Clear SI4432 FIFO");
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	SPIWrite(0x0701);						//to Standby
	SPIWrite(0x0803);						//Clr FIFO
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
	DEBUG_PRINT("Put SI4432 to Sleep");
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
	DEBUG_PRINT("Put SI4432 to Standby");
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
    DEBUG_PRINT("Config SI4432 radio");
	byte i;

	digitalWrite(SDN,1);
	delay(100);
	digitalWrite(SDN,0);				// PWRDN pin low to turn on radio
	delay(30);							// wait at least 15ms

	SPIRead(0x03);						//Clear any pending nIRQ
	SPIRead(0x04);	

	for(i=0;i<3;i++)					// Config center frequency
		SPIWrite(SI4432FreqTable[FreqSel][i]);
	for(i=0;i<N_CONFIG;i++)				//config module basc parameters
	SPIWrite(SI4432ConfigTable[i]); 
}

/******************************************************************************
**Name:          SI4432_RxChk
**Function:      be sure module in Rx state
**Input:         none
**Output:        0--Error ; 1--normal
******************************************************************************/
byte SI4432_RxChk(void)
{
    DEBUG_PRINT("Check SI4432 in Rx mode");
	byte val = 0;
	byte status = 0;

	val = SPIRead(0x07);
	status = ((val & 0x04) == 0x04);

    DEBUG_PRINT("SI4432 Rx is %d", status);
	return status;
}

/******************************************************************************
**Name:          SI4432_TxChk
**Function:      be sure module in Tx state
**Input:         none
**Output:        0--Error ; 1--normal
******************************************************************************/
byte SI4432_TxChk(void)
{
    DEBUG_PRINT("Check SI4432 in Tx mode");
	byte val = 0;
	byte status = 0;

	val = SPIRead(0x07);
	status = ((val & 0x08) == 0x08);

    DEBUG_PRINT("SI4432 Tx is %d", status);
	return status;
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
	if (len > FIFOSIZE) return 0;		// no over size ego

    DEBUG_PRINT("Receive data from FIFO");
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
    DEBUG_PRINT("%d bytes read from FIFO", len);
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
	byte status = 0;

	if (!buffer) return 0;				// no nil buffer!
	if (len > FIFOSIZE) return 0;		// no over size ego

    DEBUG_PRINT("Send %d bytes of data in FIFO", len);
	SPIRead(0x03);						//Clr nIRQ
	SPIRead(0x04);	
	SPIBurstWrite(0x7F, buffer, len);
	SPIWrite(0x0709);   				//Tx state
	unsigned long lastTime = millis();
	while(digitalRead(nIRQ) && (millis()-lastTime<Tx_WAIT_TIMEOUT));
	irqStatus1 = SPIRead(0x03);			//Clr nIRQ
	irqStatus2 = SPIRead(0x04);
	if (irqStatus1 & 0x04 == 0x04)
		status = 1;
	DEBUG_PRINT("Send result is %d", status);
	return status;
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
	byte status = 0;

	if (!buffer) return 0;				// no nil buffer!
	if (len > FIFOSIZE) return 0;		// no over size ego

    DEBUG_PRINT("Prepare to send %d bytes of data from buffer", len);
	if (SI4432_SetTx() == 1) {
		status = SI4432_TxPacket(buffer, len);
	}
	DEBUG_PRINT("Send status is %d", status);
	return status;
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
	byte status = 0;
    unsigned long lastTime = millis();

	if (!buffer) return 0;				// no nil buffer!
	if (len > FIFOSIZE) return 0;		// no over size ego

    DEBUG_PRINT("Prepare to receive %d bytes of data", len);
	if (SI4432_SetRx() == 1) {
//		while(digitalRead(nIRQ) && (millis()-lastTime<Tx_WAIT_TIMEOUT));
//		if (millis()-lastTime<Tx_WAIT_TIMEOUT) {
		while(digitalRead(nIRQ));                   // Block and wait until we have packets
		if (1) {
			irqStatus1 = SPIRead(0x03);				// Read irq status
			irqStatus2 = SPIRead(0x04);
			if (irqStatus1 & 0x02 == 0x02) {		// We have packets waiting
				status = SI4432_RxPacket(buffer, len);
				SI4432_ClearFIFO();
				SPIWrite(0x0705);					//to Rx
			} else if (irqStatus1 & 0x01 == 0x01) {
				SPIWrite(0x0701);					//disable receiver chain
				SPIWrite(0x0803);					// Reset FIFO
				SPIWrite(0x0800);
			}
		} else {
			ERROR_PRINT("SI4432_NormalRX SI4432_SetRx timeout!");
			status = 0;
		}
	} else {
		ERROR_PRINT("SI4432_NormalRX SI4432_SetRx failed!");
		status = 0;
	}
    DEBUG_PRINT("Received %d bytes of data into buffer", status);
	return status;
}

