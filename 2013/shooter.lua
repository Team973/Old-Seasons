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
local conveyerEncoder = wpilib.Encoder(10, 11)
local hardStop = wpilib.Solenoid(4)
local humanLoadFlap = wpilib.Solenoid(5)
local flapActivated = false
local hardStopActivated = true
local conveyerDistance = math.pi * 1.5

flywheelCounter1:Start()
flywheelCounter2:Start()
conveyerEncoder:Start()

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

local function RPMcontrol(rpm)
    local dangerRPM = 8000
    local pointRPM = 6000

    if rpm > dangerRPM then
        return 0
    elseif rpm < pointRPM then
        return 1
    else
        return 0.6
    end
end

-- Return RPM of a flywheel counter.
local function computeFlywheelSpeed(counter)
    return 60.0 / (counter:GetPeriod() * flywheelTicksPerRevolution)
end

local function getFlywheelSpeed()
    return computeFlywheelSpeed(flywheelCounter1)
end

function setFlapActive(bool)
    flapActivated = bool
end

function setHardStopActive(bool)
    hardStopActivated = bool
end

function update()
    conveyerSpeed = conveyerEncoder:Get() / conveyerDistance

    humanLoadFlap:Set(flapActivated)
    hardStop:Set(hardStopActivated)

    if firing then
        flywheelMotor:Set(-RPMcontrol(getFlywheelSpeed()))
    else
        flywheelMotor:Set(0.0)
    end

    if conveyerSpeed == 0 and rollerSpeed == 0 then
         if feeding then
            roller:Set(1)
        elseif loading then
            conveyer:Set(-1.0)
            roller:Set(0.2)
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
    local flywheelSpeed = getFlywheelSpeed()
    wpilib.SmartDashboard_PutNumber("RPM Bang-Bang control", RPMcontrol(flywheelSpeed))
    wpilib.SmartDashboard_PutNumber("Flywheel RPM", flywheelSpeed)
    wpilib.SmartDashboard_PutNumber("RAW BANNER", flywheelCounter1:Get())
end

-- vim: ft=lua et ts=4 sts=4 sw=4
