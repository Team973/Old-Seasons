-- drive.lua

local config = require "config"
local wpilib = require "wpilib"

module(...)

local d = wpilib.RobotDrive(
    config.leftMotor1, config.leftMotor2,
    config.rightMotor1, config.rightMotor2
)

local hiGear = false
local x, y = 0, 0

function setGear(gear)
    hiGear = gear
end

function arcade(newY, newX)
    if newY then y = newY end
    if newX then x = newX end
end

function update()
    d:ArcadeDrive(y, x)
    if config.features.gearSwitch then
        config.gearSwitch:Set(not hiGear)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
