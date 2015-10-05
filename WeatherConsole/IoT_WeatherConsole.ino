// This #include statement was automatically added by the Particle IDE.
#include "application.h"
#include "MQTT/MQTT.h"

#define version 100

#define CLIENT_NAME "envconsole01"

#define TP_HOME "sensornet/env/home/#"
#define TP_HOME_ALARM "sensornet/status/#"
#define TP_COMMAND "sensornet/command/envconsole01/#"
#define TP_LIVTEMP  "/living/temperature"
#define TP_LIVHUMI  "/living/humidity"
#define TP_LIVMOTION  "/living/motion"
#define TP_BALTEMP  "/balcony/temperature"
#define TP_BALHUMI  "/balcony/humidity"
#define TP_STATUS   "sensornet/status/envconsole01"
#define ARMED    "ARMED"
#define DISARMED   "DISARMED"
#define CMD_DISP_OFF "display/off"
#define CMD_DISP_ON "display/on"
 
#define HBLED D7                // Blinking HeartBeat LED
#define BEEPER D0               // Beeper
#define ERRFLASHTIME 100        // How fast do I blink when error
#define HBFLASHTIME 250         // How fast do I blink normally
#define RGBLEDRELASETIME 3500   // 3.5s to release control of RBG LED
#define LCDRELASETIME 15000     // 15s to release LCD

#define RECONNECT 10000         // Try reconnect every 10s

#define LCD_TEXT 1
#define LCD_VALUE 2
#define LCD_PICT 3
#define LCD_PAGESELECT 4
#define LCD_FONTCOLOR 5
#define LCD_DIRECT 99

#define BASEPICT "p0"
#define OUTDOORTEMPFIELD "t0"
#define OUTDOORHUMIFIELD "t1"
#define INDOORTEMPFIELD "t2"
#define INDOORHUMIFIELD "t3"
#define STATUSFIELD "t4"
#define CLOCKFIELD "t5"
#define DATEFIELD "t6"
#define ALARMFIELD "t7"

String lcdElems[] = {OUTDOORTEMPFIELD, OUTDOORHUMIFIELD, 
                    STATUSFIELD, CLOCKFIELD, DATEFIELD, ALARMFIELD, BASEPICT};
int forecast[] = {0,1,2,3,4,5};
int timpicts[] = {3,5,2,4};
int lasttimepict = 99;

void callback(char* topic, byte* payload, unsigned int length);

// JSON Parser

// MQTT client("stoy.iostation.com", 1883, callback);
byte server[] = { 10,0,1,250 };
MQTT client(server, 1883, callback);

// Various MQTT topics of interest
String tp_livtemp = TP_LIVTEMP;
String tp_livhumi = TP_LIVHUMI;
String tp_baltemp = TP_BALTEMP;
String tp_balhumi = TP_BALHUMI;
String tp_status = TP_STATUS;

String livtemp,livhumi,baltemp,balhumi,sstatus="-";

// timers
unsigned int ledTimer, lcdTimer, hbTimer, lastConnect;
bool blink;
bool connected=false;
bool armed=false;

// received command
char cmd[8];

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    char mesg[128];
    String m;
    String tp = String(topic);
    
    m.reserve(10);
    memcpy(p, payload, length);
    p[length] = NULL;
    m = String(p);
    if (tp.endsWith(TP_LIVTEMP)) {
        // Living room temperature
        m.concat("C");
        livtemp = m;
    } else if (tp.endsWith(TP_LIVHUMI)) {
        // Living room temperature
        m.concat("%");
        livhumi = m;
    } else if (tp.endsWith(TP_BALHUMI)) {
        // Living room temperature
        m.concat("%rH");
        balhumi = m;
    } else if (tp.endsWith(TP_BALTEMP)) {
        // Living room temperature
        m.concat("C");
        baltemp = m;
    } else if (tp.startsWith("sensornet/status/")) {
        if (m.equals(ARMED)) {
            armed = true;
            digitalWrite(BEEPER, LOW);
            delay (10);
            digitalWrite(BEEPER, HIGH);
        } else if (m.equals(DISARMED)) {
            armed = false;
            digitalWrite(BEEPER, LOW);
            delay (10);
            digitalWrite(BEEPER, HIGH);
            delay (300);
            digitalWrite(BEEPER, LOW);
            delay (10);
            digitalWrite(BEEPER, HIGH);
        }
    } else if (tp.startsWith("sensornet/command/envconsole01/")) {
        if (tp.endsWith(CMD_DISP_OFF)) {
            lcdOff();
        } else if (tp.endsWith(CMD_DISP_ON)) {
            lcdOn();
        }
    }
    updateAll();
}

