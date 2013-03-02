-- intake.lua

local wpilib = require("wpilib")

module(...)

local lowered = false

local solenoidOn = wpilib.Solenoid(8)
local solenoidOff = wpilib.Solenoid(2)

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

function update()
    solenoidOn:Set(lowered)
    solenoidOff:Set(lowered)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
