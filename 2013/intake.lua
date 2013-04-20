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
local intakePot = wpilib.AnalogChannel(1)

-- Intake States
local intakeState = nil
local STOWED = "stowed"
local DEPLOYED = "deployed"

local intakePID = pid.new(15, 0, 0)
intakePID.min, intakePID.max = -1, 1
intakePID:start()

PRESETS = {
    Stow = { angle = 2.3 },
    Deployed = { angle = 2.7 },
    Human = { angle = 2.5 },
    Intake = { angle = 2.76 },
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

function goToStow(bool)
    stowed = bool
    if stowed then
        intakeState = STOW
        setPreset("Stow")
        stowed = false
    end
end

function goToDeploy(bool)
    deployed = bool
    if deployed then
        intakeState = DEPLOYED
        setPreset("Deployed")
        deployed = false
    end
end

function update()
    intakeRollers:Set(intakeSpeed)

        if intakeState == DEPLOYED  then
            if arm.isIntakeDeploySafe() then
                if getAngle() < 2.74 then
                    motor:Set(intakePID:update(getAngle()))
                elseif intakePID:update(getAngle()) < 0 then
                    motor:Set(intakePID:update(getAngle()))
                else
                    motor:Set(0.0)
                end
            elseif not arm.isIntakeDeploySafe() and getAngle() > 2.7 then
                --this is so we can have the point blank shot
                motor:Set(intakePID:update(getAngle()))
            else
                motor:Set(0.0)
            end
        elseif intakeState == STOW then
            if arm.isIntakeDeploySafe() then
                if getAngle() > 2.36 then
                    motor:Set(intakePID:update(getAngle()))
                else
                    motor:Set(0.0)
                end
            else
                motor:Set(0.0)
            end
        else
            motor:Set(0.0)
        end

    --[[
    if intakeState == DEPLOYED then
        if arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif intakeState == STOW then
        if getAngle() > 2.36 and arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif intakeState == DOWN then
        if getAngle() > 2.63 then
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
    wpilib.SmartDashboard_PutNumber("Intake PID Output", intakePID:update(getAngle()))
    wpilib.SmartDashboard_PutNumber("Intake Motor Output", motor:Get())
    wpilib.SmartDashboard_PutNumber("Intake HIT", 0)
    wpilib.SmartDashboard_PutNumber("Intake Error", getTarget() - getAngle())
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
