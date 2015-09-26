// This #include statement was automatically added by the Spark IDE.
#include "HTTPClient.h"
#include "MFRC522/MFRC522.h"

/****************************************************************************
 *   Project: Generic project name                                           *
 *                                                                           *
 ****************************************************************************/

/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 *
 * Released into the public domain.
 *
 * This sample shows how to setup a block on a MIFARE Classic PICC to be in "Value Block" mode.
 * In Value Block mode the operations Increment/Decrement/Restore and Transfer can be used.
 *
 ----------------------------------------------------------------------------- empty_skull
 
 - Aggiunti pin per arduino Mega
 - Scritto semplice codice per la scrittura e lettura
 
 - add pin configuration for arduino mega
 - write simple read/write Code for new entry user
 
 http://mac86project.altervista.org/
 
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin            Pin
 *            Arduino Uno      Arduino Mega      SPARK          MFRC522 board
 * ---------------------------------------------------------------------------
 * Reset      9                5                 ANY (D2)       RST
 * SPI SS     10               53                ANY (A2)       SDA
 * SPI MOSI   11               51                A5             MOSI
 * SPI MISO   12               50                A4             MISO
 * SPI SCK    13               52                A3             SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com.
 */

extern char* itoa(int a, char* buffer, unsigned char radix);

#define SS_PIN A6
#define RST_PIN D2

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

// In this sample we use the second sector (ie block 4-7). the first sector is = 0
// scegliere settore di lettura da 0 = primo settore
byte sector         = 1;
// block sector 0-3(sector0) 4-7(sector1) 8-11(sector2)
// blocchi di scrittura da 0-3(sector0) 4-7(sector1) 8-11(sector2)
byte valueBlockA    = sector*4;
byte valueBlockB    = valueBlockA+1;
byte valueBlockC    = valueBlockB+1;
byte trailerBlock   = valueBlockC+1;
byte status;
byte buffer[18];

// action 0: NOP
// action 1: read / verify card
// action 2: write new card
int action = 0;


// Time for each Deep sleep
#define PERIOD 300000
// Set UPDATE2CLOUD to true if you want the measurement sent to the cloud
#define UPDATE2CLOUD false
// Thingspeak API WRITE key
#define THINGSPEAK_API_WRITEKEY "<WRITEKEY>"
#define version 101

// Pin out defs
int led = D7;


// Thingspeak.com API
TCPClient client;
const char * WRITEKEY = THINGSPEAK_API_WRITEKEY;
const char * serverName = "api.thingspeak.com";
IPAddress server = {184,106,153,149};