void updateAll()
{
    // updateBkgPict();
    sendToLCD(LCD_TEXT,INDOORTEMPFIELD,livtemp);
    sendToLCD(LCD_TEXT,INDOORHUMIFIELD,livhumi);
    sendToLCD(LCD_TEXT,OUTDOORHUMIFIELD,balhumi);
    sendToLCD(LCD_TEXT,OUTDOORTEMPFIELD,baltemp);
    sendToLCD(LCD_TEXT,STATUSFIELD,sstatus);
    if (armed)
        sendToLCD(LCD_TEXT,ALARMFIELD,"ARM");
    else
        sendToLCD(LCD_TEXT,ALARMFIELD,"   ");
}

void changePict(uint8_t pict)
{
    String s;
    
    for (int i=0; i<arraySize(lcdElems); i++) {
        s = lcdElems[i];
        Serial1.print(s);
        if (s.startsWith("p")) {    // p0.pic=0
            Serial1.print(".pic=");
        } else {                    // t0.picc=0
            Serial1.print(".picc=");
        }
        Serial1.print(pict);
	    Serial1.write(0xff);
	    Serial1.write(0xff);
    	Serial1.write(0xff);
	
	    delay(50);

        Serial1.flush();
	    while(Serial1.available() > 0)
    	{
	    	char ch = Serial1.read();
    	}
    }
}

void sendToLCD(uint8_t type,String index, String cmd)
{
	if (type == LCD_TEXT ){
		Serial1.print(index);
		Serial1.print(".txt=");
		Serial1.print("\"");
		Serial1.print(cmd);
		Serial1.print("\"");
	}
	else if (type == LCD_VALUE){
		Serial1.print(index);
		Serial1.print(".val=");
		Serial1.print(cmd);
	}
	else if (type == LCD_PICT){
		Serial1.print(index);
		Serial1.print(".picc="); 
		Serial1.print(cmd);
	}
	else if (type == LCD_PAGESELECT ){
		Serial1.print("page ");
		Serial1.print(cmd);
	} else if (type == LCD_FONTCOLOR){
		Serial1.print(index);
		Serial1.print(".pco="); 
		Serial1.print(cmd);
	} else if (type == LCD_DIRECT){         // 99 pass over the command
	    Serial1.print(cmd);
	}
	
	Serial1.write(0xff);
	Serial1.write(0xff);
	Serial1.write(0xff);
	
	delay(50);

    Serial1.flush();
	while(Serial1.available() > 0)
	{
		char ch = Serial1.read();
	}
}

void lcdOn() {
    sendToLCD(LCD_DIRECT,"","sleep=0");
}

void lcdOff() {
    sendToLCD(LCD_DIRECT,"","sleep=1");
}

void setMqtt(void)
{
    client.publish(TP_STATUS,"ready");
    client.subscribe(TP_HOME);
    client.subscribe(TP_HOME_ALARM);
    client.subscribe(TP_COMMAND);
}

