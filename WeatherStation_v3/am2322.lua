
local moduleName = ...
local M = {}
_G[moduleName] = M

require "my_i2c"

local AM2321_I2C_ADDR = 0xB8;
local init = false;
local t,h;

local function crc16_update(seed, byte)
    seed = bit.bxor(seed, byte);
    for i=1,8 do
        local j = bit.band(seed, 1);
        seed = bit.rshift(seed, 1);
        if j ~= 0 then
            seed = bit.bxor(seed, 0xA001);
        end
    end
    return seed;
end 

function M.init(i2c_sda_io, i2c_scl_io)
    i2c_init(i2c_sda_io, i2c_scl_io);
    init = true;
end

local function am2321_wakeup()
    i2c_start();
    i2c_send(AM2321_I2C_ADDR);
    tmr.delay(3000);
    i2c_stop();
end

local function am2321_read_raw()
    local crc=0xffff;
    local crc2=0xffff;
    res2 = {}
    am2321_wakeup();
    tmr.delay(3000);
    res2[0] = 2;
    i2c_start();
    i2c_send(AM2321_I2C_ADDR);
    i2c_send(0x03);
    i2c_send(0x00);
    i2c_send(0x04);
    i2c_stop();
    tmr.delay(6000);
    i2c_start();
    i2c_send(AM2321_I2C_ADDR+1);
    tmr.delay(200); -- wait at least 30us
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        res2[1] = tmp;
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        res2[1] = res2[1]*256 + tmp;
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        res2[2] = tmp*256;
        tmp = i2c_get(1);
        crc = crc16_update(crc,tmp);
        res2[2] = res2[2] + tmp;
        tmp = i2c_get(1);
        crc2 = tmp;
        tmp = i2c_get(0);
        crc2 = crc2 + tmp*256;
    i2c_stop();
    if (crc == crc2) then res2[0]=0; else res2[0]=1; end
    return res2;
end

-- read temperature and humidity from si7021
function M.read()
  if (not init) then
     print("init() must be called before read.");
     e = 1;
  else
    res = am2321_read_raw();
    e = res[0];
    h = res[1];
    t = res[2];
  end
  return e;
end;

-- get humidity
function M.getHumidity()
    -- 2 decimal points
  return h*10
end

-- get temperature
function M.getTemperature()
    -- 2 decimal points
  return t*10
end

return M
