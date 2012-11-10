-- intake.lua

local ipairs = ipairs
local type = type
local io = require("io")
local math = require("math")
local linearize = require("linearize")
local pid = require("pid")
local string = require("string")
local wpilib = require("wpilib")

local ROBOTNAME = ROBOTNAME

module(...)

local lowered = false
local frontSpeed = 0 -- front intake roller
local verticalSpeed = 0
local repack = false

local verticalConveyer = linearize.wrap(wpilib.Victor(6))
local cheaterRoller = linearize.wrap(wpilib.Victor(5))
local frontIntake = linearize.wrap(wpilib.Victor(3))
local intakeSolenoid = wpilib.Solenoid(2)
local intakeLights = wpilib.Relay(1, 7, wpilib.Relay_kReverseOnly)

local ballChecker
if ROBOTNAME == "viper" then
    ballChecker = wpilib.Counter(wpilib.DigitalInput(4))
    ballChecker:Start()
    ballChecker:SetMaxPeriod(1.0)
else
    ballChecker = wpilib.DigitalInput(4)
end

function setRepack(val)
    repack = val
end

function setVerticalSpeed(speed)
    verticalSpeed = speed
end

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

function update(turretReady)
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)

    if repack then
        verticalSpeed = -1
        cheaterRoller:Set(1)
    elseif math.abs(frontSpeed) > math.abs(verticalSpeed) then
        cheaterRoller:Set(frontSpeed)
    else
        cheaterRoller:Set(verticalSpeed)
    end

    verticalConveyer:Set(verticalSpeed)

    local lightsOn = false
    if ROBOTNAME == "viper" then
        lightsOn = frontSpeed > 0.0 and ballChecker:GetStopped()
    else
        lightsOn = not ballChecker:Get()
    end
    if lightsOn then
        intakeLights:Set(wpilib.Relay_kOn)
    else
        intakeLights:Set(wpilib.Relay_kOff)
    end

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
