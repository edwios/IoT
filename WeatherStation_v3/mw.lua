local m_dis={}
local connected = false
local MQTTClientName = "NodeMCU01"
local sleep_time = 600 * 1000 * 1000

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

-- useless when in deep sleep
m_dis["sensornet/command/dump"]=dumpfunc
m_dis["sensornet/command/stop"]=stopfunc
m_dis["sensornet/command/start"]=startfunc


print("Setup wifi mode: STATION")
wifi.setmode(wifi.STATION)
local cnt = 0
--please config ssid and password according to settings of your wireless router.
wifi.sta.config("BSSID0","UNAME0")
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



local function doInit()
	-- Lua: mqtt:connect( host, port, secure, auto_reconnect, function(client) )
	-- for secure: m:connect("10.0.1.252", 1880, 1)
	print("Connecting to MQTT broker")
	m:connect("10.0.1.250", 1883, 0, 1)
end



-- m:close();
-- you can call m:connect again

function read_sensor()
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

	-- pressure in differents units
	print("Humidity: "..(h / 100).."."..(h % 100).." %")

	-- temperature in degrees Celsius  and Farenheit
	print("Temperature: "..(t/100).."."..(t%100).." deg C")

	if (connected) then
		ts = ""..(t/100).."."..(t%100)

		-- Publish to MQTT with QoS = 0, retain = 0
		m:publish("sensornet/env/home/balcony/temperature",ts,0,0, function(conn) 
    		print("Published> "..ts) 
			ts = ""..(h/100).."."..(h%100)

			-- Publish to MQTT with QoS = 0, retain = 0
			m:publish("sensornet/env/home/balcony/humidity",ts,0,0, function(conn) 
    			print("Published> "..ts)
    			node.dsleep(sleep_time, 0)
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
    		wifi.sta.config("BSSID1","UNAME1")
    		--WRITE YOU PROGRAM HERE
    	end	

    	if (cnt == 12) then 
--please config ssid and password according to settings of your wireless router.	    		
    		wifi.sta.config("BSSID2","UNANE2")
    		--WRITE YOU PROGRAM HERE
    	end	 	    	  
    else 
    	tmr.stop(1)
    	if (cnt < 20) then print("Config done, IP is "..wifi.sta.getip())
    		--WRITE YOU PROGRAM HERE
    		doInit()
    	else print("Wifi setup time more than 20 times, Please verify wifi.sta.config() function. Then re-download the file.")
    	end
    end 
 end)

