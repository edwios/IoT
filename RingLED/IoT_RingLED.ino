#include "application.h"
// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"
#include "MQTT/MQTT.h"
#include "SparkJson/SparkJson.h"

#define version 101
/*
 * This is a minimal example, see extra-examples.cpp for a version
 * with more explantory documentation, example routines, how to 
 * hook up your pixels and all of the pixel types that are supported.
 *
 */

/*
 * JSON parser example
   StaticJsonBuffer<200> jsonBuffer;

  char json[] =
      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  JsonObject& root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  const char* sensor = root["sensor"];
  long time = root["time"];
  double latitude = root["data"][0];
  double longitude = root["data"][1];
  
 *
 */
  
SYSTEM_MODE(SEMI_AUTOMATIC);

#define HBLED D7                // Blinking HeartBeat LED
#define ERRFLASHTIME 125        // How fast do I blink when error
#define HBFLASHTIME 1000        // How fast do I blink normally
#define RGBLEDRELASETIME 3500   // 3.5s to release control of RBG LED

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 60
#define PIXEL_TYPE WS2812B

#define RECONNECT 10000          // Try reconnect every 10s

void callback(char* topic, byte* payload, unsigned int length);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// JSON Parser

// MQTT client("stoy.iostation.com", 1883, callback);
byte server[] = { 10,0,1,250 };
MQTT client(server, 1883, callback);

// timers
unsigned int ledTimer, hbTimer, lastConnect;
bool blink;

// received command
char cmd[8];
int lr, lg, lb;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    unsigned short r,g,b;
    char mesg[128];
    StaticJsonBuffer<200> jsonBuffer;

    memcpy(p, payload, length);
    p[length] = NULL;
    
/*
 * Old way of doing it. Now use JSON {"Command":"COLOR","Color":[255,255,255]}
 
    sscanf(p, "%s %hu %hu %hu", cmd, &r, &g, &b);
    lr=r; lg=g; lb=b;
 */ 
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
}

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

void switchOn() {
    uint16_t i;
    
    for(i=1; i<255; i++) {
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
    delay(100);
}

void switchOff() {
    uint16_t i;
    
    for(i=255; i>0; i--) {
        strip.setBrightness(i);
        strip.show();
    }
    delay(100);
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

void setup() 
{
    Time.zone(+8);
    WiFi.on();
    delay(500);
    WiFi.connect();
    delay(500);
    pinMode(HBLED, OUTPUT);
    
    Serial1.begin(9600);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    // connect to the server
    client.connect("Light");

    // publish/subscribe
    if (client.isConnected()) {
        client.publish("/sensornet/status","ready");
        client.subscribe("/sensornet/command/color");
    }
    colorAll(30,30,30);
}




void loop() 
{
    byte mac[6];
    char cIP[16];
    char cRSSI[8];
    char cMAC[20];
    const char *cSSID;
    char cTime[10];
    bool connected;

    // Gather sensor data
    if (client.isConnected()) {
        client.loop();
        connected = true;
    } else {
        connected = false;
        if (millis() - lastConnect > RECONNECT) {
            // connect to the server
            client.connect("Light");
            lastConnect = millis();
        }
    }

    // Gather system info
    IPAddress ipa = WiFi.localIP();
    WiFi.macAddress(mac);
    itoa(WiFi.RSSI(), cRSSI, 7);
    cSSID = WiFi.SSID();
    // Prepare onscreen info
    sprintf(cIP, "%u.%u.%u.%u", ipa[0], ipa[1], ipa[2], ipa[3]);
    sprintf(cMAC, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    sprintf(cTime, "%02d:%02d:%02d", Time.hour(), Time.minute(), Time.second());

    sendToLCD(1, "t0", String(cmd));
    sendToLCD(1, "t1", String(cIP));
    sendToLCD(1, "t2", String(cSSID));
    sendToLCD(1, "t3", String(cMAC));
    sendToLCD(1, "t4", String(cTime));
    sendToLCD(1, "t5", String(cRSSI));

//  rainbow(20);
    if (millis() - ledTimer > RGBLEDRELASETIME) {
        ledTimer = millis();
        RGB.control(false);
    }
    if (millis() - hbTimer > (connected?HBFLASHTIME:ERRFLASHTIME)) {
        hbTimer = millis();
        blink = !blink;
        if (blink) {
            digitalWrite(HBLED, HIGH);
        } else {
            digitalWrite(HBLED, LOW);
        }
    }
}


