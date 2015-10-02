# **WeatherStation** #
Version 3

Weather station based on the new SI7021 Temperature and Humidity sensor and ESP8266 running NodeMCU firmware.

Incorporated also monitoring brightness of the environment, motion and sound detection.
An MQTT message with topic "sensornet/command/arm" will arm the motion alarm.
An MQTT message with topic "sensornet/command/disarm" will disarm it.
When the motion alarm is armed, any motion detected will trigger a MQTT message with topic of "$BASE_TOPIC/alarm/motion".


# Instructions
<<<<<<< HEAD
Update the settings in config.lua
Upload all lua files to the ESP8266
Test by using dofile('mw.lua')
If everything alrights, remove the wire between D2 and Gnd
The next restart will execute mw.lua automatically
=======
Update the settings in config.lua<br/>
Upload all lua files to the ESP8266<br/>
Test by using dofile('mw.lua')<br/>
If everything alrights, remove the wire between D2 and Gnd<br/>
The next restart will execute me.lua automatically<br/>
>>>>>>> 63f194a9360c389c35d95f778545383c2eb9002d


