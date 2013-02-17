-- drive.lua

local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

module(...)

local gyro = nil
local gyroOkay = true
local ignoreGyro = false

local function limit(x)
    if x > 1 then
        return 1
    elseif x < -1 then
        return -1
    else
        return x
    end
end

local function signSquare(x)
    if x < 0 then return -x*x end
    return x*x
end

local function arcade(move, rotate)
    move = signSquare(limit(move))
    rotate = signSquare(limit(rotate))
    if move < 0 then
        if rotate > 0 then
            return -move - rotate, math.max(-move, rotate)
        else
            return math.max(-move, -rotate), -move + rotate
        end
    else
        if rotate > 0 then
            return -math.max(move, rotate), -move + rotate
        else
            return -move - rotate, -math.max(move, -rotate)
        end
    end
end

function initGyro()
    gyro = wpilib.Gyro(1, 1)
    gyro:SetSensitivity(0.00703)
    gyro:Reset()
    gyroOkay = true
    wpilib.SmartDashboard_PutBoolean("Gyro Okay", true)
end

function resetGyro()
    gyro:Reset()
    ignoreGyro = false
end

function effTheGyro()
    ignoreGyro = true
end

function getGyroAngle()
    if not gyroOkay or ignoreGyro then
        return 0
    end
    return -gyro:GetAngle()
end

function disableGyro()
    gyroOkay = false
    wpilib.SmartDashboard_PutBoolean("Gyro Okay", false)
end


local leftDriveMotor = wpilib.Talon(1)
local rightDriveMotor = wpilib.Talon(2)

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

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

function update(driveX, driveY, quickTurn)
	local leftSpeed, rightSpeed = cheesyDrive(driveY, driveX, true, quickTurn)
	leftDriveMotor:Set(-leftSpeed)
	rightDriveMotor:Set(rightSpeed)
end

--[[
Team 254's drive code. (read as: Austin magic).

throttle : number
wheel : number
highGear : boolean
quickTurn : boolean

returns: leftSpeed, rightSpeed
--]]
local oldWheel = 0.0
local negInertiaAccumulator = 0.0
local quickStopAccumulator = 0.0
local accum
function cheesyDrive(throttle, wheel, highGear, quickTurn)
    -- constants, feel free to tune
    local turnNonlinHigh = 0.9
    local turnNonlinLow = 0.8
    local negInertiaHigh = 0.5
    local senseHigh = 1.2
    local senseLow = 1.1
    local senseCutoff = 0.1
    local negInertiaLowMore = 2.5
    local negInertiaLowLessExt = 5.0
    local negInertiaLowLess = 3.0
    local negInertiaWheelExtThreshold = 0.65
    local quickStopTimeConstant = 0.1
    local quickStopLinearPowerThreshold = 0.2
    local quickStopStickScalar = 5.0
    -- /constants

    -- shortcuts
    local sin = math.sin
    local pi_2 = math.pi / 2.0
    local fabs = math.abs
    -- /shortcuts

    local wheelNonLinearity
    local negInertia = wheel - oldWheel
    oldWheel = wheel

    if highGear then
        wheelNonLinearity = turnNonlinHigh
        -- Apply a sin function that's scaled to make it feel better
        wheel = sin(pi_2 * wheelNonLinearity * wheel) / sin(pi_2 * wheelNonLinearity)
        wheel = sin(pi_2 * wheelNonLinearity * wheel) / sin(pi_2 * wheelNonLinearity)
    else
        wheelNonLinearity = turnNonlinLow
        -- Apply a sin function that's scaled to make it feel better
        wheel = sin(pi_2 * wheelNonLinearity * wheel) / sin(pi_2 * wheelNonLinearity)
        wheel = sin(pi_2 * wheelNonLinearity * wheel) / sin(pi_2 * wheelNonLinearity)
        wheel = sin(pi_2 * wheelNonLinearity * wheel) / sin(pi_2 * wheelNonLinearity)
    end

    local sensitivity
    local negInertiaScalar
    if highGear then
        negInertiaScalar = negInertiaHigh
        sensitivity = senseHigh
    else
        if wheel * negInertia > 0 then
            negInertiaScalar = negInertiaLowMore
        elseif fabs(wheel) > negInertiaWheelExtThreshold then
            negInertiaScalar = negInertiaLowLessExt
        else
            negInertiaScalar = negInertiaLowLess
        end
        sensitivity = senseLow
        if fabs(throttle) > senseCutoff then
            sensitivity = 1 - (1 - sensitivity) / fabs(throttle)
        end
    end

    local negInertiaPower = negInertia * negInertiaScalar
    negInertiaAccumulator = negInertiaAccumulator + negInertiaPower

    wheel = wheel + negInertiaAccumulator
    negInertiaAccumulator = accum(negInertiaAccumulator)

    local linearPower = throttle

    -- Quickturn!
    local overPower
    local angularPower
    if quickTurn then
        if fabs(linearPower) < quickStopLinearPowerThreshold then
            quickStopAccumulator = (1 - quickStopTimeConstant) * quickStopAccumulator + quickStopTimeConstant * limit(wheel) * quickStopStickScalar
        end
        overPower = 1.0
        sensitivity = 1.0
        angularPower = wheel
    else
        overPower = 0.0
        angularPower = fabs(throttle) * wheel * sensitivity - quickStopAccumulator
        quickStopAccumulator = accum(quickStopAccumulator)
    end

    -- Compute left/right speeds
    local leftPwm = linearPower + angularPower
    local rightPwm = linearPower - angularPower

    if leftPwm > 1.0 then
        rightPwm = rightPwm - (overPower * (leftPwm - 1))
        leftPwm = 1.0
    elseif rightPwm > 1.0 then
        leftPwm = leftPwm - (overPower * (rightPwm - 1))
        rightPwm = 1.0
    elseif leftPwm < -1.0 then
        rightPwm = rightPwm + (overPower * (-1 - leftPwm))
        leftPwm = -1.0
    elseif rightPwm < -1.0 then
        leftPwm = leftPwm + (overPower * (-1 - rightPwm))
        rightPwm = -1.0
    end
    return -leftPwm, -rightPwm
end
function accum(val)
    if val > 1 then
        return val - 1
    elseif val < -1 then
        return val + 1
    else
        return 0
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
