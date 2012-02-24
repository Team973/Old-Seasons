-- turret.lua
local pid = require("pid")

local wpilib = require("wpilib")

module(...)

local HARD_LIMIT = 90

function calculateTarget(turretAngle, desiredAngle)
    --calculates shortest desired angle
    while desiredAngle - turretAngle > 180 do
        desiredAngle = desiredAngle - 360
    end
    while desiredAngle - turretAngle < -180 do
        desiredAngle = desiredAngle + 360 
    end

    --make sure the turret doesn't crash
    if desiredAngle > HARD_LIMIT then
        --desiredAngle = desiredAngle - 360
        desiredAngle = turretAngle
    end
    if desiredAngle < -HARD_LIMIT then
        --desiredAngle = desiredAngle + 360
        desiredAngle = turretAngle
    end

    return desiredAngle
end

	



-- vim: ft=lua et ts=4 sts=4 sw=4
