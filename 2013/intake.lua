-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")

module(...)

local deploy = false
local retract = false
local intakeDeploySpeed = 0

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

function update()
    intakeRollers:Set(intakeSpeed)

    if retract then
        motor:Set(-intakeDeploySpeed)
    elseif deploy then
        motor:Set(intakeDeploySpeed)
    else
        motor:Set(0.0)
    end
end

function fullStop()
    motor:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
