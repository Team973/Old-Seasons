-- intake.lua

local wpilib = require("wpilib")
local lowered = false
local intakeSpeed = 0
local sideIntakeSpeed = 0


module(...)

function isballBlockingturret()
    return false
end

function isLowered()
    return lowered
end

function toggleRaise()
    lowered = not lowered
end

function update(turretReady)
end

function setIntakeSpeed(speed)
    if not isLowered then
        intakeSpeed = 0
    else
        intakeSpeed = speed
    end
end

function setSideIntakeSpeed(speed)
    if not isLowered then
        sideIntakeSpeed = 0
    else
        sideIntakeSpeed = speed
    end
end


-- vim: ft=lua et ts=4 sts=4 sw=4
