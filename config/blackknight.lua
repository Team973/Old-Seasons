-- config/blackknight.lua


-- Requires --
local fakesolenoid = require "fakesolenoid"
local wpilib = require "wpilib"
local pid = require "pid"

module(...)
--------------

watchdogEnabled = false


features =
{
    compressor = true,
    grabber = true, 
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



-- Arm --
-- TODO: Fill in for actual motors

--Arm feedForward 
armCompensateAmplitude = 1
armVoltsIn90Degrees = 1.093  
armVoltsIn180Degrees = 2.67

--Motors and PID
armMotor = wpilib.Jaguar(5)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(8.4, 0, 0)
armPID.min, armPID.max = -1, 1
armPresets = {0, 1.16, 2.70}


-- Grabber --
if features.grabber then
    --grabberManualSpeed = 1
    grabberPickUpMotor = wpilib.Victor(7)
    grabberWristMotor = wpilib.Jaguar(10)
end

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
