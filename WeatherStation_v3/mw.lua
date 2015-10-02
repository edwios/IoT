dofile('config_livingroom.lua')		-- import configurations
gpio.mode(PIN_Motion, gpio.INT)
gpio.mode(PIN_Sound, gpio.INT)
-- gpio.mode(PIN_Alarm, gpio.INT, gpio.PULLUP)
alarmMode=false
alarmFiring=false
mot=0
snd=0

local m_dis={}
local connected = false

local function dispatch(m,t,pl)
	if pl~=nil and m_dis[t] then
		m_dis[t](m,pl)
	end
end
local function dumpfunc(m,pl)
	print("dump: "..pl)
	read_sensor()
end
local function stopfunc(m,pl)
	print("stop: "..pl)
end
local function startfunc(m,pl)
	print("start: "..pl)
end
local function armAlarm(m,pl)
	print("arm: "..pl)
	alarmMode=true
	m:publish("sensornet/status/"..MQTTClientName,"ARMED",0,0)
end
local function disArmAlarm(m,pl)
	print("disarm: "..pl)
	alarmMode=false
	m:publish("sensornet/status/"..MQTTClientName,"DISARMED",0,0)
end

-- useless when in deep sleep
m_dis["sensornet/command/dump"]=dumpfunc
m_dis["sensornet/command/stop"]=stopfunc
m_dis["sensornet/command/start"]=startfunc
m_dis["sensornet/command/arm"]=armAlarm
m_dis["sensornet/command/disarm"]=disArmAlarm


print("Setup wifi mode: STATION")
wifi.setmode(wifi.STATION)
local cnt = 0
--please config ssid and password according to settings of your wireless router.
wifi.sta.config(BSSID0,WPA2PW)
wifi.sta.connect()

-- init mqtt client with keepalive timer 120sec
m = mqtt.Client(MQTTClientName, 120, null, null)

-- Reflect MQTT connection status
m:on("connect", function(m)
	connected = true
	print ("Broker connected")
	m:subscribe("sensornet/command/dump",0)
	m:subscribe("sensornet/command/start",0)
	m:subscribe("sensornet/command/stop",0)
	m:subscribe("sensornet/command/arm",0)
	m:subscribe("sensornet/command/disarm",0)
	-- Publish to MQTT with QoS = 0, retain = 0, announcing healthy
	m:publish("sensornet/status/"..MQTTClientName,"ready",0,0, function(conn)
		read_sensor()		-- read and publish data when publish successful
	end)
end)

m:on("offline", function(con)
	print ("offline")
	connected = false
end)

-- on publish message receive event
m:on("message",dispatch )

function motionpcb()
	mot=mot+1
	if (alarmMode and not alarmFiring) then
		alarmFiring=true
		fireAlarm()
	end
end

function soundpcb()
	snd=snd+1
end

function fireAlarm()
	if (connected) then
		m:publish(MQTTBaseTopic.."alarm/motion",1,0,0, function ( conn )
			-- body
			alarmFiring=false
		end)
	end
end

local function doInit()
	-- Lua: mqtt:connect( host, port, secure, auto_reconnect, function(client) )
	-- for secure: m:connect("10.0.1.252", 1880, 1)
	print("Connecting to MQTT broker")
	m:connect(MQTTBroker, MQTTPort, 0, 1)
	gpio.trig(PIN_Motion, "up", motionpcb)
	gpio.trig(PIN_Sound, "up", soundpcb)
--	gpio.trig(PIN_Alarm, "both", armAlarm)
end



-- m:close();
-- you can call m:connect again

function read_sensor()
--	gpio.mode(PIN_Motion, gpio.INPUT)
--	gpio.mode(PIN_Sound, gpio.INPUT)
	SDA_PIN = 6 -- sda pin, GPIO12
	SCL_PIN = 5 -- scl pin, GPIO14
	LED = 1 -- Heartbeat LED

	-- Set GPIO pin for LED
	gpio.mode(LED,gpio.OUTPUT)
	gpio.write(LED, gpio.LOW) -- Lighit it up

	si7021 = require("si7021")
	si7021.init(SDA_PIN, SCL_PIN)
	si7021.read(OSS)
	h = si7021.getHumidity()
	t = si7021.getTemperature()
	b = 1024-adc.read(0)

	-- pressure in differents units
	print("Humidity: "..(h / 100).."."..(h % 100).." %")

	-- temperature in degrees Celsius  and Farenheit
	print("Temperature: "..(t/100).."."..(t%100).." deg C")

	-- temperature in degrees Celsius  and Farenheit
	print("Brightness: "..b.." units")

	if (connected) then
		ts = ""..(t/100).."."..(t%100)

		-- Publish to MQTT with QoS = 0, retain = 0
		m:publish(MQTTBaseTopic.."temperature",ts,0,0, function(conn) 
    		print("Published> "..ts) 
			ts = ""..(h/100).."."..(h%100)
			m:publish(MQTTBaseTopic.."humidity",ts,0,0, function(conn) 
    			print("Published> "..ts)
    			m:publish(MQTTBaseTopic.."brightness",b,0,0, function (conn)
	    			print("Published> "..b)
					m:publish(MQTTBaseTopic.."motion",mot,0,0, function ( conn )
						print("Published> "..mot)
						mot=0 -- reset motion count
						m:publish(MQTTBaseTopic.."sound",snd,0,0, function ( conn )
							print("Published> "..snd)
							snd=0 -- reset sound count
--							gpio.mode(PIN_Motion, gpio.INT)
--							gpio.mode(PIN_Sound, gpio.INT)
						end)
					end)
    			end)
			end)
		end)
	end
	-- release module
	si7021 = nil
	package.loaded["si7021"]=nil

	gpio.write(LED, gpio.HIGH) -- Turn LED off

end


--Timers

tmr.alarm(1, 1000, 1, function() 
    if (wifi.sta.getip() == nil) and (cnt < 20) then 
    	print("Getting IP... " ..cnt.." (20 times max)") 
    	cnt = cnt + 1
    	if (cnt == 6) then 
--please config ssid and password according to settings of your wireless router.	    		
    		wifi.sta.config(BSSID1,WPA2PW)
    		--WRITE YOU PROGRAM HERE
    	end	

    	if (cnt == 12) then 
--please config ssid and password according to settings of your wireless router.	    		
    		wifi.sta.config(BSSID2,WPA2PW2)
    		--WRITE YOU PROGRAM HERE
    	end	 	    	  
    else 
    	tmr.stop(1)
    	if (cnt < 20) then print("Config done, IP is "..wifi.sta.getip())
    		--WRITE YOU PROGRAM HERE
    		doInit()
    	else print("Wifi setup time more than 20 times, Please verify wifi.sta.config() function. Then re-download the file.")
--    		node.dsleep(sleep_time, 0)  -- sleep anyway, hope waking up to a better WiFi
    	end
    end 
end)

tmr.alarm(0, 5*60*1000, 1, function()
	read_sensor()
end)
