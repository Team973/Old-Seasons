-- arm.lua

local config = require("config")
local pid = require("pid")
local wpilib = require("wpilib")
local pairs = pairs
local ipairs = ipairs

module(...)

local encoder = wpilib.Encoder(12, 13, false)
local motor = wpilib.Talon(1)
local absoluteEncoder = wpilib.AnalogChannel(1)
local calibrationPulse = wpilib.Counter(9)

local angleOffset = 0
local prevRawAngle = 0
local prevCalibPulse = 0

local calibrationTimeOut = wpilib.Timer()
local lastCalibTime = 0
local calibrationAngle = 0.0
local calibrationStepSpeed = 2.0 -- degrees/second

local armPID = pid.new(0.05, 0, 0)
armPID.min, armPID.max = -1.0, 1.0
armPID:start()
local realTarget = armPID.target

encoder:Start()
calibrationPulse:Start()

PRESETS = {
    Shooting = { armAngle = 36.0 },
    Loading = { armAngle = 83.9 },
    Stow = { armAngle = 5 },
}

-- read config file
do
    local cfg = config.read("ARMPRESETS")
    if cfg.PRESETS then
        for k, v in pairs(cfg.PRESETS) do
            for _, name in ipairs{"armAngle"} do
                if v[name] then
                    PRESETS[k][name] = v[name]
                end
            end
        end
    end
end

local function presetLabel(key)
     return key.." Preset Angle"
end

-- Arm Preset Prints
for k,v in pairs(PRESETS) do
    wpilib.SmartDashboard_PutNumber(presetLabel(k), v.armAngle)
end

wpilib.SmartDashboard_PutBoolean("Save Presets", false)

function getTarget()
    return realTarget
end

function setTarget(target)
    realTarget = target
    if not calibrationTimeOut then
        armPID.target = target
    end
end

local function pot2deg(volts)
    local gain = (138.360 - 0.00) / (4.66 - 0.73)
    local potBottom = 0.73
    return (volts - potBottom) * gain
end

function getRawAngle()
    local degreesPerRevolution = 360
    local gearRatio = 1
    local ticksPerRevolution = 2500
    return encoder:Get() / (gearRatio * ticksPerRevolution) * degreesPerRevolution
end

function getAngle()
    return getRawAngle() + angleOffset
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setTarget(p.armAngle)
    end
end

function update()
    -- Check for calibration pulse
    local newRawAngle = getRawAngle()
    local newCalibPulse = calibrationPulse:Get()
    if newCalibPulse > prevCalibPulse then
        -- Calibration pulse encountered
        angleOffset = calibrationAngle - (prevRawAngle+newRawAngle)/2
        armPID.target = realTarget
        calibrationTimeOut = nil
    end

    if calibrationTimeOut and armPID.target ~= realTarget then
        -- Run arm PID slowly until first calibration pulse
        calibrationTimeOut:Start()
        local t = calibrationTimeOut:Get()
        local maxTargetStep = calibrationStepSpeed * (t - lastCalibTime)
        if armPID.target + maxTargetStep < realTarget then
            armPID.target = armPID.target + maxTargetStep
        elseif armPID.target - maxTargetStep > realTarget then
            armPID.target = armPID.target - maxTargetStep
        else
            armPID.target = realTarget
        end
        lastCalibTime = t

        -- Time out
        if t > 5 then
            armPID.target = realTarget
            calibrationTimeOut = nil
        end
    end

    -- Run 
    motor:Set(-armPID:update(newRawAngle + angleOffset))

    prevCalibPulse = newCalibPulse
    prevRawAngle = newRawAngle
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Arm Angle", getAngle())
    wpilib.SmartDashboard_PutNumber("Arm PID Output", armPID.output)
    wpilib.SmartDashboard_PutNumber("Potentiometer Output", absoluteEncoder:GetVoltage())
    wpilib.SmartDashboard_PutNumber("pot2deg", pot2deg(absoluteEncoder:GetVoltage()))

    -- Arm Preset Gets
    for k,v in pairs(PRESETS) do
        v.armAngle = wpilib.SmartDashboard_GetNumber(presetLabel(k))
    end

    local savePresets = wpilib.SmartDashboard_GetBoolean("Save Presets")

    -- Change Arm Presets
    if savePresets then
        wpilib.SmartDashboard_PutBoolean("Save Presets", false)
        config.write("ARMPRESETS", {PRESETS=PRESETS})
    end
end
