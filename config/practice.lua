-- config/practice.lua

local fakesolenoid = require "fakesolenoid"
local wpilib = require "wpilib"
local pid = require "pid"

module(...)

watchdogEnabled = false

features =
{
    compressor = true,
    grabber = false, 
    gearSwitch = false, --Pneumatics not setup on practice bot 
    lcd = true,
    softClutch = false,
    driveYCable = true
}

-- Drive
leftMotor1 = wpilib.Victor(1)
leftMotor2 = wpilib.Victor(1)
rightMotor1 = wpilib.Victor(2)
rightMotor2 = wpilib.Victor(2)

gearSwitch = fakesolenoid.new(4, 2)

flipDriveY = true

-- Arm
armMotor = wpilib.Jaguar(7)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(8.4, 0, 0)
armPID.min, armPID.max = -1, 1
armPos90 = 1.62
armPos180 = 3.22
armPresets = {.13, armPos90, armPos180}
armDriveBackAmplitude = 0.3
armDriveBackDeadband = 0.1

--Grabber Variables
if features.grabber then
    gripMotor = wpilib.Victor(6)
    wristMotor = wpilib.Victor(5)
end


-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
