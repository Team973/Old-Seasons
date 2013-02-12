-- shooter.lua

local ipairs = ipairs

local bit = require("bit")
local intake = require("intake")
local pid = require("pid")
local linearize = require("linearize")
local math = require("math")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local conveyerSpeed = 0
local rollerSpeed = 0
local flywheelSpeed = 0

local Conveyer = wpilib.Victor(5)
local Roller = wpilib.Talon(6) 
local flywheelMotor = wpilib.Talon(7)

local flywheelLights = wpilib.Relay(1, 7, wpilib.Relay_kForwardOnly)
local lightTimer = wpilib.Timer()
local lightFlashOn = true

function setConveyerSpeed(speed)
    conveyerSpeed = speed
end

function setRollerSpeed(speed)
    rollerSpeed = speed
end

function setFlywheelSpeed(speed)
    flywheelSpeed = speed
end

function update()

    --Flywheel nonPID setup
    flywheelMotor:Set(flywheelSpeed)


    --Conveyer/Roller
    Conveyer:Set(conveyerSpeed)
    Roller:Set(rollerSpeed)
end

function fullStop()
    flywheelMotor:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
