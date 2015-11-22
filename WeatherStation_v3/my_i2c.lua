-- my-i2c - config
local i2c_hdelay_us = 5
local i2c_sda_io = 0
local i2c_scl_io = 0

-- I2C_ACK = 1, NACK=0
function i2c_init(sda, scl)
    i2c_sda_io = sda
    i2c_scl_io = scl
    gpio.mode(i2c_sda_io, gpio.OUTPUT, gpio.PULLUP);
    gpio.write(i2c_sda_io, gpio.HIGH);
    gpio.mode(i2c_scl_io, gpio.OUTPUT, gpio.PULLUP); -- strong driving - no needpullup
    gpio.write(i2c_scl_io, gpio.HIGH);
end

function i2c_hdelay()
    tmr.delay(i2c_hdelay_us);
end

function i2c_sdaset()
    gpio.write(i2c_sda_io, gpio.HIGH);
    gpio.mode(i2c_sda_io, gpio.INPUT, gpio.PULLUP);
end

function i2c_sdaclear()
    gpio.write(i2c_sda_io, gpio.LOW);
    gpio.mode(i2c_sda_io, gpio.OUTPUT, gpio.PULLUP);
end

function i2c_sdaget()
    return gpio.read(i2c_sda_io);
end

function i2c_sclset()
    gpio.write(i2c_scl_io, gpio.HIGH);
end

function i2c_sclclear()
    gpio.write(i2c_scl_io, gpio.LOW);
end

function i2c_start()
    --print("-i2c_start-");
    -- start without stop
    i2c_sdaset();
    i2c_hdelay();
    i2c_sclset();
    i2c_hdelay();
    -- standard start seq.
    i2c_sdaclear();
    i2c_hdelay();
    i2c_sclclear();
end

function i2c_stop()
    --print("-i2c_stop-");
    i2c_sdaclear();
    i2c_hdelay();
    i2c_sclset();
    i2c_hdelay();
    i2c_sdaset();
    i2c_hdelay();
end

function i2c_send(data)
    for n=7,0,-1 do
        if (bit.isset(data,n) == true) then
            i2c_sdaset(); 
        else 
            i2c_sdaclear();
        end;
        --data = bit.lshift(data, 1);
        i2c_hdelay();
        i2c_sclset();
        i2c_hdelay();
        i2c_sclclear();
    end
    -- ACK
    i2c_sdaset();
    i2c_hdelay();
    i2c_sclset();
    i2c_hdelay();
    n = i2c_sdaget();
    i2c_sclclear();

    --print("-i2c_send("..string.format("%02X",data)..")="..n.."-");
    
    return n;
end

function i2c_get(ack) 
    temp = 0;
    i2c_sdaset();
    for n=7,0,-1 do
        i2c_hdelay();
        i2c_sclset();
        i2c_hdelay();
        --temp <<= 1;
        if (i2c_sdaget() == 1) then
            temp = bit.set(temp, n);
        end
        i2c_sclclear();
    end
    if (ack > 0) then
        i2c_sdaclear();
    else
        i2c_sdaset();
    end
    i2c_hdelay();
    i2c_sclset();
    i2c_hdelay();
    i2c_sclclear();
    
    return temp;    
end

