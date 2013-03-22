-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")

module(...)

local lowered = false
local intakeDeploySpeed = .5

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)

function setIntakeSpeed(speed)
    intakeSpeed = speed
end

function setIntakeDeploy(val)
end

function setLowered(val)
    if lowered ~= val and arm.isIntakeDeploySafe() then
        lowered = val
    end
end

function toggleRaise()
    setLowered(not lowered)
end

function update()
    intakeRollers:Set(intakeSpeed)
    motor:Set(intakeDeploySpeed)
    motor:Set(0)
end

function fullStop()
    motor:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
