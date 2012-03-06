-- turret.lua

local pid = require("pid")
local math = require("math")
local wpilib = require("wpilib")

module(...)

flywheelPID = pid.new(0.05, 0.0, 0.001)
local flywheelSpeed = 0.0
local flywheelPrevPos = 0.0
local flywheelSpeedTimer = wpilib.Timer()

local flywheelTargetSpeed = 0.0
local flywheelFeedforward = 7800
local flywheelCounter = wpilib.Counter(2, 6)
local flywheelMotor = wpilib.Victor(2, 6)
local flywheelTicksPerRevolution = 6.0

flywheelCounter:Start()

encoder = wpilib.Encoder(2, 2, 2, 3, true, wpilib.CounterBase_k1X)
encoder:Start()
motor = wpilib.Jaguar(2, 3) 
turnPID = pid.new(0.05, 0, 0) 

allowRotate = false

local HARD_LIMIT = 90

function getTargetAngle()
    return turnPID.target
end

function setTargetAngle(angle)
    turnPID.target = calculateTarget(encoder:Get()/25, angle)
end

function setFromJoy(x,y)
    if allowRotate then
        local angle = math.atan2(x, y)
        angle = angle*180/math.pi
        turnPID.target = calculateTarget(encoder:Get()/25, angle)
    end
end

-- Calculate the current flywheel speed (in RPM).
function getFlywheelSpeed()
    return flywheelSpeed
end

-- Retrieve the target flywheel speed
function getFlywheelTargetSpeed(speed)
    return flywheelTargetSpeed
end

-- Change the target flywheel speed
function setFlywheelTargetSpeed(speed)
    flywheelTargetSpeed = speed
end

function resetFlywheel()
    flywheelCounter:Reset()
    flywheelPID.target = 0
end

function update()
    local dashboard = wpilib.SmartDashboard_GetInstance()

    dashboard:PutInt("TURN.TARGET", turnPID.target)
    dashboard:PutInt("TURN.ANGLE", encoder:Get()/25)
    turnPID:update(encoder:Get()/25)
    motor:Set(turnPID.output)

    flywheelPID.timer:Start()
    local pos = flywheelCounter:Get() / flywheelTicksPerRevolution -- in revolutions
    local dt = flywheelPID.timer:Get() / 60.0 -- in minutes
    flywheelPID.timer:Reset()

    flywheelSpeedTimer:Start()
    if flywheelSpeedTimer:HasPeriodPassed(0.5) then
        flywheelSpeed = (pos - flywheelPrevPos) / (0.5 / 60.0)
        flywheelPrevPos = pos
    end

    flywheelPID.target = flywheelPID.target + flywheelTargetSpeed * dt
    local flywheelOutput = flywheelPID:update(pos, dt) + flywheelTargetSpeed * (1/flywheelFeedforward - flywheelPID.d)
    if flywheelOutput > 0.0 then
        flywheelMotor:Set(flywheelOutput)
    else
        flywheelMotor:Set(0.0)
    end
end

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
        desiredAngle = HARD_LIMIT
    end
    if desiredAngle < -HARD_LIMIT then
        desiredAngle = -HARD_LIMIT
    end

    return desiredAngle
end

-- vim: ft=lua et ts=4 sts=4 sw=4
