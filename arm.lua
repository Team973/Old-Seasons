-- arm.lua

local config = require("config")
local math = require("math")
local wpilib = require("wpilib")

module(...)

local motor = config.armMotor
local PID = config.armPID
local wristPID = config.wristPID
local movement = 0
local manual = false
local clawOpen = true
local isForward = true
local presetName = nil

local wristSpeed = 0
local gripSpeed = 0

function init()
    PID:reset()
    PID:start()
    setPreset("pickup")
end

function getManual()
    return manual
end

function setManual(on)
    if manual and not on then
        PID.target = config.armPot:GetVoltage()
        setPreset(nil)
    end
    manual = on
end

function setMovement(delta)
    movement = delta
end

local function updateTarget()
    -- If we don't have a specific preset, don't change the target.
    if not presetName then return end

    -- Look up preset value in config and change PID target
    if isForward then
        PID.target = config.armPresets.forward[presetName].arm + config.armPositionForward
    else
        PID.target = config.armPresets.reverse[presetName].arm + config.armPositionReverse
    end
end

function setForward(f)
    isForward = f
    updateTarget()
end

function setPreset(preset)
    presetName = preset
    updateTarget()
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
    gripSpeed = speed
end

function setWristMotor(speed)
    wristSpeed = speed
end

function openClaw() clawOpen = true end

function closeClaw() clawOpen = false end

function update()
    -- Primary Joint
    local motorOutput
    if manual then
        motorOutput = movement
    else
        motorOutput = -PID:update(config.armPot:GetVoltage())
    end
    motor:Set(motorOutput)

    -- Grabber
    config.gripMotor:Set(gripSpeed)
    config.wristMotor:Set(wristSpeed)
    if clawOpen then
        config.clawPiston:Set(wpilib.Relay_kForward)
    else
        config.clawPiston:Set(wpilib.Relay_kReverse)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