// HTTP Client
unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;
// Init HTTP stuff
http_header_t headers[] = {
//  { "Content-Type", "application/json" },
//  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;


// 1.3" 12864 OLED with SH1106 and Simplified Chinese IC for Spark Core
int Rom_CS = A2;
unsigned long  fontaddr=0;
char dispCS[32];

unsigned int stimer = 0;
uint32_t lastSernum = 0;

unsigned int tinc = 1;

/****************************************************************************
 *****************************************************************************
 ****************************   OLED Driver  *********************************
 *****************************************************************************
 ****************************************************************************/


/*****************************************************************************
 Funtion    :   OLED_WrtData
 Description:   Write Data to OLED
 Input      :   byte8 ucCmd
 Output     :   NONE
 Return     :   NONE
 *****************************************************************************/
void transfer_data_lcd(byte ucData)
{
    Wire.beginTransmission(0x78 >> 1);
    Wire.write(0x40);      //write data
    Wire.write(ucData);
    Wire.endTransmission();
}

/*****************************************************************************
 Funtion    :   OLED_WrCmd
 Description:   Write Command to OLED
 Input      :   byte8 ucCmd
 Output     :   NONE
 Return     :   NONE
 *****************************************************************************/
void transfer_command_lcd(byte ucCmd)
{
    Wire.beginTransmission(0x78 >> 1);            //Slave address,SA0=0
    Wire.write(0x00);      //write command
    Wire.write(ucCmd);
    Wire.endTransmission();
}


/* OLED Initialization */
void initial_lcd()
{
    digitalWrite(Rom_CS, HIGH);
    Wire.begin();
    delay(20);
    transfer_command_lcd(0xAE);   //display off
    transfer_command_lcd(0x20); //Set Memory Addressing Mode
    transfer_command_lcd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    transfer_command_lcd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    transfer_command_lcd(0xc8); //Set COM Output Scan Direction
    transfer_command_lcd(0x00);//---set low column address
    transfer_command_lcd(0x10);//---set high column address
    transfer_command_lcd(0x40);//--set start line address
    transfer_command_lcd(0x81);//--set contrast control register
    transfer_command_lcd(0x7f);
    transfer_command_lcd(0xa1);//--set segment re-map 0 to 127
    transfer_command_lcd(0xa6);//--set normal display
    transfer_command_lcd(0xa8);//--set multiplex ratio(1 to 64)
    transfer_command_lcd(0x3F);//
    transfer_command_lcd(0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    transfer_command_lcd(0xd3);//-set display offset
    transfer_command_lcd(0x00);//-not offset
    transfer_command_lcd(0xd5);//--set display clock divide ratio/oscillator frequency
    transfer_command_lcd(0xf0);//--set divide ratio
    transfer_command_lcd(0xd9);//--set pre-charge period
    transfer_command_lcd(0x22); //
    transfer_command_lcd(0xda);//--set com pins hardware configuration
    transfer_command_lcd(0x12);
    transfer_command_lcd(0xdb);//--set vcomh
    transfer_command_lcd(0x20);//0x20,0.77xVcc
    transfer_command_lcd(0x8d);//--set DC-DC enable
    transfer_command_lcd(0x14);//
    transfer_command_lcd(0xaf);//--turn on oled panel
    
}

void lcd_address(byte page,byte column)
{
    
    transfer_command_lcd(0xb0 + column);   /* Page address */
    transfer_command_lcd((((page + 1) & 0xf0) >> 4) | 0x10);  /* 4 bit MSB */
    transfer_command_lcd(((page + 1) & 0x0f) | 0x00); /* 4 bit LSB */
}

void clear_screen()
{
    unsigned char i,j;
    digitalWrite(Rom_CS, HIGH);
    for(i=0;i<8;i++)
    {
        transfer_command_lcd(0xb0 + i);
        transfer_command_lcd(0x00);
        transfer_command_lcd(0x10);
        for(j=0;j<132;j++)
        {
            transfer_data_lcd(0x00);
        }
    }
    
}

void display_128x64(byte *dp)
{
    unsigned int i,j;
    for(j=0;j<8;j++)
    {
        lcd_address(0,j);
        for (i=0;i<132;i++)
        {
            if(i>=2&&i<130)
            {
                // Write data to OLED, increase address by 1 after each byte written
                transfer_data_lcd(*dp);
                dp++;
            }
        }
        
    }
    
}



void display_graphic_16x16(unsigned int page,unsigned int column,byte *dp)
{
    unsigned int i,j;
    
    digitalWrite(Rom_CS, HIGH);
    for(j=2;j>0;j--)
    {
        lcd_address(column,page);
        for (i=0;i<16;i++)
        {
            transfer_data_lcd(*dp);
            dp++;
        }
        page++;
    }
}


void display_graphic_8x16(unsigned int page,byte column,byte *dp)
{
    unsigned int i,j;
    
    for(j=2;j>0;j--)
    {
        lcd_address(column,page);
        for (i=0;i<8;i++)
        {
            // Write data to OLED, increase address by 1 after each byte written
            transfer_data_lcd(*dp);
            dp++;
        }
        page++;
    }
    
}


/*
 Display a 5x7 dot matrix, ASCII or a 5x7 custom font, glyph, etc.
 */

void display_graphic_5x7(unsigned int page,byte column,byte *dp)
{
    unsigned int col_cnt;
    byte page_address;
    byte column_address_L,column_address_H;
    page_address = 0xb0 + page - 1;//
    
    
    
    column_address_L =(column&0x0f);  // -1
    column_address_H =((column>>4)&0x0f)+0x10;
    
    transfer_command_lcd(page_address);     /*Set Page Address*/
    transfer_command_lcd(column_address_H); /*Set MSB of column Address*/
    transfer_command_lcd(column_address_L); /*Set LSB of column Address*/
    
    for (col_cnt=0;col_cnt<6;col_cnt++)
    {
        transfer_data_lcd(*dp);
        dp++;
    }
}

/**** Send command to Character ROM ***/
void send_command_to_ROM( byte datu )
{
    SPI.transfer(datu);
}

/**** Read a byte from the Character ROM ***/
byte get_data_from_ROM( )
{
    byte ret_data=0;
    ret_data = SPI.transfer(255);
    return(ret_data);
}


/*
 *     Read continuously from ROM DataLen's bytes and
 *     put them into pointer pointed to by pBuff
 */

void get_n_bytes_data_from_ROM(byte addrHigh,byte addrMid,byte addrLow,byte *pBuff,byte DataLen )
{
    byte i;
    digitalWrite(Rom_CS, LOW);
    delayMicroseconds(100);
    send_command_to_ROM(0x03);
    send_command_to_ROM(addrHigh);
    send_command_to_ROM(addrMid);
    send_command_to_ROM(addrLow);
    
    for(i = 0; i < DataLen; i++ ) {
        *(pBuff+i) =get_data_from_ROM();
    }
    digitalWrite(Rom_CS, HIGH);
}


/******************************************************************/

void display_string_5x7(byte y,byte x,const char *text, bool c)
{
    unsigned char i = 0, Tt = 0, j = 0;
    unsigned char addrHigh,addrMid,addrLow ;
    
    byte sl = strlen(text);
    
    if (c && x == 1)
        j = 21;
    else
        j = sl;
        
    while(i<j)
    {
        if (i<sl)
            Tt = text[i];
        else
            Tt = 0x20;
        if((Tt < 0x20) || (Tt > 0x7e))
        {
            Tt = 0x20;
        }
        unsigned char fontbuf[8];
        fontaddr = (Tt - 0x20);
        fontaddr = (unsigned long)(fontaddr*8);
        fontaddr = (unsigned long)(fontaddr+0x3bfc0);
        addrHigh = (fontaddr&0xff0000)>>16;
        addrMid = (fontaddr&0xff00)>>8;
        addrLow = fontaddr&0xff;
            
        get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);/*取8个字节的数据，存到"fontbuf[32]"*/
        
        display_graphic_5x7(y,x+1,fontbuf);/*显示5x7的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
        i+=1;
        x+=6;
    }
}

void display_string_5x7c(byte y,byte x,const char *text)
{
    display_string_5x7( y, x, text, 1);
}

/****************************************************************************
 *****************************************************************************
 ****************************  Data Upload   *********************************
 *****************************************************************************
 ****************************************************************************/

void sendToThingSpeak(const char * key, String mesg)
{
    client.stop();
    String outMesg = String("field1="+mesg);
    RGB.control(true);
    RGB.color(0,255,0);
    if (client.connect(server, 80)) {
        client.print("POST /update");
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverName);
        client.println("User-Agent: Spark");
        client.println("Connection: close");
        client.print("X-THINGSPEAKAPIKEY: ");
        client.println(key);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-length: ");
        client.println(outMesg.length());
        client.println();
        client.print(outMesg);
        client.flush();
        RGB.control(false);
    }
    else{
        RGB.color(255,0,0);
    }
}


int myVersion(String command)
{
    return version;
}

void perform_timer()
{
        display_string_5x7c(2,1,"                ");
        display_string_5x7c(3,1,"                ");
        display_string_5x7c(5,1,"                ");
        display_string_5x7c(4,1," --> Present Card <--");
        tinc = 0;
        lastSernum = 0;
}

char *URLEncode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    char encodedMsg[1024] = "";
    int i = 0;

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            encodedMsg[i++] = *msg;
        } else if (*msg == ' ') {
            encodedMsg[i++] = '+';
        } else {
            encodedMsg[i++] = '%';
            encodedMsg[i++] = hex[*msg >> 4];
            encodedMsg[i++] = hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}



/****************************************************************************
 *****************************************************************************
 **************************  Initialization  *********************************
 *****************************************************************************
 ****************************************************************************/


/****************************************************************************
 *****************************************************************************
 **************************  Main Proc Loop  *********************************
 *****************************************************************************
 ****************************************************************************/




void setup() {
    
    Serial.begin(9600);        // Initialize serial communications with the PC
    
    
    Time.zone(8);
    // Register a Spark variable here
    Spark.function("version", myVersion);
    
    
    pinMode(led, OUTPUT);
    
    
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
//    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    mfrc522.PCD_Init();        // Init MFRC522 card

    digitalWrite(Rom_CS, HIGH);
    
    initial_lcd();
    clear_screen();    //clear all dots
    digitalWrite(led, LOW);
    display_string_5x7c(4,1,"(c)2014 ioStation");
    sprintf(dispCS, "Version: %d", version);
    display_string_5x7c(6,1,dispCS);
    delay(1000);
    clear_screen();    //clear all dots
    sprintf(dispCS, "v%d", version);
    display_string_5x7c(7,1,dispCS);
    
    //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
//        key.keyByte[i] = 0xA0+i;
    }
    

}

