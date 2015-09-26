// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

/****************************************************************************
*   Upload temperature reading from a TMP36 sensor to Thingspeak            *
*   and display on a local OLED display                                     *
****************************************************************************/

#define PERIOD 600000   // Sleep for 300s

int version=108;

int Rm = 10000;

int led = D7;
int tempSensor = A7;
int pressureSensor = A3;
int soundSensor = D2;
int Enabler = D6;
int PIR = D3;

// Create a variable that will store the temperature value
int temperature = 0;
int pressure = 0;
int IpP = 0;
int IpT = 0;
int weight = 0;
int read = 0;
bool human, sound;
int enabled;
int motion = 0;
int noise = 0;
int accumotion = 0;
int accunoise = 0;
int timers[5];
const char * WRITEKEY = "94R8F61EKX9BXNGQ";
const char * DID_WL = "vF86B705D61B0EE3&message=Intruder+Alert%21";
TCPClient client;
const char * serverNameTSpeak = "api.thingspeak.com";
const char * serverNamePBox = "api.pushingbox.com";   // PushingBox API URL
IPAddress serverIPTSpeak = {184,106,153,149};
IPAddress serverIPPBox = {213,186,33,19};

// 1.3" 12864 OLED with SH1106 and Simplified Chinese IC for Spark Core

extern char* itoa(int a, char* buffer, unsigned char radix);

int Rom_CS = A2;

bool cursor = true;
int lastTime = 0;

char dispCS[256];

String U2GB_BASE_URL="util/conv.php?str=";

// MQTT client("stoy.iostation.com", 1883, callback);
byte server[] = { 10,0,1,252 };
MQTT mqttclient(server, 1883, callback);

// OLED control


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



/*从相关地址（addrHigh：地址高字节,addrMid：地址中字节,addrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址*/
/*连续读取*/
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


