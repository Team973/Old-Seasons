-- drive.lua

local config = require "config"
local wpilib = require "wpilib"

module(...)

local d = wpilib.RobotDrive(
    config.leftMotor1, 
    config.rightMotor1, 
    if features.driveYCable then
        config.rightMotor2,
        config.leftMotor2,
    end
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
    if config.flipDriveY then
        d:ArcadeDrive(-y, x)
    else
        d:ArcadeDrive(y, x)
    end

    if config.features.gearSwitch then
        config.gearSwitch:Set(not hiGear)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
