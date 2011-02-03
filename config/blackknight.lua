-- config/blackknight.lua

local fakesolenoid = require "fakesolenoid"
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
leftMotor1 = wpilib.Jaguar(1)
leftMotor2 = wpilib.Jaguar(3)
rightMotor1 = wpilib.Jaguar(2)
rightMotor2 = wpilib.Jaguar(4)

gearSwitch = fakesolenoid.new(4, 2)

flipDriveY = true

-- Arm
armMotor = wpilib.Jaguar(5)
armPot = wpilib.AnalogChannel(1)
armPID = pid.PID:new(8.4, 0, 0)
armPID.min, armPID.max = -1, 1
armPositionForward = 2.00
armPositionReverse = 3.22
armPresets = {
    forward={
        pickup={arm=-1, wrist=0},
        stow={arm=1.78, wrist=0},
        slot={arm=0, wrist=0},
    },
    reverse={
        pickup={arm=0, wrist=0},
        stow={arm=0, wrist=0},
        slot={arm=0, wrist=0},
    },
}
armDriveBackAmplitude = 0
armDriveBackDeadband = 0.1

gripMotor = wpilib.Victor(7)
wristMotor = wpilib.Jaguar(10)

clawPiston = wpilib.Relay(4, 3, wpilib.Relay_kBothDirections)

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
