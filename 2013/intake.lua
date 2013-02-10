-- intake.lua

local ipairs = ipairs
local type = type
local io = require("io")
local math = require("math")
local linearize = require("linearize")
local pid = require("pid")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local lowered = false
local frontSpeed = 0 -- front intake roller

local frontIntake = linearize.wrap(wpilib.Victor(3))
local otherIntake = linearize.wrap(wpilib.Victor(4))
local intakeSolenoid = wpilib.Solenoid(2)

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

function update()
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)
end

function setIntake(speed)
    frontSpeed = speed
end

function intakeStop()
    frontSpeed = 0
end

function fullStop()
    frontIntake:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