bool updateBkgPict(void)
{
    bool updated = false;
    int th = Time.hour();
    if (th>20 || th<6) {    // Night
        if (lasttimepict != timpicts[3]) {
            changePict(timpicts[3]);
            sendToLCD(LCD_FONTCOLOR,OUTDOORTEMPFIELD,"65535");
            sendToLCD(LCD_FONTCOLOR,OUTDOORHUMIFIELD,"65535");
            sendToLCD(LCD_FONTCOLOR,CLOCKFIELD,"64487");
            sendToLCD(LCD_FONTCOLOR,DATEFIELD,"64487");
            lasttimepict = timpicts[3];
            updated= true;
        }
    } else if (th > 15 && th <= 20) {
        if (lasttimepict != timpicts[2]) {
            changePict(timpicts[2]);
            lasttimepict = timpicts[2];
            sendToLCD(LCD_FONTCOLOR,OUTDOORTEMPFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,OUTDOORHUMIFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,CLOCKFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,DATEFIELD,"0");
            updated= true;
        }
    } else if (th >= 12 && th <= 15) {
        if (lasttimepict != timpicts[1]) {
            changePict(timpicts[1]);
            sendToLCD(LCD_FONTCOLOR,OUTDOORTEMPFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,OUTDOORHUMIFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,CLOCKFIELD,"64487");
            sendToLCD(LCD_FONTCOLOR,DATEFIELD,"64487");
            lasttimepict = timpicts[1];
            updated= true;
        }
    } else {
        if (lasttimepict != timpicts[0]) {
            changePict(timpicts[0]);
            sendToLCD(LCD_FONTCOLOR,OUTDOORTEMPFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,OUTDOORHUMIFIELD,"0");
            sendToLCD(LCD_FONTCOLOR,CLOCKFIELD,"64487");
            sendToLCD(LCD_FONTCOLOR,DATEFIELD,"64487");
            lasttimepict = timpicts[0];
            updated= true;
        }
    }
    if (updated) sendToLCD(LCD_PICT,"q0","8");
    return updated;
}

void setup() 
{
    Time.zone(+8);
    Serial1.begin(9600);
    delay(50);
    sendToLCD(LCD_TEXT,STATUSFIELD,"Initializing");
    WiFi.on();
    delay(500);
    WiFi.connect();
    delay(500);
    pinMode(HBLED, OUTPUT);
    pinMode(BEEPER, OUTPUT);
    digitalWrite(BEEPER, HIGH);
    
    // connect to the server
    client.connect(CLIENT_NAME);

    // publish/subscribe
    if (client.isConnected()) {
        connected=true;
        setMqtt();
        sendToLCD(LCD_TEXT,STATUSFIELD,"Connected");
    } else {
        sendToLCD(LCD_TEXT,STATUSFIELD,"Not connected");
    }
    lcdTimer = millis();
}




void loop() 
{
    char cTime[10];

    // Maintain connection
    if (client.isConnected()) {
        client.loop();
        connected = true;
        sstatus = "Connected";
    } else {
        connected = false;
        sstatus = "Not connected";
        if (millis() - lastConnect > RECONNECT) {
            // connect to the server
            client.connect(CLIENT_NAME);
            lastConnect = millis();
            if (client.isConnected()) {
                setMqtt();
            } 
        }
    }

    if (millis() - ledTimer > RGBLEDRELASETIME) {
        ledTimer = millis();
        RGB.control(false);
    }

    if (millis() - hbTimer > (connected?HBFLASHTIME:ERRFLASHTIME)) {
        if (updateBkgPict()) updateAll();
        sprintf(cTime, "%02d:%02d:%02d", Time.hour(), Time.minute(), Time.second());
        sendToLCD(LCD_TEXT, CLOCKFIELD, String(cTime));
        sendToLCD(LCD_TEXT, DATEFIELD, Time.format(Time.now(),"%h %d"));
        hbTimer = millis();
        blink = !blink;
        if (blink) {
            digitalWrite(HBLED, HIGH);
        } else {
            digitalWrite(HBLED, LOW);
        }
        
    }
}


