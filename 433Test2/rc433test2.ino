// This #include statement was automatically added by the Particle IDE.
#include "si4432.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

Si4432 radio(nSEL, SDN, nIRQ);

byte pkg;
//byte resLen = 0;
//static byte answer[64] = { 0 };
static byte msg[64] = { 0x01, 0x03, 0x11, 0x13 };
byte Rx = 1;
byte rssi = 0;
byte rb = 0;

void init(void) {
    pinMode(TxNRx, INPUT_PULLDOWN);             // default Rx
    pinMode(LED, OUTPUT);
    Rx = !(digitalRead(TxNRx));
    RGB.control(true);
    RGB.brightness(0);                          // Turn off RGB
    WiFi.off();                                 // We don't need Wi-Fi yet
}

void initRadio(void) {
    radio.softReset();
    radio.setBaudRate(70);
    radio.setFrequency(433);
//    radio.readAll();
}

void setRGB(byte val) {
    byte red = 255 - val;
    byte green = val;
    RGB.color(red, green, 0);
    RGB.brightness(255);    
}

void setup() {
    Serial.begin(9600);
    delay(1000);
    init();
    radio.init();
    radio.setBaudRate(70);
    radio.setFrequency(433);
    radio.readAll();
    
    if (Rx) {
        digitalWrite(LED, 1);
        radio.startListening();
    }
    
}

void loop() {
    
    byte retry = 5;
    RGB.brightness(0);

    if (!Rx) {                      // Tx
        digitalWrite(LED, 0);
        delay(1500);
        digitalWrite(LED, 1);
        delay(500);                // Send/Receive each 2 second
        RGB.color(0,0,255);
        RGB.brightness(255);
        DEBUG_PRINT("Main loop Send packet");
        msg[0]='T'; msg[1]='x';
        msg[2] = rb; msg[3]=msg[3]+1;

//        pkg = radio.sendPacket(4, msg, true, 50, &resLen, answer);
        pkg = radio.sendPacket(4, msg);
        rb=radio.readRSSI(); 

    } else {                        // Rx
//        DEBUG_PRINT("Main loop Waiting for packet");
        pkg = radio.isPacketReceived();
    }
    
    if (pkg == radio.STATUS_OK) {
        if (!Rx) {                  //Tx
            
            if(!radio.waitForPacket(100)) {
                DEBUG_PRINT("No ack received");
                RGB.color(255,0,0);
                RGB.brightness(255);
                delay(100);
            } else {
                RGB.color(0,255,0);
                RGB.brightness(255);
                byte answer[64] = { 0 };
                byte resLen = 0;
                radio.getPacketReceived(&resLen, answer);
                DEBUG_PRINT("Got packet of length %d", resLen);
                for (int i = 0; i < resLen; i++) {
                    Serial.printf("0x%x ", answer[i]);
                }
                Serial.println();
                if ((answer[0] == 'R') && (answer[1] == 'x')) {
                    rb = answer[2];
                } else {
                    rb = 255;
                }
            }
        } else {                     // Rx
            RGB.color(0,255,0);
            RGB.brightness(255);
            byte payload[64] = { 0 };
            byte len = 0;
            radio.getPacketReceived(&len, payload);
            if ((payload[0] == 'T') && (payload[1] == 'x')) {
                rb = payload[2];
            } else {
                rb = 255;
            }
            DEBUG_PRINT("Received %d bytes", len);
            
            for (int i=0; i < len; i++) {
                Serial.printf("0x%x ", (int)payload[i]);
            }
            Serial.println();
            
            DEBUG_PRINT("Sending response");
            msg[0]='R'; msg[1]='x';
            msg[2]=radio.readRSSI(); msg[3]=msg[3]+1;
//            initRadio();
            uint16_t et = millis();
            while ((millis() - et < 500) && !radio.sendPacket(4, msg));
            DEBUG_PRINT("Response sent!");
            pkg = radio.STATUS_OK;
//            initRadio();
            setRGB(rb);
            radio.startListening();
        }
    }
    if (pkg == radio.NO_PACKET) {
        // Ignore
    } else if (pkg != radio.STATUS_OK) {
        RGB.color(255,0,0);
        RGB.brightness(255);
        delay(200);
        ERROR_PRINT("Generic, status: 0x%x", pkg);
        initRadio();
        if (Rx)
            radio.startListening();
    }
}