unsigned long  fontaddr=0;
void display_GB2312_string(byte y,byte x,const char *text)
{
  byte i= 0;
  byte addrHigh,addrMid,addrLow ;
  byte fontbuf[32];     
  while((text[i]>0x00))
  {
    if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
    {           
      /*国标简体（GB2312）汉字在字库IC中的地址由以下公式来计算：*/
      /*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
      /*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
      fontaddr = (text[i]- 0xb0)*94; 
      fontaddr += (text[i+1]-0xa1)+846;
      fontaddr = (unsigned long)(fontaddr*32);
      
      addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
      addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
      addrLow = fontaddr&0xff;       /*地址的低8位,共24位*/
      get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
      display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
      i+=2;
      x+=16;
    }
    else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
    {           
      /*国标简体（GB2312）15x16点的字符在字库IC中的地址由以下公式来计算：*/
      /*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
      /*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
      fontaddr = (text[i]- 0xa1)*94; 
      fontaddr += (text[i+1]-0xa1);
      fontaddr = (unsigned long)(fontaddr*32);
      
      addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
      addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
      addrLow = fontaddr&0xff;       /*地址的低8位,共24位*/
      get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
      display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
      i+=2;
      x+=16;
    }
    else if((text[i]>=0x20) &&(text[i]<=0x7e))  
    {           
      unsigned char fontbuf[16];      
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr*16);
      fontaddr = (unsigned long)(fontaddr+0x3cf80);     
      addrHigh = (fontaddr&0xff0000)>>16;
      addrMid = (fontaddr&0xff00)>>8;
      addrLow = fontaddr&0xff;

      get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );/*取16个字节的数据，存到"fontbuf[32]"*/
      
      display_graphic_8x16(y,x,fontbuf);/*显示8x16的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
      i+=1;
      x+=8;
    }
    else
      i++;  
  }
  
}

void display_string_5x7(byte y,byte x,const char *text)
{
  unsigned char i= 0;
  unsigned char addrHigh,addrMid,addrLow ;
  while((text[i]>0x00))
  {
    
    if((text[i]>=0x20) &&(text[i]<=0x7e)) 
    {           
      unsigned char fontbuf[8];     
      fontaddr = (text[i]- 0x20);
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
    else
    i++;  
  }
  
}


// Send to thingspeak

void sendToThingSpeak(const char * key, String mesg)
{
    client.stop();
    String outMesg = String("field1="+mesg+"&field2="+motion+"&field3="+noise);
    RGB.control(true);
    RGB.color(255,255,0);
    if (client.connect(serverIPTSpeak, 80)) {
        RGB.color(0,255,0);
        client.print("POST /update");
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverNameTSpeak);
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

void sendToPushingBox(const char * devid, String mesg, String mesg2)
{
    client.stop();
    String outMesg = String("+%28M/A:"+mesg+"/"+mesg2+"%29");
    RGB.control(true);
    RGB.color(255,255,0);
    if (client.connect(serverIPPBox, 80)) {
         RGB.color(0,255,0);
        client.print("GET /pushingbox?devid=");
        client.print(DID_WL);
        client.print(outMesg);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverNamePBox);
        client.println("User-Agent: Spark");
        client.println("Connection: close");
        client.println();
        client.flush();
        RGB.control(false);
    } 
    else{
        RGB.color(255,0,0);
    }
}


// The returned value from the Core is going to be in the range from 0 to 4095.
// You can easily convert this value to actual temperature reading by using the following formula:
// voltage = (sensor reading x 3.3)/4095
// Temperature (in Celsius) = (voltage - 0.5) X 100

int readSensors(String command)
{
      int Rf, vP, vT;
      
      IpT = analogRead(tempSensor) ;
      IpP = analogRead(pressureSensor);
      vT = IpT * 3300 / 4095;
      temperature = (vT - 500); // 10 times, actual temperature need to /10
      vP = IpP * 330 / 4095;
      if (vP != 0) {
        Rf = Rm * 330 / vP - Rm;
      } else {
          Rf = 0;
      }
      weight = (Rf - 2500) * 1000 / 11111 + 100; // Rf = 11.11*(weight - 100) + 2500
      if (command == "V") {
          return vT;
      }
      if (command == "T") {
        return temperature;
      }
      if (command == "P") {
          return vP;
      }
      if (command = "W") {
          return weight;
      }
      return 0;
}

int myVersion(String command)
{
    return version;
}

void soundDetected()
{
    sound = true;
}

void humanDetected()
{
    human = true;
}

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {

    char p[length + 1];
    unsigned short r,g,b;
    char mesg[128];
//    StaticJsonBuffer<200> jsonBuffer;

    memcpy(p, payload, length);
    p[length] = NULL;
    
    sprintf(mesg, "{\"Location\":\"Living Room\",\"Value\":%d}", accumotion);
    if (mqttclient.isConnected())
        mqttclient.publish("/sensornet/env/home/motion",mesg);
    sprintf(mesg, "{\"Location\":\"Living Room\",\"Value\":%d}", noise);
    if (mqttclient.isConnected())
        mqttclient.publish("/sensornet/env/home/sound",mesg);
    sprintf(mesg, "{\"Location\":\"Living Room\",\"Value\":%3.1f}", temperature/10.0);
    if (mqttclient.isConnected())
        mqttclient.publish("/sensornet/env/home/temperature",mesg);
    
/*
// Old way of doing it. Now use JSON {"Command":"COLOR","Color":[255,255,255]}
 
    JsonObject& jroot = jsonBuffer.parseObject(p);  // WILL FAIL if declare this global
    if (jroot.success()) {
        const char *cmd = jroot["Command"];
        r = jroot["Color"][0];
        g = jroot["Color"][1];
        b = jroot["Color"][2];
        String message(cmd);
        RGB.control(true);
        RGB.color(r,g,b);
    
        if (message.equals("ON"))
            switchOn();
        else if (message.equals("OFF"))
            switchOff();
        else if (message.equals("COLOR"))
            colorAll(r, g, b);
        sprintf(mesg, "{\"Command\":\"%s\",\"Color\":\"%d,%d,%d\"}", cmd, r, g, b);
        if (client.isConnected()) {
            client.publish("/sensornet/echo",mesg);
        }
    } else {
        Serial.println("JSON parse failed");
    }
 */
 
}

