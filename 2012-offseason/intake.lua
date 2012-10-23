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
local verticalSpeed = 0
local cheaterSpeed = 0
local repack = false

local verticalConveyer = linearize.wrap(wpilib.Victor(6))
local cheaterRoller = wpilib.Victor(5)
local frontIntake = wpilib.Victor(3)
local intakeSolenoid = wpilib.Solenoid(2)

function setRepack(val)
    repack = val
end

function setVerticalSpeed(speed)
    verticalSpeed = speed
end

function setCheaterSpeed(speed)
    cheaterSpeed = speed
end

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

function update(turretReady)
    local dashboard = wpilib.SmartDashboard_GetInstance()

    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)
    if repack then
        verticalSpeed = -1
        cheaterRoller:Set(1)
    else
        cheaterRoller:Set(cheaterSpeed)
    end

    verticalConveyer:Set(verticalSpeed)
end

function setIntake(speed)
    frontSpeed = speed
end

function intakeStop()
    frontSpeed = 0
    verticalSpeed = 0
    cheaterSpeed = 0
end

function fullStop()
    verticalConveyer:Set(0.0)
    cheaterRoller:Set(0.0)
    frontIntake:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
