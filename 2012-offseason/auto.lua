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
    drive.resetFollowerPosition()
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
    drive.undeployFollower()
    drive.setFrontSkid(true)
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
5. Skid
--]]

function sittingKeyshot(t)
    turret.setTurretEnabled(false)
    turret.setPreset("key")
    if t < Delay_1 - 2 then
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

function sittingKeyshotWithPass(t)
    turret.setTurretEnabled(false)
    turret.setPreset("key")
    if t < Delay_1 - 2 then
        turret.runFlywheel(false)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.runFlywheel(true)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.runFlywheel(true)
        stopFire()
    else
        turret.runFlywheel(true)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
    end
end

function sittingKeyshotWithDropPass(t)
    turret.setTurretEnabled(false)
    turret.setPreset("key")
    if t < Delay_1 - 2 then
        turret.runFlywheel(false)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.runFlywheel(true)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.runFlywheel(true)
        stopFire()
        intake.setLowered(true)
    else
        turret.runFlywheel(true)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
        intake.setLowered(true)
    end
end

function sittingKeyshotWithSlapPass(t)
    local SLAP_INTERVAL = 20
    turret.setTurretEnabled(false)
    turret.setPreset("key")
    if t < Delay_1 - 2 then
        turret.runFlywheel(false)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.runFlywheel(true)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    else
        intake.setIntake(1.0)
        turret.runFlywheel(true)
        if t < Delay_3 then
            stopFire()
        else
            fireCount = fireCount + fire()
        end
        intake.setLowered(math.floor((t-Delay_2)/SLAP_INTERVAL)%2 == 0)
    end
end

function keyShotWithCoOpBridge(t)
    local posx, posy = drive.getFollowerPosition()
    turret.setTurretEnabled(false)
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)
    drive.deployFollower()
    turret.setPreset("key")
    if t < Delay_1 - 2 then
        drive.setFrontSkid(false)
        turret.runFlywheel(true)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        drive.setFrontSkid(false)
        turret.runFlywheel(true)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        drive.setFrontSkid(true)
        turret.runFlywheel(true)
        if fireCount < 2 then
            fireCount = fireCount + fire()
            drive.run({x=0, y=0}, 0, 1)
        else
            stopFire()
            autodrivePIDX.target = 0.0
            autodrivePIDY.target = -4.0
            setDriveAxis("y")
            drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        end
        intake.setIntake(1.0)
    else
        drive.setFrontSkid(true)
        turret.runFlywheel(true)
        if not drive.isFollowerStopped() then
            drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        else
            drive.run({x=0, y=0}, 0, 1)
        end
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
    end
end

local DISTANCE_TO_HOOP = 15

function keyShotWithCoOpBridgeFar(t)
    local CLOSE_BRIDGE_DISTANCE = -13
    local RPM_FUDGE = 100

    local posx, posy = drive.getFollowerPosition()

    turret.setTurretEnabled(true)
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)
    drive.deployFollower()
    turret.setPreset("key")

    if t < Delay_1 - 2 then
        drive.run({x=0, y=0}, 0, 1)
        turret.runFlywheel(false)
        intake.setIntake(0.0)
        stopFire()
        drive.setFrontSkid(false)
    elseif t < Delay_1 then
        drive.run({x=0, y=0}, 0, 1)
        turret.runFlywheel(true)
        intake.setIntake(0.0)
        stopFire()
        drive.setFrontSkid(false)
    elseif fireCount < 2 then
        -- After Delay_1, Fire two balls
        drive.run({x=0, y=0}, 0, 1)
        turret.runFlywheel(true)
        intake.setIntake(0.0)
        fireCount = fireCount + fire()
        drive.setFrontSkid(false)
    elseif not driveStopped then
        -- After firing two balls
        autodrivePIDX.target = 0.0
        autodrivePIDY.target = CLOSE_BRIDGE_DISTANCE
        autodrivePIDY.min, autodrivePIDY.max = -0.9, 0.9
        setDriveAxis("y")
        drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        stopFire()

        intake.setIntake(1)
        drive.setFrontSkid(true)
        turret.setTargetAngle(calculateTurretTarget(posx, DISTANCE_TO_HOOP - posy, drive.normalizeAngle(-drive.getGyroAngle())))
        if posy/CLOSE_BRIDGE_DISTANCE > .9 and drive.isFollowerStopped() then
            driveStopped = true
            bridgeTimer = wpilib.Timer()
            bridgeTimer:Start()
        end
    else
        if bridgeTimer:Get() > 2 then
            intake.setLowered(true)
            autodrivePIDX.target = 0.0
            autodrivePIDY.target = 0.0
            autodrivePIDY.min, autodrivePIDY.max = -1.0, 1.0
            drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        else
            intake.setLowered(false)
            drive.run({x=0, y=0}, 0, 1)
        end

        if t > Delay_3 then
            -- Fire continuous
            fireCount = fireCount + fire()
            turret.setTargetAngle(calculateTurretTarget(posx, DISTANCE_TO_HOOP - posy, drive.normalizeAngle(-drive.getGyroAngle())))
        else
            stopFire()
        end
    end
end

function lshape(t)
    local posx, posy = drive.getFollowerPosition()
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)

    intake.setIntake(0.0)
    turret.runFlywheel(false)
    stopFire()

    if t < Delay_1 then
        autodrivePIDX.target = 0.0
        autodrivePIDY.target = -4.0
        setDriveAxis("y")
        drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
    else
        autodrivePIDX.target = 4.0
        autodrivePIDY.target = -4.0
        setDriveAxis("x")
        drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
    end
end

function sittingKeyshotSafety(t)
    turret.setPreset("key")
    turret.setTurretEnabled(false)
    if t < Delay_1 - 2 then
        turret.runFlywheel(false)
        intake.setVerticalSpeed(0)
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        intake.setVerticalSpeed(0)
    else
        intake.setVerticalSpeed(1)
        turret.runFlywheel(true)
    end
end

function sittingKeyshotWithPassSafety(t)
    turret.setPreset("key")
    turret.setTurretEnabled(false)
    if t < Delay_1 - 2 then
        turret.runFlywheel(false)
        intake.setVerticalSpeed(0)
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.runFlywheel(true)
        intake.setVerticalSpeed(0)
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.runFlywheel(true)
        intake.setVerticalSpeed(1)
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.runFlywheel(true)
        intake.setVerticalSpeed(0)
    else
        turret.runFlywheel(true)
        intake.setVerticalSpeed(1)
        intake.setIntake(1.0)
    end
end

autoMode = sittingKeyshot
