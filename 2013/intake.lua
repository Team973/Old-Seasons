-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")

module(...)

local lowered = false

local solenoidOn = wpilib.Solenoid(8)
local solenoidOff = wpilib.Solenoid(2)

function setLowered(val)
    if lowered ~= val and arm.isIntakeDeploySafe() then
        lowered = val
    end
end

function toggleRaise()
    setLowered(not lowered)
end

function update()
    solenoidOn:Set(lowered)
    solenoidOff:Set(not lowered)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
