-- config/titan.lua
-- Competition Bot

local wpilib = require "wpilib"
local pid = require "pid"

module(...)

watchdogEnabled = false

features =
{
    compressor = true,
    gearSwitch = true,
    lcd = true,
    softClutch = false,
}

-- Drive
leftMotor1 = wpilib.Victor(1)
rightMotor1 = wpilib.Victor(2)

gearSwitch = wpilib.Solenoid(1)

flipDriveY = false

-- Arm
armMotor = wpilib.Jaguar(5)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(5.0, 0, 0)
armPID.min, armPID.max = -1, 1
armPos90 = 1.16
armPos180 = 2.70
armPresets = {.13, armPos90, armPos180}
armDriveBackAmplitude = 0
armDriveBackDeadband = 0.1

gripMotor = wpilib.Victor(7)
wristMotor = wpilib.Jaguar(10)

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
