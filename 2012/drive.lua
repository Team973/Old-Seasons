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

    y = x * math.cos(gyroRad) + y * math.sin(gyroRad)
    x = -x * math.sin(gyroRad) + y * math.cos(gyroRad)
    local a = y - w * (wheelBase / r)
    local b = y + w * (wheelBase / r)
    local c = x - w * (trackWidth / r)
    local d = x + w * (trackWidth / r)

    local wheels = {rightFront={}, leftFront={}, rightBack={}, leftBack={}}
    wheels.rightFront.speed = math.sqrt(b * b + c * c)
    wheels.leftFront.speed = math.sqrt(b * b + d * d)
    wheels.leftBack.speed = math.sqrt(a * a + d * d)
    wheels.rightBack.speed = math.sqrt(a * a + c * c)
    wheels.rightFront.angleDeg = math.deg(math.atan2(b, c))
    wheels.leftFront.angleDeg = math.deg(math.atan2(b, d))
    wheels.leftBack.angleDeg = math.deg(math.atan2(a, d))
    wheels.rightBack.angleDeg = math.deg(math.atan2(a, c))

    -- Normalize wheel speeds
    local maxSpeed = math.max(
        wheels.rightFront.speed,
        wheels.leftFront.speed,
        wheels.rightBack.speed,
        wheels.leftBack.speed
    )
    if maxSpeed > 1 then
        for _, wheel in pairs(wheels) do
            wheel.speed = wheel.speed / maxSpeed
        end
    end

    return wheels
end

-- Wraps an angle (in degrees) to (-180, 180].
function normalizeAngle(theta)
    while theta > 180 do
        theta = theta - 360
    end
    while theta < -180 do
        theta = theta + 360
    end
    return theta
end

-- Calculate the error and the flip of the motor.
function calculateTurn(current, target)
    local err, flip = normalizeAngle(target - current), false
    if math.abs(err) > 90 then
        err, flip = normalizeAngle(err + 180), true
    end
    return err, flip
end

function angleError(current, target)
    local err, flip = calculateTurn(current, target)
    return err
end

function driveScale(err, flip)
    local scale
    if math.abs(err) < 45 then
        scale = math.cos(math.rad(err))
    else
        scale = 0
    end
    if flip then
        scale = -scale
    end
    return scale
end

-- vim: ft=lua et ts=4 sts=4 sw=4
