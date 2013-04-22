-- arm.lua

local config = require("config")
local pid = require("pid")
local wpilib = require("wpilib")
local string = require("string")
local pairs = pairs
local ipairs = ipairs

module(...)

local encoder = wpilib.Encoder(11, 10, false)
-- 12, 13
local motor = wpilib.Talon(1)
local calibrationPulse = wpilib.Counter(9)

local angleOffset = 0
local prevRawAngle = 0
local prevCalibPulse = 0

local calibrationAngle = 0.0

local armPID = pid.new(0.07, 0.01, 0)
armPID.min, armPID.max = -1.0, 1.0
armPID.icap = .025
armPID:start()

encoder:Start()
calibrationPulse:Start()

PRESETS = {
    Shooting = { armAngle = 16.7 },
    sideShot = { armAngle = 34.12 },
    -- also use 35 if it doesn't work
    autoShot = { armAngle = 34.12 },
    -- 33.4 Silicon Valley
    Loading = { armAngle = 80.5 },
    Stow = { armAngle = 5.2 },
    Horizontal = { armAngle = 59.2 },
    Intake = { armAngle = 0.0 },
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
    return armPID.target
end

function setTarget(target)
    armPID.target = target
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

function isIntakeDeploySafe()
    return getAngle() > 30
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setTarget(p.armAngle)
    end
end

function update()
    local newRawAngle = getRawAngle()
    local newCalibPulse = calibrationPulse:Get()
    --[[
    TODO(ross)

    if newCalibPulse > prevCalibPulse then
        -- Calibration pulse encountered
        angleOffset = calibrationAngle - (prevRawAngle+newRawAngle)/2
    end
    --]]

    motor:Set(-armPID:update(newRawAngle + angleOffset))

    prevCalibPulse = newCalibPulse
    prevRawAngle = newRawAngle
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Arm Angle", getAngle())
    wpilib.SmartDashboard_PutNumber("Arm PID Output", armPID.output)

    wpilib.DriverStationLCD_GetInstance():PrintLine(wpilib.DriverStationLCD_kUser_Line2, string.format("Arm Angle: %.2f", getAngle()))
    wpilib.DriverStationLCD_GetInstance():UpdateLCD()

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
