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

--The arm Talon will be 8

--TODO Find out the real victor numbers from Allen
local Conveyer = wpilib.Victor(5)
local Roller = wpilib.Talon(6) 

local flywheelSpeedTable = {
    numSamples=25,
}
local flywheelSpeedFilter = {
    prev=0,
    curr=0,
    weight=.2,
}
local flywheelFired = false
flywheelPID = pid.new(0.025, 0.0, -0.005,
    nil,
    function()
        return flywheelSpeedFilter:average()
    end)

local currPresetName = nil

PRESETS = {
    Feeder={flywheelRPM=2150, hoodAngle=1100, targetAngle=0},
    rightKey={flywheelRPM=2150, hoodAngle=450, targetAngle=-15},
    key={flywheelRPM=2150, hoodAngle=450, superSoftHoodAngle=450, targetAngle=0, superHardHoodAngle = 450, hardHoodAngle =450},
    autoKey={flywheelRPM=2150, hoodAngle=1100, targetAngle=0},
    bridge={flywheelRPM=2150},
}

local dashboard = wpilib.SmartDashboard_GetInstance()
local flywheelTargetSpeed = PRESETS.key.flywheelRPM
local flywheelOn = false
local flywheelFeedforward = math.huge
local flywheelCounter = wpilib.Counter(wpilib.DigitalInput(3))
local flywheelMotor = linearize.wrap(wpilib.Talon(7))
local flywheelTicksPerRevolution

local flywheelLights = wpilib.Relay(1, 7, wpilib.Relay_kForwardOnly)
local lightTimer = wpilib.Timer()
local lightFlashOn = true

flywheelCounter:Start()

local HARD_LIMIT = 90

-- Calculate the current flywheel speed (in RPM).
function getFlywheelSpeed()
    return flywheelSpeedTable:average()
end

function flywheelSpeedTable:add(val)
    if #self < self.numSamples then
        self[#self + 1] = val
    else
        for i = 1, #self - 1 do
            self[i] = self[i + 1]
        end
        self[#self] = val
    end
end

function flywheelSpeedTable:average()
    local sum = 0.0
    for _, val in ipairs(self) do
        sum = sum + val
    end
    return sum / #self
end

function getFlywheelFilterSpeed()
    return flywheelSpeedFilter:average()
end

function flywheelSpeedFilter:add(val)
    self.prev, self.curr = self:average(), val
end

function flywheelSpeedFilter:average()
    return (1 - self.weight) * self.prev + self.weight * self.curr
end

function clearFlywheelFired()
    flywheelFired = false
end

function getFlywheelFired()
    return flywheelFired
end

local function tableStep(t, x)
    if x < t[1][1] then
        return t[1]
    end
    for i = 2, #t do
        if x >= t[i - 1][1] and x < t[i][1] then
            return t[i - 1]
        end
    end
    return t[#t]
end

-- Retrieve the target flywheel speed
function getFlywheelTargetSpeed(speed)
    return flywheelTargetSpeed
end

-- Change the target flywheel speed
function setFlywheelTargetSpeed(speed)
    flywheelTargetSpeed = speed
end

function resetFlywheel()
    flywheelCounter:Reset()
    flywheelPID.target = 0
end

function runFlywheel(on, speed)
    if not flywheelOn and on then
        resetFlywheel()
    end
    flywheelOn = on
    if speed then
        setFlywheelTargetSpeed(speed)
    end
end

function setConveyerSpeed(speed)
    conveyerSpeed = speed
end

function setRollerSpeed(speed)
    rollerSpeed = speed
end

function update()
    if currPresetName then
        setFlywheelTargetSpeed(PRESETS[currPresetName].flywheelRPM)
    end

    -- Add flywheel velocity sample
    local speedSample = 60.0 / (flywheelCounter:GetPeriod() * flywheelTicksPerRevolution)
    flywheelSpeedTable:add(speedSample)
    flywheelSpeedFilter:add(speedSample)

    if flywheelSpeedTable:average() - flywheelSpeedFilter:average() > 25 then
        flywheelFired = true
    end

    -- Get flywheel position and time
    flywheelPID.timer:Start()
    local pos = flywheelCounter:Get() / flywheelTicksPerRevolution -- in revolutions
    local dt = flywheelPID.timer:Get() / 60.0 -- in minutes
    flywheelPID.timer:Reset()

    -- Update flywheel PID
    do
        local speed = flywheelTargetSpeed
        if not flywheelOn then
            speed = 0
        end

        flywheelPID.target = flywheelPID.target + speed * dt
        local extraTerm = speed * (1/flywheelFeedforward - flywheelPID.d)
        flywheelPID.target = math.min(pos - (1 - flywheelPID.d - extraTerm) / flywheelPID.p, flywheelPID.target)
        local flywheelOutput = flywheelPID:update(pos, dt) + extraTerm
        if flywheelOutput > 0.0 then
            flywheelMotor:Set(-flywheelOutput)
        else
            flywheelMotor:Set(0.0)
        end
    end

    -- Lights!
    lightTimer:Start()
    if lightTimer:HasPeriodPassed(1) then
        lightFlashOn = not lightFlashOn
    end
    if flywheelOn and math.abs(flywheelSpeedTable:average() - flywheelSpeedFilter:average()) < 10 then
        flywheelLights:Set(wpilib.Relay_kOn)
    else
        flywheelLights:Set(wpilib.Relay_kOff)
    end

    --Conveyer/Roller
    Conveyer:Set(conveyerSpeed)
    Roller:Set(rollerSpeed)

    -- Print flywheel diagnostics
    dashboard:PutDouble("Flywheel Speed", getFlywheelSpeed())
    dashboard:PutInt("Flywheel Speed(Int)", getFlywheelSpeed())
    dashboard:PutDouble("Flywheel Target Speed", getFlywheelTargetSpeed())
end

function fullStop()
    flywheelMotor:Set(0.0)
end

function setPreset(name)
    currPresetName = name
    local p = PRESETS[name]
    if not p then return end

    if p.flywheelRPM then
        setFlywheelTargetSpeed(p.flywheelRPM)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
