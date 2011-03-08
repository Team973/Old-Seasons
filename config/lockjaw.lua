-- config/lockjaw.lua
-- Practice Bot

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
armPot = wpilib.AnalogChannel(2)
armPID = pid.PID:new(10, 0, 0)
armPID.min, armPID.max = -1, 1
armUpwardP = 10
armDownwardP = 10
armPositionForward = 1.69
armPositionReverse = 3.24

wristPID = pid.PID:new(10, 0, 0)
wristPID.min, wristPID.max = -1, 1
wristPositionForward = 2.21
wristPositionReverse = 3.95
wristSafetyAngle = 35 -- in degrees
flipWrist = true
flipWristPot = true

armDriveBackAmplitude = 0
armDriveBackDeadband = 0.1

gripMotor = wpilib.Victor(6)
wristMotor = wpilib.Victor(8)
wristPot = wpilib.AnalogChannel(3)
wristIntakeSwitch = wpilib.DigitalInput(5)
wristIntakeTime = 0.5

clawSolenoids = true
clawOpenPiston = wpilib.Solenoid(2)
clawClosePiston = wpilib.Solenoid(3)

armLength = 42.75 -- inches
armRunDelta = 8 -- inches

armPresets = {
    forward={
        pickup={arm=-0.60, wrist=-0.27, claw=-1},
        stow={arm=-0.74, wrist=-0.27, claw=0},
        slot={arm=0, wrist=0, claw=-1},
        vertical={arm=0.75, wrist=0.53},
        carry={arm=-0.68, wrist=0.74, claw=0},
        low={arm=0, wrist=0},
        middle={arm=0, wrist=0},
        high={arm=0, wrist=0},
        midLow={arm=0, wrist=0},
        midMiddle={arm=0, wrist=0},
        midHigh={arm=0, wrist=0},
    },
    reverse={
        pickup={arm=0.55, wrist=0.3, claw=-1},
        stow={arm=0.65, wrist=0.3, claw=0},
        slot={arm=0, wrist=0, claw=-1},
        vertical={arm=-0.80, wrist=-0.36},
        carry={arm=0.62, wrist=-0.68, claw=0},
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
