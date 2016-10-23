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
#include "MQTT/MQTT.h"
#include "si7021.h"

//#define SEMI_AUTOMATIC


#define CLIENT_NAME "AQI01"
#define NOWIFITIMEOUT 30000
#define PERIOD 900

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


void callback(char* topic, byte* payload, unsigned int length);

//byte server[] = { 10,175,151,107 };
byte server[] = { 10,0,1,250 };
MQTT client(server, 1883, callback);
si7021 thsensor;

// timers
unsigned long ledTimer, d7Timer;
unsigned long lastConnect;
int d7Rate = 250;

// flags
bool d7On = false;
bool init = true;

// received command
char cmd[8];

void callback(char* topic, byte* payload, unsigned int length) {
}

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

void fanOn() {
    digitalWrite(FAN, HIGH);
}

void fanOff() {
    digitalWrite(FAN, LOW);
}

void measureTemperature() {
    float tp, rh = 0.0;
    char sv[128];

    // Measure RH
    rh = round(thsensor.getRH()*100.0)/100.0;
    // Measure Temperature
    tp = round(thsensor.getTemp()*100.0)/100.0;
    sprintf(sv, "{\"temperature\":%.2f,\"humidity\":%.2f}", tp, rh);
    client.publish("sensornet/aqi/AQI01/temperature",sv);
}

void measureAQI(bool slow) {
    int v, aqi = 0;
    char sv[128];
    float pm25, tp, rh = 0.0;
    
    if (slow) {
        v = slowReadSensor();
    } else {
        v = readSensor();
    }
    // Measure RH
    rh = round(thsensor.getRH()*100.0)/100.0;
    // Measure Temperature
    tp = round(thsensor.getTemp()*100.0)/100.0;
    pm25 = vToPM25(v, tp, rh);
    aqi = calculateAirQualityIndex(pm25);
    sprintf(sv, "{\"value\":%d,\"pm25\":%.3f,\"aqi\":%d,\"temperature\":%.2f,\"humidity\":%.2f}", v, pm25, aqi, tp, rh);
    client.publish("sensornet/aqi/AQI01/allvalues",sv);
}


void calAQI() {
    int v;
    char sv[8];
    v = slowReadSensor();   
#ifdef TESTING
    Serial.print("Calibration: ");
    Serial.println(v);
#endif
    sprintf(sv, "%d", v);
    client.publish("sensornet/aqi/AQI01/calvalues",sv);
}

void setMqtt()
{
    if (client.isConnected()) {
        client.publish("sensornet/status/AQI01","ready");
        RGB.control(true);
        RGB.color(0,255,0);
        ledTimer = millis();
    }
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
    Time.zone(+8);
    pinMode(FAN, OUTPUT);
    pinMode(VLED, OUTPUT);
    digitalWrite(VLED, HIGH);
    WiFi.on();
    WiFi.connect();
    waitFor(WiFi.ready, NOWIFITIMEOUT);

    fanOff();
    setADCSampleTime(ADC_SampleTime_3Cycles);
    thsensor.begin();
    client.connect(CLIENT_NAME);
    delay(500);
    setMqtt();
    lastConnect = millis();
}




void loop()
{
    if (client.isConnected()) {
        fanOn();
        delay(7000);
        measureTemperature();
        measureAQI(true);
        fanOff();
        System.sleep(SLEEP_MODE_DEEP, PERIOD);
    } else {
        client.connect(CLIENT_NAME);
        delay(500);
        setMqtt();
        if (millis() - lastConnect > NOWIFITIMEOUT) {
            // WiFi no good, next time
            System.sleep(SLEEP_MODE_DEEP, PERIOD);
        }
    }
    delay(5000);
}
