/*
 * This is copyrighted materials by ioStation Ltd.
 * All rights reserved.
 *
 * 3PP library used here are subject to their individual license agreement.
 * The source code below is only for ioStation internal use and must not
 * be redistributed, published partly or whole, copied and reused partly
 * or whole under any circumstance for purpose other than ioStation Internal
 * Demonstration to ioStation employees.
 *
 */

// This #include statement was automatically added by the Particle IDE.

#include "application.h"
#include "LM61.h"
#include "si7021.h"

//#define SEMI_AUTOMATIC


#define CLIENT_NAME "AQI01"
#define NOWIFITIMEOUT 30000
#define PERIOD 900

#define SIPOWER D2
#define FAN D3
#define VLED D4
#define Vo  A0

#define VREF 428

#define SEMI_AUTOMATIC
#undef TESTING

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

const float concentrationBoundaries[7][2] =
{
        {0.0f,12.0f},
        {12.1f,35.4f},
        {35.5f,55.4f},
        {55.5f,150.4f},
        {150.5f,250.4f},
        {250.5f,350.4f},
        {350.5f,500.4f}
};

const unsigned int indexBoundaries[7][2] =
{
        {0,50},
        {51,100},
        {101,150},
        {151,200},
        {201,300},
        {301,400},
        {401,500}
};


si7021 thsensor;
LM61 lm61 = LM61(0, 0);

// timers
unsigned long ledTimer, d7Timer;
unsigned long lastConnect;
int d7Rate = 250;

// flags
bool d7On = false;
bool init = true;

// received command
char cmd[8];

char server[] = "ag.iostation.com";
uint16_t port = 12345;
char jsonStr[255];

/*
// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    char mesg[128];

#ifdef TESTING
    Serial.println("Callback");
#endif
    memcpy(p, payload, length);
    p[length] = NULL;
    String stopic = String(topic);
    if (stopic.endsWith("/fanon")) {
        fanOn();
    } else if (stopic.endsWith("/fanoff")) {
        fanOff();
    } else if (stopic.endsWith("/fastmeasure")) {
        measureAQI(false);
    } else if (stopic.endsWith("/measure")) {
        measureAQI(true);
    } else if (stopic.endsWith("/calibrate")) {
        calAQI();
    } else if (stopic.endsWith("/temperature")) {
        measureTemperature();
    }

}
*/

