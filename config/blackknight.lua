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
armPID = pid.PID:new(20.0, 0, 0)
armPID.min, armPID.max = -1, 1
armPositionForward = 1.80
armPositionReverse = 3.02

wristPID = pid.PID:new(10.0, 0, 0)
wristPID.min, wristPID.max = -1, 1
wristPositionForward = 2.21
wristPositionReverse = 3.95
wristSafetyAngle = 35 -- in degrees

armDriveBackAmplitude = 0
armDriveBackDeadband = 0.1

gripMotor = wpilib.Victor(7)
wristMotor = wpilib.Victor(10)
wristPot = wpilib.AnalogChannel(2)
wristIntakeSwitch = wpilib.DigitalInput(5)
wristIntakeTime = 0.5

clawPiston = wpilib.Relay(4, 3, wpilib.Relay_kBothDirections)

armPresets = {
    forward={
        pickup={arm=-0.42, wrist=-0.27, claw=false},
        stow={arm=-0.42, wrist=-0.27},
        slot={arm=0, wrist=0, claw=false},
        vertical={arm=0.65, wrist=0.53},
        carry={arm=-0.43, wrist=0.74},
        low={arm=0, wrist=0},
        middle={arm=0, wrist=0},
        high={arm=0, wrist=0},
        midLow={arm=0, wrist=0},
        midMiddle={arm=0, wrist=0},
        midHigh={arm=0, wrist=0},
    },
    reverse={
        pickup={arm=0.45, wrist=0.3, claw=false},
        stow={arm=0.45, wrist=0.3},
        slot={arm=0, wrist=0, claw=false},
        vertical={arm=-0.57, wrist=-0.36},
        carry={arm=0.45, wrist=-0.68},
        low={arm=0, wrist=0},
        middle={arm=0, wrist=0},
        high={arm=0, wrist=0},
        midLow={arm=0, wrist=0},
        midMiddle={arm=0, wrist=0},
        midHigh={arm=0, wrist=0},
    },
}

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- vim: ft=lua et ts=4 sts=4 sw=4
