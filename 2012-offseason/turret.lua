-- turret.lua

local ipairs = ipairs

local bit = require("bit")
local intake = require("intake")
local pid = require("pid")
local linearize = require("linearize")
local math = require("math")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local flywheelSpeedTable = {
    numSamples=25,
}
local flywheelSpeedFilter = {
    prev=0,
    curr=0,
    weight=.2,
}
local flywheelFired = false
flywheelPID = pid.new(0.05, 0.0, -0.0007,
    nil,
    function()
        return flywheelSpeedFilter:average()
    end)
local flywheelPIDGains = {
    {0, p=0.025, d=-0.005},
    {2800, p=0.025,d=-0.005},
    {3273, p=0.025, d=-0.005},
    {3275, p=0.025, d=-0.005},
    {3277, p=0.025, d = -.005}
}
local TURRET_ANGLE_OFFSET = 0

local currPresetName = nil
PRESETS = {
    Feeder={flywheelRPM=3000, hoodAngle=1100, targetAngle=0},
    rightKey={flywheelRPM=2150, hoodAngle=450, targetAngle=-15},
    key={flywheelRPM=2100, hoodAngle=450, superSoftHoodAngle=450, targetAngle=0, superHardHoodAngle = 450, hardHoodAngle =450},
    autoKey={flywheelRPM=6200, hoodAngle=1100, targetAngle=-TURRET_ANGLE_OFFSET},
    bridge={flywheelRPM=7000},
}

local dashboard = wpilib.SmartDashboard_GetInstance()

local flywheelTargetSpeed = PRESETS.key.flywheelRPM
local flywheelOn = false
local flywheelFeedforward = math.huge
local in4 = wpilib.DigitalInput(2, 4)
local in5 = wpilib.DigitalInput(2, 5)
local in6 = wpilib.DigitalInput(2, 6)
local flywheelCounter = wpilib.Counter(in6)
local flywheelMotor = linearize.wrap(wpilib.Victor(2, 6))
local flywheelTicksPerRevolution = 4.0
local turretEnabled = true

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
    local gain = tableStep(flywheelPIDGains, speed)
    if gain.p then flywheelPID.p = gain.p else flywheelPID.p = 0 end
    if gain.i then flywheelPID.i = gain.i else flywheelPID.i = 0 end
    if gain.d then flywheelPID.d = gain.d else flywheelPID.d = 0 end
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

function update()
    -- Update flywheel target speed from intake's squish meter
    if currPresetName then
        setFlywheelTargetSpeed(PRESETS[currPresetName].flywheelRPM)
    end

    -- Add flywheel velocity sample
    local speedSample = 60.0 / (flywheelCounter:GetPeriod() * flywheelTicksPerRevolution)
    flywheelSpeedTable:add(speedSample)
    flywheelSpeedFilter:add(speedSample)

    if flywheelSpeedTable:average() - flywheelSpeedFilter:average() > 300 then
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
            flywheelMotor:Set(flywheelOutput)
        else
            flywheelMotor:Set(0.0)
        end
    end

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
