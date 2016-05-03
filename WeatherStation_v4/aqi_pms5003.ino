// This #include statement was automatically added by the Particle IDE.
#include "si7021.h"
#include "MQTT/MQTT.h"
SYSTEM_THREAD(ENABLED);

// Air Quality Monitor 1.0
// Copyright 2016 ioStation Ltd.

#define SEMI_AUTOMATIC

#define SETPIN D2
#define RESETPIN D3
#define PACKETLENGTH 32         // 32 bytes of input

#define CLIENT_NAME "AQI02"
#define RECONNECT 15*1000

#define FAST 125
#define SLOW 500


char buf[40];

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
char sv[128];


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

void deviceOn() {
    digitalWrite(RESETPIN, HIGH);
    digitalWrite(SETPIN, HIGH);
    digitalWrite(RESETPIN, LOW);
    delay(10);
    digitalWrite(RESETPIN, HIGH);
    delay(100);
}

void deviceOff() {
    digitalWrite(SETPIN, LOW);
}

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
    if (stopic.endsWith("/deviceon")) {
        deviceOn();
    } else if (stopic.endsWith("/deviceoff")) {
        deviceOff();
    } else if (stopic.endsWith("/measure")) {
        deviceOn();
    }

}

void setMqtt()
{
    if (client.isConnected()) {
        client.publish("sensornet/status/AQI02","ready");
        client.subscribe("sensornet/AQI02/#");
        RGB.control(true);
        RGB.color(0,255,0);
        ledTimer = millis();
        d7Rate = SLOW;
    }
}


bool checkValue(char *thebuf, char leng) {  
    unsigned int receiveSum=0;
    bool receiveflag=false;
    char i=0;
 
    for (i=0; i<leng; i++) {
        receiveSum = receiveSum + thebuf[i];
    }
    
    if (receiveSum == ((thebuf[leng-2]<<8) + thebuf[leng-1] + thebuf[leng-2] + thebuf[leng-1])) {    //check the serial data
        receiveSum = 0;
        receiveflag = true;
    }
    return receiveflag;
}

int readPM01(char *thebuf) {
    int PM01Val;
    PM01Val = ((thebuf[4]<<8) + thebuf[5]); //count PM1.0 value of the air detector module
    return PM01Val;
}
 
//transmit PM Value to PC
int readPM2_5(char *thebuf) {
    int PM2_5Val;
    PM2_5Val = ((thebuf[6]<<8) + thebuf[7]);//count PM2.5 value of the air detector module
    return PM2_5Val;
}
 
//transmit PM Value to PC
int readPM10(char *thebuf)
{
    int PM10Val;
    PM10Val = ((thebuf[8]<<8) + thebuf[9]); //count PM10 value of the air detector module  
    return PM10Val;
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

#ifdef SEMI_AUTOMATIC
SYSTEM_MODE(MANUAL);
#endif

void setup() {
    Time.zone(+8);
    pinMode(SETPIN, OUTPUT);    // LOW = Power save mode
    pinMode(RESETPIN, OUTPUT);  // LOW = Reset
    pinMode(D7, OUTPUT);
    ledTimer = millis();
    d7Timer = millis();
    Serial.begin(9600);
    Serial1.begin(9600);
    deviceOff();
    d7Rate = SLOW;
#ifdef SEMI_AUTOMATIC
    WiFi.on();
    digitalWrite(D7,HIGH);
    delay(500);
    WiFi.connect();
    digitalWrite(D7,LOW);
    delay(500);
    digitalWrite(D7,HIGH);
#endif
    Serial.println("Ready");
}

void loop() {
    int pm01 = 0, pm25 = 0, pm10 = 0;
    int aqi = 0;   
    bool gotReadings = false;
    float rh = 0.0, tp = 0.0;

        // Maintain connection

    if (millis() - ledTimer > 3500) {
        ledTimer = millis();
        RGB.control(false);
        d7Rate = SLOW;
    }
    
    if (WiFi.ready() && init) {
        // connect to the server
        client.connect(CLIENT_NAME);
        delay(500);
        // publish/subscribe
        setMqtt();
        delay(200);
        init = false;
    }
    
    // Flash D7 according to the statue (controlled by d7Rate)
    if (millis() - d7Timer > d7Rate) {
        d7Timer = millis();
        if (d7On) {
            digitalWrite(D7,LOW);
        } else {
            digitalWrite(D7,HIGH);
        }
        d7On = !d7On;

        if (client.isConnected()) {
            client.loop();
        } else if (WiFi.ready()) {
            RGB.control(true);
            RGB.color(255,0,0);
            d7Rate = FAST;
            ledTimer = millis();
            if (millis() - lastConnect > RECONNECT) {
                lastConnect = millis();

                RGB.color(40,0,0);
                WiFi.off();
                digitalWrite(D7, HIGH);
                delay(1000);
                WiFi.on();
                RGB.color(255,255,0);
                delay(500);
                WiFi.connect();
                RGB.color(0,0,255);
                delay(500);
                digitalWrite(D7,LOW);
                init = true;
            }
        } else {
            RGB.control(true);
            RGB.color(255,0,255);
            ledTimer = millis();
        }
    }

    if (Serial1.available()) {
        Serial.println("Got input");
        Serial1.readBytes(buf,PACKETLENGTH);
        Serial.println("32 bytes input read");
        if (buf[0] == 0x42 && buf[1] == 0x4d) {
            Serial.println("Found valid packet header");
            if (checkValue(buf,PACKETLENGTH)) {
                pm01 = readPM01(buf); //count PM1.0 value of the air detector module
                pm25 = readPM2_5(buf);//count PM2.5 value of the air detector module
                pm10 = readPM10(buf); //count PM10 value of the air detector module
#ifdef TESTING
                Serial.print("PM01: ");
                Serial.print(pm01);
                Serial.print(" PM2.5: ");
                Serial.print(pm25);
                Serial.print(" PM10: ");
                Serial.println(pm10);
#endif
                deviceOn();     // Reset device
                gotReadings = true;
            }           
        } 
    }

    if (gotReadings) {
        // Measure RH
//        rh = round(thsensor.getRH()*100.0)/100.0;
        // Measure Temperature
//        tp = round(thsensor.getTemp()*100.0)/100.0;

        aqi = calculateAirQualityIndex(pm25);
        sprintf(sv, "{\"pm25\":%d,\"aqi\":%d,\"pm01\":%d,\"pm10\":%d,\"temperature\":%.2f,\"humidity\":%.2f}", pm25, aqi, pm01, pm10, tp, rh);
#ifdef TESTING
        Serial.println(sv);
#endif
        if (client.isConnected()) {
           client.publish("sensornet/aqi/AQI02/allvalues",sv);
        } else {
            Serial.println("ERROR: MQTT is not connected");
        }
    }
}