void loop() {
    uint32_t sernum = 0;
    char msg[128];
    byte setmode = 0;
    byte cardSupported = 1;
    byte b1[18], b2[18];
    byte c1[18], c2[18];
    
    Time.timeStr().substring(4,19).toCharArray(dispCS, 16);
    display_string_5x7(1,1,"     ",0);
    display_string_5x7(1,20,dispCS,0);
    
    // increase timer value
    stimer = stimer + tinc;
    if (stimer >= 20) {
        stimer = 0;
        if (!setmode) perform_timer();
    }

    delay(250);

/*
********** Note: Anything below this line is not guaranteed to run @ each loop *********
*/

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    // Now a card is selected. The UID and SAK is in mfrc522.uid.
    
    // Dump UID
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        //                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //                Serial.print(mfrc522.uid.uidByte[i], HEX);
        sernum <<= 8;
        sernum += mfrc522.uid.uidByte[i];
    }
    
//      Prevent same card read twice in single session, confusing!
    if (lastSernum == sernum) {
        return;
    }

    // Dump PICC type
	byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	Serial.print("PICC type: ");
	Serial.println(mfrc522.PICC_GetTypeName(piccType));
	if (	piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
		&&	piccType != MFRC522::PICC_TYPE_MIFARE_1K
		&&	piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
		Serial.println("This sample only works with MIFARE Classic cards.");
        display_string_5x7c(4,1,"Card not supported.");
		cardSupported = 0;
	}

    lastSernum = sernum;
    
    if (sernum == 0xEB338645) {
        action = 1;
        setmode = 1;
        display_string_5x7c(3,1,"");
        display_string_5x7c(5,1,"");
        display_string_5x7c(4,1,"Set READ mode");
    } else if (sernum == 0xd01bf755) {
        action = 2;
        setmode = 1;
        display_string_5x7c(3,1,"");
        display_string_5x7c(5,1,"");
        display_string_5x7c(4,1,"Set WRITE mode");
    } else if (sernum == 0x6EF10423) {
        action = 3;
        setmode = 1;
        display_string_5x7c(3,1,"");
        display_string_5x7c(5,1,"");
        display_string_5x7c(4,1,"Set ERASE mode");
    } else {
        display_string_5x7c(4,1,"");
        setmode = 0;
    }
    // Authenticate using key A.
    // avvio l'autentificazione A
    //Serial.println("Authenticating using key A...");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Authenticate using key B.
    // avvio l'autentificazione B
    //Serial.println("Authenticating again using key B...");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    
    if (action == 3 && !setmode && cardSupported) {
        // ERASE mode
        // ERASE and Init a New Card by overwriting BlockA1 with keyword "Invalid"
        if (sernum == 0xd01bf755 || sernum == 0xEB338645 || sernum == 0x6EF10423) {
            Serial.println("READ/WRITE/ERASE card cannot be erased!");
            action =0;
            setmode = 0;
        } else {
            // Writing new value block A
            // Scrivo i valori per il settore A
            byte value1Block[] = { 'I', 'n', 'v', 'a', 'l', 'i', 'd', 0, 0, 0, 0, 0, 0, 0, 0, 0,   valueBlockA,~valueBlockA,valueBlockA,~valueBlockA };
            status = mfrc522.MIFARE_Write(valueBlockA, value1Block, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print("MIFARE_Write() failed: ");
                Serial.println(mfrc522.GetStatusCodeName(status));
            } else {
                display_string_5x7c(4,1,"Card deactivated.");
            }
            action = 0;
        }
    } else if (action == 2 && !setmode && cardSupported) {
        // WRITE mode
        // Overwrite Block1A with the string "Hypatia Tam 2002"
        if (sernum == 0xd01bf755 || sernum == 0xEB338645 || sernum == 0x6EF10423) {
            display_string_5x7c(4,1,"READ/WRITE card cannot be written!");
            action =0;
            setmode = 0;
        } else {
            request.ip = {10,0,1,253};
            request.port = 8888;
            request.path = "/name";
            http.get(request, response, headers);
            int ll = response.body.length()>15?15:response.body.length();
            response.body.toCharArray((char *)buffer,sizeof(buffer));
            char *rbody = URLEncode((char *)buffer);
            strncpy((char *)b1, rbody, 16);
            char *lp = strrchr((char *)b1, '%');
            if (lp) *lp = 0; // we force termination on the last '%xx' found
            b1[15]=0; // ensure proper string terminiation

            request.ip = {10,0,1,253};
            request.port = 8888;
            request.path = "/trackid";
            http.get(request, response, headers);
            Serial.println(response.body);
            ll = response.body.length()>13?13:response.body.length();
            response.body.toCharArray((char *)(b2+2),16);
            b2[ll+2] = 0;
            b2[0]=0xbe; b2[1]=0xef; // Signify track ID
            Serial.print("Name: "); Serial.println((char *)b1);
            Serial.print("ID: "); Serial.println((char *)(b2+2));
            
            // Writing new value block A
            // Scrivo i valori per il settore A
            byte value1Block[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,   valueBlockA,~valueBlockA,valueBlockA,~valueBlockA };
            for (int ii=0; ii<16; ii++) {
                value1Block[ii] = b1[ii];
            }
            status = mfrc522.MIFARE_Write(valueBlockA, value1Block, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print("MIFARE_Write() failed: ");
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            action = 0;
            byte value2Block[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  valueBlockB,~valueBlockB,valueBlockB,~valueBlockB };
            for (int ii=0; ii<16; ii++) {
                value2Block[ii] = b2[ii];
            }
            status = mfrc522.MIFARE_Write(valueBlockB, value2Block, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print("MIFARE_Write() failed: ");
                Serial.println(mfrc522.GetStatusCodeName(status));
            } else {
                display_string_5x7c(2,1,(const char*)b1);
                display_string_5x7c(3,1,(const char *)(b2+2));
                display_string_5x7c(4,1,"New card is ready");
            }
            
            /*
             
             // Writing new value block B
             // Scrivo i valori per il settore B
             Serial.println("Writing new value block B");
             byte value2Block[] = { 255,255,255,255,  0,0,0,0, 0,0,0,0,  255,255,255,255,   valueBlockB,~valueBlockB,valueBlockB,~valueBlockB };
             status = mfrc522.MIFARE_Write(valueBlockB, value2Block, 16);
             if (status != MFRC522::STATUS_OK) {
             Serial.print("MIFARE_Write() failed: ");
             Serial.println(mfrc522.GetStatusCodeName(status));
             }
             
             // Writing new value block D
             // Scrivo i valori per il settore C
             Serial.println("Writing new value block C");
             byte value3Block[] = { 255,255,255,255,  0,0,0,0, 0,0,0,0,  255,255,255,255,   valueBlockC,~valueBlockC,valueBlockC,~valueBlockC };
             status = mfrc522.MIFARE_Write(valueBlockC, value3Block, 16);
             if (status != MFRC522::STATUS_OK) {
             Serial.print("MIFARE_Write() failed: ");
             Serial.println(mfrc522.GetStatusCodeName(status));
             }
             
             */
            
        }
    } else if (action == 1 && !setmode && cardSupported) {
        // READ mode

        action = 0;
    } else if (action == 0 && setmode == 0) {
        // Read value stored in Block1A and compare to the string "Hypatia Tam 2002"
        // Display welcome message if match, otherwise say not recognized
        byte size = sizeof(c1);
        // change this: valueBlockA , for read anather block
        // cambiate valueBlockA per leggere un altro blocco
        status = mfrc522.MIFARE_Read(valueBlockA, c1, &size);
        c1[15] = 0;
        status = mfrc522.MIFARE_Read(valueBlockB, c2, &size);
        c2[15] = 0;
        
        if ((c2[0] != 0xbe) && (c2[1] != 0xef)) {
            Serial.print("Card has: "); Serial.println((char *)c1);
            Serial.print("and: "); Serial.println((char *)c2);
            sprintf(dispCS, "%0lx", sernum);
            display_string_5x7c(3,1,dispCS);
            display_string_5x7c(4,1,"Card not valid");
        } else {
            sprintf(msg, "%s", (char *)c1);
            display_string_5x7c(3,1,msg);
            sprintf(msg, "[%s]", (char *)(c2+2));
            display_string_5x7c(4,1,msg);
            
            char cmd[128];
            snprintf(cmd, 128, "/play?name=%s&trackid=%s", (char *)c1, (char *)(c2+2));
            cmd[127]=0;
            Serial.print("cmd: "); Serial.println(cmd);
            String scmd = String(cmd);
            request.ip = {10,0,1,253};
            request.port = 8888;
            request.path = scmd;
            http.get(request, response, headers);

        }
         stimer = 0; tinc = 1;
    }
    // risponde successo
    //Serial.println(mfrc522.GetStatusCodeName(status));
    
    // Dump the result
    //mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    
}
