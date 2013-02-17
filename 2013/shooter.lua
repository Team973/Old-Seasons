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

local conveyer = wpilib.Victor(5)
local roller = wpilib.Talon(6)
local flywheelMotor = wpilib.Talon(7)

local flywheelLights = wpilib.Relay(1, 7, wpilib.Relay_kForwardOnly)
local lightTimer = wpilib.Timer()
local lightFlashOn = true
local feeding = false
local firing = false

function fire(bool)
    firing = bool
end

function feed(bool)
    feeding = bool
end

function humanLoad(bool)
    loading = bool
end

function setConveyerManual(speed)
    conveyerSpeed = speed
end

function setRollerManual(speed)
    rollerSpeed = speed
end

function update()
    if firing then
        flywheelMotor:Set(-1)
    else
        flywheelMotor:Set(0)
    end

    if conveyerSpeed == 0 and rollerSpeed == 0 then
         if feeding then
            conveyer:Set(-0.25)
            roller:Set(1)
        elseif loading then
            conveyer:Set(-0.5)
            roller:Set(0.0)
        else
            conveyer:Set(0)
            roller:Set(0)
        end
    else
        conveyer:Set(-conveyerSpeed)
        roller:Set(rollerSpeed)
    end

end

function fullStop()
    flywheelMotor:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
