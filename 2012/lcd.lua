-- lcd.lua

local string = require "string"
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

    function print(line, format, ...)
        lcd:PrintLine(lineConstants[line], string.format(format, ...))
    end

    function update()
        lcd:UpdateLCD()
    end
else
    print = function() end
    update = function() end
end