void setup()
{
    Time.zone(8);
  // Register a Spark variable here
    Spark.variable("temperature", &temperature, INT);
    Spark.variable("version", &version, INT);
    Spark.variable("pressure", &pressure, INT);
    Spark.function("readSensor", readSensors);
    Spark.function("version", myVersion);
  // Connect the temperature sensor to A7 and configure it
  // to be an input
    pinMode(tempSensor, INPUT);
    pinMode(pressureSensor, INPUT);
    pinMode(led, OUTPUT);
    pinMode(soundSensor, INPUT);
    pinMode(PIR, INPUT);
    pinMode(Enabler, INPUT);
 
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    digitalWrite(Rom_CS, HIGH);
    attachInterrupt(soundSensor, soundDetected, RISING);
    attachInterrupt(PIR, humanDetected, RISING);

    initial_lcd();  
    clear_screen();    //clear all dots
    digitalWrite(led, LOW);
    display_string_5x7(4,1,"(c)2014 ioStation");
    sprintf(dispCS, "Version: %d", version);
    display_string_5x7(6,1,dispCS);
    
    
    // MQTT publish/subscribe
    mqttclient.connect("MultiSensor");
    if (mqttclient.isConnected()) {
        mqttclient.publish("/sensornet/status","ready");
        mqttclient.subscribe("/sensornet/command/dump");
    }


    delay(1000);
    clear_screen();    //clear all dots
    lastTime = millis();
}

void loop()
{
        char msg[128];
    
        if (mqttclient.isConnected())
            mqttclient.loop();
        
        digitalWrite(led, HIGH);
        enabled = digitalRead(Enabler);
//        clear_screen();    //clear all dots
        int curTemp = readSensors("T");
        float fcT = (float)(curTemp / 10.0f);
        String cT = String(curTemp/10);
        cT.concat(".");
        cT.concat(String(curTemp % 10));
        sprintf(dispCS, "%0.1f degC   ", fcT);
        noInterrupts();
//        display_string_5x7(1,1,   "Current temperature");
        display_GB2312_string(2,1, dispCS);
  // Keep reading the temperature so when we make an API
  // call to read its value, we have the latest one
        if ((millis() - lastTime > PERIOD) && WiFi.ready()) {
          display_string_5x7(1,1, "Updating            ");
          sprintf(msg, "{\"Location\":\"Living Room\",\"Value\":%3.1f}", temperature/10.0);
          if (mqttclient.isConnected())
            mqttclient.publish("/sensornet/env/home/temperature",msg);
          sendToThingSpeak(WRITEKEY, cT);
          lastTime = millis();
          motion = 0;
          noise = 0;
          //interrupts();
          //Spark.sleep(PERIOD);
        }
        if (human) {
          human = !human;
          motion++;
          accumotion++;
          sprintf(msg, "{\"Location\":\"Living Room\",\"Value\":%d}", accumotion);
          if (mqttclient.isConnected())
            mqttclient.publish("/sensornet/env/home/motion",msg);
          timers[1] = millis();
          display_string_5x7(6,1,"I see you!");
        }
        if (sound) {
          sound = !sound;
          noise++;
          sprintf(msg, "{\"Location\":\"Living Room\",\"Value\":%d}", noise);
          if (mqttclient.isConnected())
            mqttclient.publish("/sensornet/env/home/sound",msg);
          timers[2] = millis();
          display_string_5x7(7,1,"I heard you!");
        }
        if ((accumotion > 2) && enabled) {
            Spark.publish("home-motion", "HOME", 60, PRIVATE);
            sprintf(msg, "{\"Sensor\":\"Motion\",\"Value\":%d}", accumotion);
            if (mqttclient.isConnected())
                mqttclient.publish("/sensornet/alarm",msg);
            sendToPushingBox(DID_WL, Time.timeStr().substring(4,19), String("-"));
            accumotion = 0;
        }
        Time.timeStr().substring(4,19).toCharArray(dispCS, 16);
        display_string_5x7(1,1,"     ");
        display_string_5x7(1,20,dispCS);
        if (millis() - timers[1] > 2000)
            display_string_5x7(6,1,"                ");
        if (millis() - timers[2] > 2000)
            display_string_5x7(7,1,"                ");
        sprintf(dispCS, "%d", (millis()-lastTime)/1000);
        display_string_5x7(7,1,dispCS);
        interrupts();
        // flash LED
        digitalWrite(led, LOW);
        delay(500);
//        Spark.sleep(SLEEP_MODE_DEEP, PERIOD);
}

