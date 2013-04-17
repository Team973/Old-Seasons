-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")
local pid = require("pid")

module(...)

local deploy = false
local retract = false
local intakeDeploySpeed = .5
local intakeSpeed = 0

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)
local intakePot = wpilib.AnalogChannel(7)

-- Intake States
local intakeState = nil
local STOWED = "stowed"
local DEPLOYED = "deployed"
local DOWN = "down"

local intakePID = pid.new(0, 0, 0)
intakePID.min, intakePID.max = -.5, .5
intakePID:start()

PRESETS = {
    Stow = { angle = 0.0 },
}

function getAngle()
    return intakePot:GetValue()
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

function goToStow(bool)
    stowed = bool
    if stowed then
        setPreset("Stow")
        intakeState = STOW
    end
end

function goToDeploy(bool)
    deployed = bool
    if deployed then
        setPreset("Deployed")
        intakeState = DEPLOYED
    end
end

function goToDown(bool)
    down = bool
    if down then
        setPreset("Down")
        intakeState = DOWN
    end
end

function update()
    intakeRollers:Set(intakeSpeed)

    --[[
    if intakeState == DEPLOYED then
        if arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif intakeState == STOW then
        if getAngle() > 1 and arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif intakeState == DOWN then
        if getAngle() < 3.4 then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    else
        motor:Set(0.0)
    end
    ]]
end

function getState()
    return intakeState
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Intake Angle", getAngle())
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
