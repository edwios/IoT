// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

#define RECONNECT 15*1000
#define CLIENT_NAME "MagicLight-Spark"

byte server[] = { 10,0,1,250 };
MQTT client(server, 1883, callback);

bool init = true;
unsigned long lastConnect;

void callback(char* topic, byte* payload, unsigned int length) {
    // MQTT messages received here in topic and payload
}

void setMqtt()
{
    if (client.isConnected()) {
        client.publish("sensornet/status/MagicLight-Spark","ready");
        client.subscribe("sensornet/MagicLight-Spark/#");
    }

}


SYSTEM_MODE(MANUAL);

void setup()
{
    WiFi.on();
    WiFi.connect();
}

void loop() {
	if (WiFi.ready() && init) {         // Wait it out until we have a network to talk to
    	client.connect(CLIENT_NAME);    // connect to the broker as CLIENT_NAME
    	delay(500);
    	setMqtt();                      // Perform MQTT pub/sub
    	delay(200);
    	init = false;
	}

	if (client.isConnected()) {
        client.loop();                  // As freq as possible to keep the realtimeness
    } else if (WiFi.ready()) {          // Sum Ting Wong ... network's there but server's gone :(
        if (millis() - lastConnect > RECONNECT) {   // try not to stress the network
            lastConnect = millis();
            WiFi.off();                 // Restart the WiFi part
            delay(1000);
            WiFi.on();
            delay(500);
            WiFi.connect();
            init = true;                // Reconnect when network's available again
        }
    }
}
