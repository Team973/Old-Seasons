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

local resetDrive, driveToPoint, calculateDrive, storeDriveCalculations, runConveyer

function run()
    resetDrive()
    local FIRST_TURN = .1
    local INTAKE_TIME = 4
    local MOVEMENT_6 = 4
--[[
    while turnInPlace(90) do
        coroutine.yield()
    end
    
    --[[
    while linearDrive(96, 0) do
        coroutine.yield()
    end

    --[[
    shooter.fullStop()
    arm.setPreset("autoShot")
    ]]

    --[[
    arm.setPreset("autoShot")

    shooter.setFlywheelRunning(true)

    driveTimer = wpilib.Timer()
    driveTimer:Start()

    while turnInPlace(12, 2) do
        coroutine.yield()
    end

    shooter.fire()
    while shooter.getDiscsFired() < 3 do
        coroutine.yield()
    end
    shooter.fire(false)

    shooter.setFlywheelRunning(false)
    shooter.clearDiscsFired()


    while driveToPoint(0, -110, true, 12, 5, .8) do
        intake.goToDeploy(true)
        coroutine.yield()
    end

    while turnInPlace(-70) do
        intake.setPreset("Intake")
        coroutine.yield()
    end

    while turnInPlace(-90) do
        coroutine.yield()
    end

    arm.setPreset("Intake")

    while driveToPoint(-96, -110, true, 36, 5, .7) or driveTimer:Get() < driveTimer:Get() + INTAKE_TIME do
        intake.setIntakeSpeed(1)
        shooter.setConveyerManual(1)
        coroutine.yield()
    end


    while driveToPoint(0, -30, false, 12, 5, .8) or driveTimer:Get() < driveTimer:Get() + MOVEMENT_6 do
        coroutine.yield()
    end

    shooter.setConveyerManual(0)
    shooter.setRollerManual(0)
    intake.setIntakeSpeed(0)

    arm.setPreset("autoShot")

    while driveToPoint(12, 60, false, 66, 5, .9) do
        coroutine.yield()
    end

    shooter.setFlywheelRunning(true)

    while turnInPlace(20) do
        coroutine.yield()
    end

    shooter.fire()

    while shooter.getDiscsFired() <= 3 do
        coroutine.yield()
    end
    ]]

    local shootTimer = wpilib.Timer()
    shootTimer:Start()

    arm.setPreset("autoShot")

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



    -- Clean up
    shooter.fullStop()
    intake.setDeploy(false)
    drive.update(0, 0, false)
    intake.setIntakeSpeed(0.0)
end

local drivePID = pid.new(.03, .001)
drivePID.icap = .2
local anglePID = pid.new(.1)
local rotatePID = pid.new(.15, .01, 0.005)
rotatePID.icap = .1

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

end


function calculateDrive()
    currGyro = drive.getGyroAngle()
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

function linearDrive(targetDrive, targetAngle, drivePercision, arcPercision)
    if not drivePrecision then drivePrecision = 0 end -- inches
    local driveError = targetDrive - drive.getWheelDistance()
    local angleError = targetAngle - currGyro
    local driveInput = 0
    drivePID.min, drivePID.max = -.7, .7
    --anglePID.min, anglePID.max = -arcCap, arcCap
    drivePID.target = targetDrive
    anglePID.target = targetAngle

        if math.abs(driveError) < drivePrecision then
            driveInput = 0
            arcInput = 0
        else
            driveInput = drivePID:update(drive.getWheelDistance())
            arcInput = anglePID:update(currGyro)
        end

    drive.update(driveInput, arcInput, false)

    return math.abs(driveError) > drivePrecision
end

function driveToPoint(targetX, targetY, backward, drivePrecision, turnPrecision, driveCap, turnCap, arcCap)
    if not drivePrecision then drivePrecision = 6 end -- inches
    if not turnPrecision then turnPrecision = 5 end -- degrees

    if not driveCap then driveCap = 0.5 end
    if not arcCap then arcCap = 0.3 end
    if not turnCap then turnCap = 0.7 end

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

    -- Display auto values
    wpilib.SmartDashboard_PutNumber("X", currX)
    wpilib.SmartDashboard_PutNumber("Y", currY)
    wpilib.SmartDashboard_PutNumber("Drive Error", driveError)
    wpilib.SmartDashboard_PutBoolean("Backward", backward)
    wpilib.SmartDashboard_PutNumber("Angle Error", angleError)
    wpilib.SmartDashboard_PutNumber("Target Angle", targetAngle)
    wpilib.SmartDashboard_PutNumber("Drive PID output", drivePID.output)
    wpilib.SmartDashboard_PutNumber("Current Gyro", currGyro)

    -- Store values as previous
    storeDriveCalculations()

    -- Report whether we should continue driving
    if math.abs(angleError) > turnPrecision then
        return math.abs(robotLinearError) > drivePrecision
    else
        return true
    end
end

function turnInPlace(targetAngle, turnPrecision, turnCap)
    if not turnPrecision then turnPrecision = 5 end -- degrees
    if not turnCap then turnCap = 0.7 end
    rotatePID.min, rotatePID.max = -turnCap, turnCap

    local angleError = targetAngle - currGyro

    calculateDrive()

        if math.abs(angleError) < turnPrecision then
            turnInput = 0
        else
            turnInput = rotatePID:update(angleError)
        end

        drive.update(0, turnInput, false)

    storeDriveCalculations()

    return math.abs(angleError) > turnPrecision
end
