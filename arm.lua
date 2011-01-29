-- arm.lua


--Requires--
local config = require("config")
local wpilib = require("wpilib")
local math = require "math"

module(...)
-------------


--Variables--
local motor = config.armMotor
local PID = config.armPID
local movement = 0
local manual = true
--------------


--Functions
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


function update()
    local armVoltage = config.armPot:GetVoltage()
    local armAngle = armVoltage * (180 - 90) / (config.armVoltsIn180Degrees - config.armVoltsIn90Degrees)
    local feedForward = config.armCompensateAmplitude * math.sin(armAngle)
   if manual then
        motor:Set(movement + feedForward) --Manual + Feed Forward
   else
        motor:Set(-PID:update(config.armPot:GetVoltage()) + feedForward) --PID + Feed Forward
   end
end




--Vim Stuff
-- vim: ft=lua et ts=4 sts=4 sw=4
