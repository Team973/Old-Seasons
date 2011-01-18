-- config.lua

local wpilib = require "wpilib"

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

-- Arm
-- TODO: Fill in for actual motors
armMotor = wpilib.Jaguar(5)
armPID = pid.PID:new(1,0,0)
armPID.min, armPID.max = -.5,.5
armPreset1 = .45
armPreset2 = .87
armPreset3 = 1.45
if features.grabber then
    grabberManualSpeed = 0.3
    grabberMotor = wpilib.Victor(6)
end

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
