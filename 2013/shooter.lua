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
local flywheelCounter = wpilib.Counter(wpilib.DigitalInput(1, 3))
local flywheelTicksPerRevolution = 1.0

flywheelCounter:Start()

local feeding = false
local firing = false
local measuredRPM = 0

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

local function RPMcontrol(rpm)
    local dangerRPM = 8000
    local pointRPM = 6000

    if rpm > dangerRPM then
        return 0
    elseif rpm < pointRPM then
        return 1
    else
        return 0
    end
end

function update()
    measuredRPM = 60.0 / (flywheelCounter:Get() * flywheelTicksPerRevolution)

    if firing then
        flywheelMotor:Set(RPMcontrol(measuredRPM))
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

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("RPM Bang-Bang control", RPMcontrol(measuredRPM))
    wpilib.SmartDashboard_PutNumber("Flywheel RPM", measuredRPM)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
