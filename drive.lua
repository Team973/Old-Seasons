-- drive.lua

local config = require "config"
local wpilib = require "wpilib"

module(...)

local d = wpilib.RobotDrive(
    config.leftMotor1, config.leftMotor2,
    config.rightMotor1, config.rightMotor2
)

local hiGear = false
local lastLo, lastHi = false, false

function drive(stick1, stick2)
    d:ArcadeDrive(-stick1:GetY(), -stick2:GetX())

    if stick1:GetRawButton(1) and not lastLo then
        hiGear = false
    elseif stick2:GetRawButton(1) and not lastHi then
        hiGear = true
    end
    lastLo, lastHi = stick1:GetRawButton(1), stick2:GetRawButton(1)

    if config.features.gearSwitch then
        config.gearSwitch:Set(not hiGear)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
