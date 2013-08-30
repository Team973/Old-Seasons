-- intake.lua

local wpilib = require("wpilib")
local pid = require("pid")

module(...)

local intakeDeploySpeed = .5
local intakeSpeed = 0

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)
local intakePot = wpilib.AnalogChannel(1)

local intakePID = pid.new(5, 0, 0)
intakePID.min, intakePID.max = -1, 1
intakePID:start()

PRESETS = {
    Stow = { angle = 2.0 },
    Deployed = { angle = 4.2 },
    Human = { angle = 3.07 },
    Intake = { angle = 4.3 },
}

function getAngle()
    return intakePot:GetVoltage()
end

function getTarget()
    return intakePID.target
end

function setTarget(target)
    intakePID.target = target
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setTarget(p.angle)
    end
end

function setIntakeSpeed(speed)
    intakeSpeed = speed
end

function update()
    intakeRollers:Set(intakeSpeed)
end

function getState()
    return intakeState
end

function getMoveState()
    return intakeMoveState
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Intake Angle", getAngle())
    wpilib.SmartDashboard_PutNumber("Intake PID Output", intakePID:update(getAngle()))
    wpilib.SmartDashboard_PutNumber("Intake Motor Output", motor:Get())
    wpilib.SmartDashboard_PutNumber("Intake HIT", 0)
    wpilib.SmartDashboard_PutNumber("Intake Error", getTarget() - getAngle())
    wpilib.SmartDashboard_PutString("Intake State", intakeState)
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