#ifdef HMILCD
void sendToLCD(uint8_t type,String index, String cmd)
{
	if (type == 1 ){
		Serial1.print(index);
		Serial1.print(".txt=");
		Serial1.print("\"");
		Serial1.print(cmd);
		Serial1.print("\"");
	}
	else if (type == 2){
		Serial1.print(index);
		Serial1.print(".val=");
		Serial1.print(cmd);
	}
	else if (type == 3){
		Serial1.print(index);
		Serial1.print(".picc=");
		Serial1.print(cmd);
	}
	else if (type ==4 ){
		Serial1.print("page ");
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
#endif

String URLEncode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    String encodedMsg = "";

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}

void lm61_init(void)
{
    Serial.print("DEBUG: Initializing LM61...");
    lm61.begin();
    if (lm61.init()) {
        Serial.println("OK");
    } else {
        Serial.println("Failed");
    }
}

void lm61_close(void)
{
    Serial.print("DEBUG: Closing LM61...");
    lm61.end();
    Serial.println("OK");
}

void lm61_send(char *sendStr)
{
    Serial.printf("DEBUG: %s", sendStr);
    Serial.println();
    
    if (lm61.socketSend(sendStr)) {
        Serial.println("DEBUG: Sent");
    }else {
        Serial.println("DEBUG: Send failed");
    }
}


void fanOn() {
    digitalWrite(FAN, HIGH);
}

void fanOff() {
    digitalWrite(FAN, LOW);
}


void measureAQI(bool slow) {
    int v, aqi = 0;
    char sv[384];
    float pm25, tp, rh = 0.0;
    char buf[1024];
    
    digitalWrite(SIPOWER, 1);
    pinMode(SIPOWER, OUTPUT);
    digitalWrite(SIPOWER, 1);
    if (slow) {
        v = slowReadSensor();
    } else {
        v = readSensor();
    }
    delay(100);
    // Measure RH
    rh = round(thsensor.getRH()*100.0)/100.0;
    // Measure Temperature
    tp = round(thsensor.getTemp()*100.0)/100.0;
    pinMode(SIPOWER, OUTPUT);
    pm25 = vToPM25(v, tp, rh);
    aqi = calculateAirQualityIndex(pm25);
    sprintf(sv, "{\"adcraw\":%d,\"pm25\":%.3f,\"aqi\":%d,\"temperature\":%.2f,\"humidity\":%.2f}", v, pm25, aqi, tp, rh);
    
    String s = URLEncode((const char *)sv);
    s.toCharArray(buf, 1023);

    lm61_send(buf);
}


void calAQI() {
    int v;
    char sv[8];
    v = slowReadSensor();   
#ifdef TESTING
    Serial.print("Calibration: ");
    Serial.println(v);
#endif
    sprintf(sv, "{\"calibration\":%d}", v);
    lm61_send(sv);
}


float vToPM25(int vo, float tp, float rh) {
    float dv, vr, pm25 = 0.0;
    float Vref = 0.0;
    
    Vref = VREF/4.095*0.917;     //When Vcc is connected, A0 has 3.6V x 0.917 = 3.3V calib
    
    vr = (Vref/1.0543)*(0.007143*tp+0.89);  // calib the Vref against temp

    dv = (vo/4.095) - vr;
    dv = (dv < 0)?0:dv;
    if (rh > 50.0) {
        pm25 = 0.6*(1-0.01467*(rh-50))*dv;
    } else {
        pm25 = 0.6*dv;
    }
    return pm25;
}

int readSensor() {
    unsigned long d320us, d10ms, t = 0;
    long v = 0;
    int i = 0;

    setADCSampleTime(ADC_SampleTime_3Cycles);

    for (i=0; i<80; i++) {
        t = micros();
        pinResetFast(VLED);    // turn on LED
        delayMicroseconds(280);
        v += analogRead(Vo);
        d320us = 320 + t - micros();
        delayMicroseconds(d320us);
        pinSetFast(VLED);   // turn off LED
        d10ms = 10000 + t - micros();
        delayMicroseconds(d10ms);
    }
#ifdef TESTING
        Serial.print("d320us = ");
        Serial.println(d320us);
        Serial.print("d10ms = ");
        Serial.println(d10ms);
        
        Serial.print("Average ");
        Serial.println(v/i);
#endif
    return (v/i);
}

int slowReadSensor() {
    unsigned long d320us, d10ms, t = 0;
    long v = 0;
    int i = 0;

    setADCSampleTime(ADC_SampleTime_112Cycles);
    for (i=0; i<80; i++) {
        t = micros();
        pinResetFast(VLED);    // turn on LED
        delayMicroseconds(280);
        v += analogRead(Vo);
        pinSetFast(VLED);   // turn off LED
        d10ms = 10000 + t - micros();
        delayMicroseconds(d10ms);
    }
#ifdef TESTING
        Serial.print("d10ms = ");
        Serial.println(d10ms);
        
        Serial.print("Average ");
        Serial.println(v/i);
#endif
    return (v/i);
}

/* Takes the PM2.5 float data and converts it into the air quality index (AQI)
   in accordance to the US EPA's standards. See the user's guide for more
   information on how this is calculated */
int calculateAirQualityIndex(float pm25)
{
    int iLow, iHigh, ii;
    float cLow, cHigh;

    cLow = 0;
    cHigh = 0;
    iHigh = 0;
    iLow = 0;

    /* Finding out the boundary values and calculating the PM2.5 based off
        that */
    for(ii=0;ii<7;ii++)
    {
        if (concentrationBoundaries[ii][0] <= pm25
                && concentrationBoundaries[ii][1] >= pm25)
        {
            cLow = concentrationBoundaries[ii][0];
            cHigh = concentrationBoundaries[ii][1];
            iLow = indexBoundaries[ii][0];
            iHigh = indexBoundaries[ii][1];
            break;
        }
    }
        
    return (int)(((iHigh - iLow)/(cHigh - cLow)) * (pm25 - cLow)) + iLow;
}


void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);
    Time.zone(+8);
    pinMode(FAN, OUTPUT);
    pinMode(VLED, OUTPUT);
    pinMode(SIPOWER, INPUT);
    digitalWrite(VLED, HIGH);
    WiFi.off();
//    WiFi.connect();
//    waitFor(WiFi.ready, NOWIFITIMEOUT);

    fanOff();
    setADCSampleTime(ADC_SampleTime_3Cycles);
    thsensor.begin();
    delay(1000);
    Serial.println("App ready.");
    lm61_init();
    delay(500);
    lastConnect = millis();
    RGB.control(true);

}




void loop()
{
    RGB.color(0,128,0);
    lm61_init();
    fanOn();
//    delay(7000);
    measureAQI(true);
    fanOff();
    lm61_close();
//    System.sleep(SLEEP_MODE_DEEP, PERIOD);
    RGB.color(0,0,0);
    delay(5000);
}
