-- auto.lua

local drive = require("drive")
local intake = require("intake")
local math = require("math")
local pid = require("pid")
local turret = require("turret")
local wpilib = require("wpilib")

module(...)

local dashboard = wpilib.SmartDashboard_GetInstance()

autoMode = nil
local fireCount = 0
local startedFiring = false
local driveStopped = false
local bridgeTimer = nil

local driveGains = {p=0.5, i=0, d=0.00}
local stableGains = {p=0.1, i=0, d=0}
local autodrivePIDX = pid.new()
autodrivePIDX.min, autodrivePIDX.max = -1, 1
local autodrivePIDY = pid.new()
autodrivePIDY.min, autodrivePIDY.max = -1, 1

Delay_1 = 3
Delay_2 = 4
Delay_3 = 4

local function setDriveAxis(axis)
    if axis == "x" then
        autodrivePIDX.p = driveGains.p
        autodrivePIDX.i = driveGains.i
        autodrivePIDX.d = driveGains.d
        autodrivePIDY.p = stableGains.p
        autodrivePIDY.i = stableGains.i
        autodrivePIDY.d = stableGains.d
    else
        autodrivePIDX.p = stableGains.p
        autodrivePIDX.i = stableGains.i
        autodrivePIDX.d = stableGains.d
        autodrivePIDY.p = driveGains.p
        autodrivePIDY.i = driveGains.i
        autodrivePIDY.d = driveGains.d
    end
end
setDriveAxis("y")

local fireTimer = nil
local FIRE_COOLDOWN = 0.5
local REPACK_COOLDOWN = 0.25

function run(extraUpdate)
    fireCount = 0
    startedFiring = false
    drive.resetGyro()
    driveStopped = false
    bridgeTimer = nil

    local t = wpilib.Timer()
    t:Start()

    while wpilib.IsAutonomous() and wpilib.IsEnabled() do
        autoMode(t:Get())

        intake.update(true)
        turret.update()
        if extraUpdate then
            extraUpdate()
        end

        wpilib.Wait(0.005)
    end

    turret.fullStop()
    intake.fullStop()
end

function calculateTurretTarget(x,y,gyro)
    return math.deg(-math.atan2(x,y)) - gyro
end

function fire()
    local fireCount = 0
    if fireTimer and fireTimer:Get() > FIRE_COOLDOWN then
        fireTimer = nil
        startedFiring = false
    end

    if fireTimer == nil then
        -- Ready to fire
        if intake.getLastBallSoftness() and startedFiring then
            fireTimer = wpilib.Timer()
            fireTimer:Start()
            fireCount = 1
        else
            intake.loadBall()
            startedFiring = true
        end
    else
        -- Cooldown
        intake.setVerticalSpeed(0.0)
    end
    turret.clearFlywheelFired()
    return fireCount
end

function stopFire()
    if fireTimer and fireTimer:Get() > FIRE_COOLDOWN then
        fireTimer = nil
        startedFiring = false
    end

    intake.setVerticalSpeed(0)
    turret.clearFlywheelFired()
end

-- For each autonomous mode, make sure you set:
--[[
1. Drive
2. Flywheel Speed
3. Intake speed
4. Fire/stopFire
--]]

function sittingKeyshot(t)
    turret.setTurretEnabled(false)
    turret.setPreset("key")
    if t < Delay_1 - 10 then
        turret.runFlywheel(false)
        stopFire()
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        stopFire()
    else
        fireCount = fireCount + fire()
        turret.runFlywheel(true)
    end
end

autoMode = sittingKeyshot
