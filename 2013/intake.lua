-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")

module(...)

local deploy = false
local retract = false
local intakeDeploySpeed = .5
local intakeSpeed = 0

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)

function setIntakeSpeed(speed)
    intakeSpeed = speed
end

function setDeploy(val)
    if deploy ~= val and arm.isIntakeDeploySafe() then
        deploy = val
    end
end

function setRetract(val)
    if retract ~= val and arm.isIntakeDeploySafe() then
        retract = val
    end
end

local intakeTimer = wpilib.Timer()
intakeTimer:Start()

function update()
    intakeRollers:Set(intakeSpeed)

    intakeTimer:Reset()
    if deploy and intakeTimer:Get() <= 1 then
        motor:Set(intakeDeploySpeed)
    elseif retract and intakeTimer:Get() <= 1 then
        motor:Set(-intakeDeploySpeed)
    else
        motor:Set(0.0)
    end
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
