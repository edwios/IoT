WeatherStation
==============
Version 3

Weather station based on the new SI7021 Temperature and Humidity sensor and ESP8266 running NodeMCU firmware.

Incorporated also monitoring brightness of the environment, motion and sound detection.
An MQTT message with topic "sensornet/command/arm" will arm the motion alarm.
An MQTT message with topic "sensornet/command/disarm" will disarm it.
When the motion alarm is armed, any motion detected will trigger a MQTT message with topic of "$BASE_TOPIC/alarm/motion".

New with version 3.1
A new and lighter mw.lua provides support on the AM2322 (same as AM2321) temperature and relative humidity sensor. The AM232x uses a non-standard I2C signaling and thus will not work with the standard I2C libraries available. The AM2322.lua and my_i2c.lua were not my work, I took it from [Saper-2](https://github.com/saper-2/esp8266-am2321-remote-sensor) and changed it a bit to give a more consistent interface (e.g. on initializing the I2C) and make the AM2321 into a separate module.

Instructions
============
Update the settings in config.lua
Upload all lua files to the ESP8266
Test by using dofile('mw.lua')
If everything is ok, remove the wire between D2 and Gnd. This wire prevents the execution of init.lua which is the autorun on the NodeMCU. If this wire is removed. upon every restart of the NodeMCU, mw.lua will be executed automatically.
