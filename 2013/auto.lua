-- auto.lua

local arm = require("arm")
local drive = require("drive")
local intake = require("intake")
local pid = require("pid")
local shooter = require("shooter")
local wpilib = require("wpilib")
local math = require("math")
local coroutine = require("coroutine")

module(...)

local resetDrive, driveToPoint, calculateDrive, storeDriveCalculations

function run()
    resetDrive()

    shooter.fullStop()
    arm.setPreset("autoShot")

    while turnInPlace(15) do
        wpilib.SmartDashboard_PutString("Auto Phase", "WTF")
        coroutine.yield()
    end

    wpilib.SmartDashboard_PutString("Auto Phase", "FINISHED")

    --[[
    local shootTimer = wpilib.Timer()
    shootTimer:Start()

    while shootTimer:Get() < 4 do
        shooter.setFlywheelRunning(true)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end
--    intake.setDeploy(true)

    local t = wpilib.Timer()
    t:Start()
    while t:Get() < 1 do
        shooter.setConveyerManual(0)
        shooter.setRollerManual(1)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end

 --   intake.setDeploy(false)
    local t = wpilib.Timer()
    t:Start()
    shooter.fire()
    while t:Get() < 5 do
        shooter.setConveyerManual(0)
        shooter.setRollerManual(0)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end

  --  arm.setPreset("Intake")
    --]]



    -- Clean up
    shooter.fullStop()
    intake.setDeploy(false)
    drive.update(0, 0, false)
    intake.setIntakeSpeed(0.0)
end

local drivePID = pid.new(.3)
local anglePID = pid.new(.1)
local rotatePID = pid.new(.1)

local currTheta = 0
local currGyro = 0
local currLeft = 0
local currRight = 0
local currX = 0
local currY = 0

local prevTheta = 0
local prevGyro = 0
local prevLeft = 0
local prevRight = 0
local prevX = 0
local prevY = 0

local driveTimer

function resetDrive()
    -- XXX(ross): these values may not actually be zero at start
    prevTheta = 0
    currTheta = 0
    prevGyro = 0
    currGyro = 0
    prevLeft = 0
    currLeft = 0
    prevRight = 0
    currRight = 0
    prevX = 0
    currX = 0
    prevY = 0
    currY = 0

    drivePID:reset()
    anglePID:reset()
    rotatePID:reset()

    drivePID:start()
    anglePID:start()
    rotatePID:start()

    driveTimer = wpilib.Timer()
    driveTimer:Start()
end

function calculateDrive()
    currGyro = drive.getGyroAngle()
    while currGyro > 180 do
        currGyro = currGyro - 360
    end
    while currGyro < -180 do
        currGyro = currGyro + 360
    end
    currTheta = prevTheta + (currGyro - prevGyro)
    theta = (currTheta + prevTheta) / 2
    currLeft =  drive.getLeftDist()
    currRight = drive.getRightDist()
    magnitude = (currLeft + currRight - prevLeft - prevRight) / 2
    deltaX = -magnitude * math.sin(theta / 180 * math.pi)
    deltaY = magnitude * math.cos(theta / 180 * math.pi)
    currX = prevX + deltaX
    currY = prevY + deltaY
end

function storeDriveCalculations()
    prevGyro = currGyro
    prevTheta = currTheta
    prevLeft = currLeft
    prevRight = currRight
    prevX = currX
    prevY = currY
end

function driveToPoint(targetX, targetY, backward, drivePrecision, turnPrecision, driveCap, turnCap, arcCap)
    if not drivePrecision then drivePrecision = 6 end -- inches
    if not turnPrecision then turnPrecision = 5 end -- degrees

    if not driveCap then driveCap = 0.5 end
    if not arcCap then arcCap = 0.3 end
    if not turnCap then turnCap = 0.5 end

    drivePID.min, drivePID.max = -driveCap, driveCap
    anglePID.min, anglePID.max = -arcCap, arcCap
    rotatePID.min, rotatePID.max = -turnCap, turnCap

    calculateDrive()

    local driveError = math.sqrt((targetX - currX)^2 + (targetY - currY)^2)
    local targetAngle = math.atan2(currX - targetX, targetY - currY) / math.pi * 180
    if backward then
        targetAngle = targetAngle + 180
    end
    while targetAngle > 180 do
        targetAngle = targetAngle - 360
    end

    local robotLinearError = (targetY - currY) * math.cos(currGyro / 180 * math.pi) - (targetX - currX) * math.sin(currGyro / 180 * math.pi)

    local angleError = targetAngle - currGyro

    -- Run wheels, but wait until follower wheels have deployed.
    if driveTimer:Get() > 2 then
        local driveInput = 0
        local turnInput = 0

        if math.abs(angleError) < turnPrecision then
            if math.abs(robotLinearError) < drivePrecision then
                driveInput = 0
                turnInput = 0
            else
                driveInput = -drivePID:update(driveError)
                turnInput = anglePID:update(angleError)
            end
        else
            driveInput = 0
            turnInput = rotatePID:update(angleError)
        end

        if backward then
            driveInput = -driveInput
        end

        drive.update(driveInput, turnInput, false)
    else
        drive.update(0, 0, false)
    end

    -- Display auto values
    wpilib.SmartDashboard_PutNumber("X", currX)
    wpilib.SmartDashboard_PutNumber("Y", currY)
    wpilib.SmartDashboard_PutNumber("Drive Error", driveError)
    wpilib.SmartDashboard_PutBoolean("Backward", backward)
    wpilib.SmartDashboard_PutNumber("Angle Error", angleError)
    wpilib.SmartDashboard_PutNumber("Target Angle", targetAngle)
    wpilib.SmartDashboard_PutNumber("Drive PID output", drivePID.output)

    -- Store values as previous
    storeDriveCalculations()

    -- Report whether we should continue driving
    return math.abs(robotLinearError) > drivePrecision
end

function turnInPlace(targetAngle, turnPrecision, turnCap)
    if not turnPrecision then turnPrecision = 5 end -- degrees
    if not turnCap then turnCap = 0.5 end
    rotatePID.min, rotatePID.max = -turnCap, turnCap

    local angleError = targetAngle - currGyro

    calculateDrive()

    if driveTimer:Get() > 2 then
        if math.abs(angleError) < turnPrecision then
            turnInput = 0
        else
            turnInput = rotatePID:update(angleError)
        end

        drive.update(0, turnInput, false)
    else
        drive.update(0, 0, false)
    end

    storeDriveCalculations()

    return math.abs(angleError) > turnPrecision
end
