FileToExecute="mw.lua"
-- Pin 2 for /Chip Enable. Pull low to enter test mode
-- i.e. remove jumper then file will auto-execute
-- Do not use Pin D3,D4 and D0, all are used for something else
CE=2
gpio.mode(CE, gpio.INPUT, gpio.PULLUP)
l = file.list()
for k,v in pairs(l) do
  if k == FileToExecute then
	if (gpio.read(CE)==1) then
    	print("Executing ".. FileToExecute)
    	dofile(FileToExecute)
    end
  end
end
-- file.remove('init.lua')