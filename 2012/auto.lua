-- auto.lua

local drive = require("drive")
local intake = require("intake")
local pid = require("pid")
local turret = require("turret")
local wpilib = require("wpilib")
local KEY_RPM = 6300
local KEY_HOOD_TARGET = 1100


local math = require("math")
module(...)

local dashboard = wpilib.SmartDashboard_GetInstance()

local autoMode
local fireCount = 0

local driveGains = {p=0.5, i=0, d=0.01}
local stableGains = {p=0.1, i=0, d=0}
local autodrivePIDX = pid.new()
autodrivePIDX.min, autodrivePIDX.max = -1, 1
local autodrivePIDY = pid.new()
autodrivePIDY.min, autodrivePIDY.max = -1, 1

Delay_1 = 0
Delay_2 = 0
Delay_3 = 0

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
local FIRE_COOLDOWN = 1.0
local REPACK_COOLDOWN = 0.5

function run(extraUpdate)
    fireCount = 0
    drive.resetFollowerPosition()
    drive.resetGyro()

    local t = wpilib.Timer()
    t:Start()

    while wpilib.IsAutonomous() and wpilib.IsEnabled() do
        autoMode(t:Get(),3,10,0) 

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
    end

    if fireTimer == nil then
        -- Ready to fire
        intake.setVerticalSpeed(1)
        if turret.getFlywheelFired() then 
            fireTimer = wpilib.Timer() 
            fireTimer:Start()
            fireCount = 1
        end
    else
        -- Cooldown
        if fireTimer:Get() < REPACK_COOLDOWN then
            intake.setVerticalSpeed(-0.2)
        else
            intake.setVerticalSpeed(0.0)
        end
    end
    turret.clearFlywheelFired()
    return fireCount
end

function stopFire()
    if fireTimer and fireTimer:Get() > FIRE_COOLDOWN then
        fireTimer = nil
    end

    if fireTimer and fireTimer:Get() < REPACK_COOLDOWN then
        intake.setVerticalSpeed(-0.2)
    else
        intake.setVerticalSpeed(0)
    end
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
    turret.setHoodTarget(KEY_HOOD_TARGET)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(KEY_RPM)
        stopFire()
    else
        turret.setFlywheelTargetSpeed(KEY_RPM)
        if fireCount < 2 then
            fireCount = fireCount + fire()
        else
            stopFire()
            turret.setFlywheelTargetSpeed(0)
        end
    end
end

function keyShotWithCoOpBridge(t)
    local BRIDGE_RPM = 7000
    
    local posx, posy = drive.getFollowerPosition()
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(KEY_RPM)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        --TODO: Lower mantis
        turret.setFlywheelTargetSpeed(KEY_RPM)
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
        turret.setFlywheelTargetSpeed(BRIDGE_RPM)
        if not drive.isFollowerStopped() then
            drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        else
            drive.run({x=0, y=0}, 0, 1)
        end
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
    end
end

function foo(t)
    local BRIDGE_RPM = 7000
    
    local posx, posy = drive.getFollowerPosition()
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(KEY_RPM)
        stopFire()
        drive.run({x=0, y=0}, 0, 1)
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        --TODO: Lower mantis
        turret.setFlywheelTargetSpeed(KEY_RPM)
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
        turret.setFlywheelTargetSpeed(BRIDGE_RPM)
        autodrivePIDX.target = 0.0
        autodrivePIDY.target = 0.0
        drive.run({x=autodrivePIDX.output, y=autodrivePIDY.output}, 0, 1)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
        turret.setTargetAngle(calculateTurretTarget(posx, 12 - posy, drive.normalizeAngle(-drive.getGyroAngle())))
    end
end

function lshape(t)
    local posx, posy = drive.getFollowerPosition()
    autodrivePIDX:update(posx)
    autodrivePIDY:update(posy)
    dashboard:PutDouble("Follower X", posx)
    dashboard:PutDouble("Follower Y", posy)

    intake.setIntake(0.0)
    turret.setFlywheelTargetSpeed(0)
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

autoMode = sittingKeyshot
