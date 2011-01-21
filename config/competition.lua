-- config/competition.lua

local wpilib = require "wpilib"
local pid = require "pid"

module(...)

watchdogEnabled = false

features =
{
    compressor = true,
    grabber = false, 
    gearSwitch = true,
    lcd = true,
    softClutch = false,
}

-- Drive
leftMotor1 = wpilib.Victor(1)
leftMotor2 = wpilib.Victor(5)
rightMotor1 = wpilib.Victor(2)
rightMotor2 = wpilib.Victor(4)

gearSwitch = wpilib.Solenoid(1)

flipDriveY = false

-- Arm
-- TODO: Fill in for actual motors
armMotor = wpilib.Jaguar(5)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(5.0, 0, 0)
armPID.min, armPID.max = -1, 1
armPresets = {.13, 1.16, 2.70}
if features.grabber then
    grabberManualSpeed = 0.3
    grabberMotor = wpilib.Victor(6)
end

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
