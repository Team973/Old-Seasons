-- arm.lua

local config = require("config")
local wpilib = require("wpilib")

module(...)

local motor = config.armMotor
local PID = config.armPID
local movement = 0
local manual = true

function init()
    PID:reset()
    PID:start()
    setPreset(1)
end

function setManualMode(on)
    manual = on
end

function setMovement(delta)
    movement = delta
end

function setPreset(preset)
    PID.target = config.armPresets[preset]
end

function update()
    if manual then
        motor:Set(movement)
    else
        motor:Set(-PID:update(config.armPot:GetVoltage()))
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
