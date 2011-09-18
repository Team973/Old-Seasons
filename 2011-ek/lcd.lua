-- lcd.lua

local wpilib = require "wpilib"

module(...)

local enableLcd = true

if enableLcd then
    local lcd = wpilib.DriverStationLCD_GetInstance()
    local lineConstants = {
        wpilib.DriverStationLCD_kUser_Line1,
        wpilib.DriverStationLCD_kUser_Line2,
        wpilib.DriverStationLCD_kUser_Line3,
        wpilib.DriverStationLCD_kUser_Line4,
        wpilib.DriverStationLCD_kUser_Line5,
        wpilib.DriverStationLCD_kUser_Line6,
    }

    function print(line, msg)
        lcd:PrintLine(lineConstants[line], msg)
    end
    
    function update()
        lcd:UpdateLCD()
    end
else
    print = function() end
    update = function() end
end
