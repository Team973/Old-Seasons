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

local angleOffset = 0

local armPID = pid.new(0.05, 0, 0)
armPID.min, armPID.max = -1.0, 1.0
armPID:start()

encoder:Start()

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

function setArmTarget(target)
    armPID.target = target
    armTarget = target
end

function getArmTarget()
    return armTarget
end

local function pot2deg(volts)
    local gain = (138.360 - 0.00) / (4.66 - 0.73)
    local potBottom = 0.73
    return (volts - potBottom) * gain
end

function getAngle()
    local degreesPerRevolution = 360
    local gearRatio = 10 * 6
    local ticksPerRevolution = 360
    return encoder:Get() / (gearRatio * ticksPerRevolution) * degreesPerRevolution + angleOffset
end

local function calibrate()
    angleOffset = angleOffset - getAngle()
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setArmTarget(p.armAngle)
    end
end

function update()
    motor:Set(-armPID:update(getAngle()))
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
