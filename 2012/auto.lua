-- auto.lua

local drive = require("drive")
local intake = require("intake")
local pid = require("pid")
local turret = require("turret")
local wpilib = require("wpilib")

module(...)

local dashboard = wpilib.SmartDashboard_GetInstance()

local autoMode
local fireCount = 0
local autodrivePIDX = pid.new(0.1)
autodrivePIDX.min, autodrivePIDX.max = -1, 1
local autodrivePIDY = pid.new(0.5, 0.0, 0.01)
autodrivePIDY.min, autodrivePIDY.max = -1, 1

local fireTimer = nil
local FIRE_COOLDOWN = 1.0

function run(extraUpdate)
    fireCount = 0
    drive.resetFollowerPosition()

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
        intake.setVerticalSpeed(0)
    end
    turret.clearFlywheelFired()
    return fireCount
end

function stopFire()
    if fireTimer and fireTimer:Get() > FIRE_COOLDOWN then
        fireTimer = nil
    end

    intake.setVerticalSpeed(0)
    turret.clearFlywheelFired()
end

function sittingKeyshot(t, Delay_1, Delay_2, Delay_3)
    local RPM = 6400
    local HOOD_TARGET = 950
    turret.setHoodTarget(HOOD_TARGET)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
    else
        turret.setFlywheelTargetSpeed(RPM)
        if fireCount < 2 then
            fireCount = fireCount + fire()
        else
            stopFire()
            turret.setFlywheelTargetSpeed(0)
        end
    end
end

function keyShotWithCoOpBridge(t, Delay_1, Delay_2, Delay_3)
    local BRIDGE_RPM = 7000
    local HOOD_TARGET = 950
    local KEY_RPM = 6400
    
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

autoMode = keyShotWithCoOpBridge
