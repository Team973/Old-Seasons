-- drive.lua

local config = require "config"
local wpilib = require "wpilib"

module(...)

function init()
    config.leftDriveEncoder:Start()
    config.rightDriveEncoder:Start()
end

local d
if config.leftMotor2 and config.rightMotor2 then
    d = wpilib.RobotDrive(
        config.leftMotor1, config.leftMotor2,
        config.rightMotor1, config.rightMotor2
    )
else
    d = wpilib.RobotDrive(config.leftMotor1, config.rightMotor1)
end

local hiGear = false
local x, y = 0, 0

function setGear(gear)
    hiGear = gear
end

function arcade(newY, newX)
    if newY then y = newY end
    if newX then x = newX end
end

function getDrive()
    return d
end

function update()
    if config.flipDriveY then
        d:ArcadeDrive(-y, x)
    else
        d:ArcadeDrive(y, x)
    end

    if config.features.gearSwitch then
        config.gearSwitch:Set(hiGear)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
