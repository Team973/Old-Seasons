-- config/blackknight.lua

local fakesolenoid = require "fakesolenoid"
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
leftMotor1 = wpilib.Jaguar(1)
leftMotor2 = wpilib.Jaguar(3)
rightMotor1 = wpilib.Jaguar(2)
rightMotor2 = wpilib.Jaguar(4)

gearSwitch = fakesolenoid.new(4, 2)

flipDriveY = true

-- Arm
-- TODO: Fill in for actual motors
armMotor = wpilib.Jaguar(5)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(8.4, 0, 0)
armPID.min, armPID.max = -1, 1
armPreset1 = .13
armPreset2 = 1.16
armPreset3 = 2.70
if features.grabber then
    grabberManualSpeed = 0.3
    grabberMotor = wpilib.Victor(6)
end

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4