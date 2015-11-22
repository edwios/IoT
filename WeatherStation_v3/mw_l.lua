dofile('config.lua')

local sd=false
local m_dis={}
local connected = false

wifi.setmode(wifi.STATION)
local cnt = 0
wifi.sta.config(BSSID0,WPA2PW)
wifi.sta.autoconnect(0)
wifi.sta.connect()
m = mqtt.Client(MQTTClientName, 120, null, null)
m:on("connect", function(m)
	connected = true
	print("Connected")
	m:publish("sensornet/status/"..MQTTClientName,"ready",0,0, function(conn)
		read_sensor()
	end)
end)

m:on("offline", function(con)
	connected = false
end)

local function doInit()
	sd=false
	m:connect(MQTTBroker, MQTTPort, 0, 1)
end

function read_sensor()
	gpio.mode(LED,gpio.OUTPUT)
	gpio.write(LED, gpio.LOW)

	sensor = require("am2322")
	sensor.init(SDA_PIN, SCL_PIN)
	err = sensor.read()
	h = sensor.getHumidity()
	t = sensor.getTemperature()
	b = 1024-adc.read(0)
	if (connected and not (err == 1)) then
		ts = ""..(t/100).."."..(t%100)
		m:publish(MQTTBaseTopic.."temperature",ts,0,0, function(conn) 
			ts = ""..(h/100).."."..(h%100)
			m:publish(MQTTBaseTopic.."humidity",ts,0,0, function(conn) 
    			m:publish(MQTTBaseTopic.."brightness",b,0,0, function (conn)
    				print("Pub'ed")
    				wifi.sta.disconnect()
    				sd = true
    			end)
			end)
		end)
	end
	sensor = nil
	package.loaded["am2322"]=nil
	gpio.write(LED, gpio.HIGH)
end

--Timers
tmr.alarm(1, 1000, 1, function() 
    if (wifi.sta.getip() == nil) and (cnt < 20) then 
    	cnt = cnt + 1
    	if (cnt == 6) then 
    		wifi.sta.config(BSSID1,WPA2PW)
    	end	

    	if (cnt == 12) then 
    		wifi.sta.config(BSSID2,WPA2PW2)
    	end	 	    	  
    else 
    	tmr.stop(1)
    	if (cnt < 20) then print("Config done, IP is "..wifi.sta.getip())
    		doInit()
    	else print("Wifi Err")
    		sd=true
    	end
    end 
end)

tmr.alarm(2, 500, 1, function() 
	if ((wifi.sta.getip() == nil) and sd) then
		tmr.stop(2)
		node.dsleep(sleep_time, 0)
	end
end)
