-- drive.lua

local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

module(...)

local leftDriveMotor = wpilib.Talon(8)
local rightDriveMotor = wpilib.Talon(7)
local leftCurrent = wpilib.AnalogChannel(2)
local rightCurrent = wpilib.AnalogChannel(3)
local kickUpOn = wpilib.Solenoid(2)
local kickUpOff = wpilib.Solenoid(8)

--auto pid
local followerWheels = wpilib.Solenoid(3)
local lowered = false
local rightEncoder = wpilib.Encoder(5, 6, false)
local leftEncoder = wpilib.Encoder(3, 4, false)
local colinGyro = wpilib.Encoder(2, 1, false)
colinGyro:Start()
rightEncoder:Start()
leftEncoder:Start()

local arcade, cheesyDrive

local gyro = nil
local gyroOkay = true
local ignoreGyro = false

local punching = false
function punchTheGround(bool)
    punching = bool
end

function getLeftDrive()
    return leftEncoder:Get()
end

function getRightDrive()
    return rightEncoder:Get()
end

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
end

function resetGyro()
    gyro:Reset()
    ignoreGyro = false
end

function effTheGyro()
    ignoreGyro = true
end

function disableGyro()
    gyroOkay = false
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

-- These are all functions for the auto pid
function getWheelDistance()
    local diameter = 2.75
    local encoderTicks = 360
    --comp is still 360
    local distancePerRevolution = math.pi * diameter
     rightDist = (rightEncoder:Get() / encoderTicks) * distancePerRevolution
     leftDist = (leftEncoder:Get() / encoderTicks) * distancePerRevolution
    return (rightDist + leftDist) / 2
end

function resetEncoders()
    rightEncoder:Reset()
    leftEncoder:Reset()
end

--[[
function getDriveVelocity()
    local diameter = 2.75
    local encoderTicks = 360
    leftEncoderPeriod = leftEncoder:GetPeriod()
    rightEncoderPeriod = rightEncoder:GetPeriod()
    leftVelocity = 2 * math.pi^2 * (diameter / encoderTicks * leftEncoderPeriod)
    rightVelocity = 2 * math.pi^2 * (diameter / encoderTicks * rightEncoderPeriod)
    return (leftVelocity + rightVelocity) / 2
end
]]

function getRightDist()
    return rightDist
end

function getLeftDist()
    return leftDist
end

function resetColinGyro()
    colinGyro:Reset()
end

function getGyroAngle()
    local encoderTicks = 1024
    local currGyro =  colinGyro:Get() * (360 / encoderTicks)
    while currGyro > 180 do
        currGyro = currGyro - 360
    end
    while currGyro < -180 do
        currGyro = currGyro + 360
    end
    return currGyro
end

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

function update(driveX, driveY, quickTurn)
    local leftSpeed, rightSpeed
    if wpilib.IsAutonomous()then
        leftSpeed, rightSpeed = arcade(driveX, driveY) 
        followerWheels:Set(true)
    else
        --[[
        leftSpeed, rightSpeed = cheesyDrive(driveY, driveX, true, quickTurn)
        followerWheels:Set(false)
        --]]
        leftSpeed, rightSpeed = arcade(driveX, driveY) 
        followerWheels:Set(false)
    end

    kickUpOn:Set(punching)
    kickUpOff:Set(not punching)
	leftDriveMotor:Set(-leftSpeed)
	rightDriveMotor:Set(rightSpeed)
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Left Drive Current", leftCurrent:GetVoltage())
    wpilib.SmartDashboard_PutNumber("Right Drive Current", rightCurrent:GetVoltage())
    wpilib.SmartDashboard_PutNumber("Distance traveled", getWheelDistance())
    wpilib.SmartDashboard_PutNumber("right drive encoder", rightEncoder:Get())
    wpilib.SmartDashboard_PutNumber("left drive encoder", leftEncoder:Get())
    wpilib.SmartDashboard_PutNumber("Gyro Angle Raw", colinGyro:Get())
    wpilib.SmartDashboard_PutNumber("Gyro Angle", getGyroAngle())
    wpilib.SmartDashboard_PutNumber("left dist", getLeftDist())
    wpilib.SmartDashboard_PutNumber("right dist", getRightDist())
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
    local negInertiaHigh = 0
    local senseHigh = 0.9
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

function getLeftDrive()
    return leftDriveMotor:Get()
end

function getRightDrive()
    return rightDriveMotor:Get()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
