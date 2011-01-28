-- arm.lua


--Requires--
local config = require("config")
local wpilib = require("wpilib")
require "math.lua"

module(...)
-------------


--Varriables--
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
    feedForward = (config.armCompensateAmplitude)*(math.sin(( (config.armPot:GetVoltage()(180-90)/(config.armVoltsIn180Degrees - config.armVoltsIn90Degrees)))))
    
   --feedForward temporarily disabled
   motor:Set((movement + (-PID:update(config.armPot:GetVoltage())))) --+ feedForward)) --Manual + PID + Feed Forward
end




--Vim Stuff
-- vim: ft=lua et ts=4 sts=4 sw=4
