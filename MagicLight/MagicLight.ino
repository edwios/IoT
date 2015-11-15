// This #include statement was automatically added by the Particle IDE.
#include "neopixel.h"


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

#include "application.h"
#include "MQTT.h"

#define SEMI_AUTOMATIC


#define CLIENT_NAME "MagicLight-Spark"
#define RECONNECT 15*1000

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D0
#define PIXEL_COUNT 144
#define PIXEL_TYPE WS2812B

#define FAST 125
#define SLOW 500

void callback(char* topic, byte* payload, unsigned int length);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

//byte server[] = { 10,175,151,107 };
byte server[] = { 10,0,1,250 };
MQTT client(server, 1883, callback);

// timers
unsigned long ledTimer, d7Timer;
unsigned long lastConnect;
int d7Rate = 250;

// flags
bool d7On = false;
bool init = true;

// received command
char cmd[8];
int lr=80, lg=80, lb=80;
unsigned int pxcnt = PIXEL_COUNT;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    unsigned short r,g,b,f,t;
    char mesg[128];

    memcpy(p, payload, length);
    p[length] = NULL;
    String stopic = String(topic);
    if (stopic.endsWith("/pixelcount")) {
        pxcnt = atoi(p);
        Adafruit_NeoPixel strip = Adafruit_NeoPixel(pxcnt, PIXEL_PIN, PIXEL_TYPE);
    } else if (stopic.endsWith("/color")) {
        sscanf(p, "%s %hu %hu %hu", cmd, &r, &g, &b);
        lr=r; lg=g; lb=b;
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
            client.publish("sensornet/echo/MagicLight-Spark/color",mesg);
        }
    } else if (stopic.endsWith("/on")) {
        switchOn();
    } else if (stopic.endsWith("/off")) {
        switchOff();
    } else if (stopic.endsWith("/range")) {
        sscanf(p, "%hu-%hu %hu %hu %hu", &f, &t, &r, &g, &b);
        RGB.control(true);
        RGB.color(r,g,b);
        colorRange(f, t, r, g, b);
    }
}

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

void switchOn() {
    uint16_t i;

/*
    for(i=1; i<256; i++) {
        if (lr==0 && lg==0 && lb==0) {
            colorAll(255,255,255);
        } else {
            for(int j=0; j<strip.numPixels(); j++) {
                strip.setPixelColor(j, lr, lg, lb);
            }
        }
        strip.setBrightness(i);
        strip.show();
    }
*/    
    colorAll(lr,lg,lb);
}

void switchOff() {
    uint16_t i;

/*    for(i=255; i>=0; i--) {
        strip.setBrightness(i);
        strip.show();
    }
    */
    colorAll(0,0,0);
}


// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint8_t cr, uint8_t cg, uint8_t cb) {
  uint16_t i;

  strip.setBrightness(255);
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, cr, cg, cb);
  }

  strip.show();
}

void colorRange(uint8_t from, uint8_t to, uint8_t cr, uint8_t cg, uint8_t cb) {
  uint16_t i;

  if (from >= 0 && to >=0 && to <pxcnt && to >= from) {
    for(i=from; i<=to; i++) {
        strip.setPixelColor(i, cr, cg, cb);
    }
    strip.show();
  }
}

void setMqtt()
{
    if (client.isConnected()) {
        client.publish("sensornet/status/MagicLight-Spark","ready");
        client.subscribe("sensornet/MagicLight-Spark/#");
        RGB.control(true);
        RGB.color(0,255,0);
        ledTimer = millis();
        d7Rate = SLOW;
    }

}

SYSTEM_MODE(MANUAL);

void setup()
{
    Time.zone(+8);
    pinMode(D7,OUTPUT);
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
#ifdef HMILCD
    Serial1.begin(9600);
#endif
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    colorAll(80,80,30);
    digitalWrite(D7,LOW);
}




void loop()
{
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
}
