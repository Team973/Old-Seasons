-- arm.lua

local config = require("config")
local pid = require("pid")
local wpilib = require("wpilib")

module(...)

local encoder = wpilib.Encoder(1, 2, true)
local motor = wpilib.Talon(8)
local absoluteEncoder = wpilib.AnalogChannel(1)

local angleOffset = 0

local armPID = pid.new(0.05, 0, 0)
armPID.min, armPID.max = -1.0, 1.0
armPID:start()

encoder:Start()

PRESETS = {
    Arm1 = { armAngle = 10 }, 
    Shooting = { armAngle = 37.1 }, 
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

function setArmTarget(target)
    armPID.target = target
end

local function pot2deg(volts)
    local gain = (138.360 - 0.00) / (4.66 - 0.73)
    local potBottom = 0.73
    return (volts - potBottom) * gain
end

function getAngle()
    return encoder:Get() / 50 * 3 + angleOffset
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
end
