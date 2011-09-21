-- drive.lua

local math = require("math")

local pairs = pairs

module(...)

--[[
    calculate computes the wheel angles and speeds.

    x and y are the strafe inputs from the operator, w is the rotation speed
    about the z-axis.

    gyroDeg is the field-centric shift (in degrees).

    The units for wheelBase and trackWidth are irrelevant as long as they are
    consistent.

    (Consult Adam for the math.)
--]]
function calculate(x, y, w, gyroDeg, wheelBase, trackWidth)
    local r = math.sqrt(wheelBase ^ 2 + trackWidth ^ 2)
    local gyroRad = math.rad(gyroDeg)

    x, y = y * math.cos(gyroRad) + x * math.sin(gyroRad), -y * math.sin(gyroRad) + x * math.cos(gyroRad)
    local a = x - w * (wheelBase / r)
    local b = x + w * (wheelBase / r)
    local c = y - w * (trackWidth / r)
    local d = y + w * (trackWidth / r)

    local wheels = {frontRight={}, frontLeft={}, rearRight={}, rearLeft={}}
    wheels.frontRight.speed = math.sqrt(b * b + c * c)
    wheels.frontLeft.speed = math.sqrt(b * b + d * d)
    wheels.rearLeft.speed = math.sqrt(a * a + d * d)
    wheels.rearRight.speed = math.sqrt(a * a + c * c)
    wheels.frontRight.angleDeg = math.deg(math.atan2(b, c))
    wheels.frontLeft.angleDeg = math.deg(math.atan2(b, d))
    wheels.rearRight.angleDeg = math.deg(math.atan2(a, d))
    wheels.rearLeft.angleDeg = math.deg(math.atan2(a, c))

    -- Normalize wheel speeds
    local maxSpeed = math.max(
        wheels.frontRight.speed,
        wheels.frontLeft.speed,
        wheels.rearRight.speed,
        wheels.rearLeft.speed
    )
    if maxSpeed > 1 then
        for _, wheel in pairs(wheels) do
            wheel.speed = wheel.speed / maxSpeed
        end
    end

    return wheels
end

function angleError(current, target)
    local delta = target - current
    if delta > 180 then
        return delta - 360
    elseif delta < -180 then
        return delta + 360
    else
        return delta
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
