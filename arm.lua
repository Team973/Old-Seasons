-- arm.lua

local config = require("config")
local math = require("math")
local wpilib = require("wpilib")

module(...)

local motor = config.armMotor
local PID = config.armPID
local movement = 0
local manual = true
local clawOpen = true

local wristSpeedSet = 0
local gripSpeedSet = 0

function init()
    PID:reset()
    PID:start()
    setPreset(1)
end

function setManual(on)
    manual = on
end

function setMovement(delta)
    movement = delta
end

function setPreset(preset)
    PID.target = config.armPresets[preset]
end

local function getArmAngle()
    local voltage = config.armPot:GetVoltage()
    local scale = 90 / (config.armPos180 - config.armPos90)
    local offset = 90 - scale * config.armPos90
    return voltage * scale + offset
end

local function calculateFeedForward()
    return config.armDriveBackAmplitude * math.sin(getArmAngle())
end

function setGripMotor(speed)
    gripSpeedSet = speed
end

function setWristMotor(speed)
    wristSpeedSet = speed
end

function openClaw() clawOpen = true end

function closeClaw() clawOpen = false end

function update()
    -- Primary Joint
    local motorOutput
    if manual then
        motorOutput = movement
        if movement < config.armDriveBackDeadband and movement > 0 then
            motorOutput = motorOutput + calculateFeedForward()
        end
    else
        motorOutput = -PID:update(config.armPot:GetVoltage())
    end
    motor:Set(motorOutput)

    -- Grabber
    config.gripMotor:Set(gripSpeedSet)
    config.wristMotor:Set(wristSpeedSet)
    if clawOpen then
        config.clawPiston:Set(wpilib.Relay_kForward)
    else
        config.clawPiston:Set(wpilib.Relay_kReverse)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
