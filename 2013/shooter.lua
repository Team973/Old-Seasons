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

local conveyer = wpilib.Victor(6)
local roller = wpilib.Talon(3)
local flywheelMotor = wpilib.Talon(2)
local flywheelCounter1 = wpilib.Counter(wpilib.DigitalInput(8))
local flywheelCounter2 = wpilib.Counter(wpilib.DigitalInput(7))
local flywheelTicksPerRevolution = 1.0

flywheelCounter1:Start()
flywheelCounter2:Start()

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
    local pointRPM = 5000

    if rpm > dangerRPM then
        return 0
    elseif rpm < pointRPM then
        return 1
    else
        return 0.8
    end
end

function update()
    measuredRPM = 60.0 / (flywheelCounter1:GetPeriod() * flywheelTicksPerRevolution)
    measuredRPM2 = 60.0 / (flywheelCounter2:GetPeriod() * flywheelTicksPerRevolution)

    if firing then
        flywheelMotor:Set(-RPMcontrol(measuredRPM))
    else
        flywheelMotor:Set(0.0)
    end

    if conveyerSpeed == 0 and rollerSpeed == 0 then
         if feeding then
            conveyer:Set(-0.25)
            roller:Set(1)
        elseif loading then
            conveyer:Set(-0.5)
            roller:Set(0.0)
            -- Locked out so we can't run it during human loading
            flywheelMotor:Set(0.0)
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
    wpilib.SmartDashboard_PutNumber("RAW BANNER", flywheelCounter:Get())
end

-- vim: ft=lua et ts=4 sts=4 sw=4
