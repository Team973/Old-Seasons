-- auto.lua

local drive = require("drive")
local intake = require("intake")
local pid = require("pid")
local turret = require("turret")
local wpilib = require("wpilib")

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

-- For each autonomous mode, make sure you set:
--[[
1. Drive
2. Flywheel Speed
3. Intake speed
4. Fire/stopFire
--]]

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

function sittingKeyshotWithPass(t, Delay_1, Delay_2, Delay_3)
    local RPM = 6400
    local HOOD_TARGET = 950
    turret.setHoodTarget(HOOD_TARGET)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
    else
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
    end
end

function sittingKeyshotWithDropPass(t, Delay_1, Delay_2, Delay_3)
    local RPM = 6400
    local HOOD_TARGET = 950
    turret.setHoodTarget(HOOD_TARGET)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
        intake.setLowered(true)
    else
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
    end
end
function sittingKeyshotWithDropPass(t, Delay_1, Delay_2, Delay_3)
    local RPM = 640
    local SLAP_INTERVAL = 1.5
    local HOOD_TARGET = 950
    turret.setHoodTarget(HOOD_TARGET)
    if t < Delay_1 - 2 then
        turret.setFlywheelTargetSpeed(0)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_1 then
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
        intake.setIntake(0.0)
    elseif t < Delay_2 then
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(0.0)
    elseif t < Delay_3 then
        intake.setIntake(1.0)
        turret.setFlywheelTargetSpeed(RPM)
        stopFire()
        intake.setLowered(math.floor((t-Delay_2)/SLAP_INTERVAL)%2 == 0)
    else
        turret.setFlywheelTargetSpeed(RPM)
        fireCount = fireCount + fire()
        intake.setIntake(1.0)
        intake.setLowered(math.floor((t-Delay_2)/SLAP_INTERVAL)%2 == 0)
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

local driveStopped = false

function keyShotWithCoOpBridgeFar(t, Delay_1, Delay_2, Delay_3)
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
    else
        if fireCount < 2 then
            fireCount = fireCount + fire()
            intake.setIntake(1.0)
            intake.setLowered(true)
            autodrivePIDX.target = 0.0
            autodrivePIDY.target = -8.0
            turret.setTargetAngle(calculateTurretTarget(posx, 12 - posy, drive.normalizeAngle(-drive.getGyroAngle())))

        elseif not driveStopped then
            if drive.isFollowerStopped() then
                driveStopped = true
            else
            end
        elseif t < Delay_2 then
            stopFire()
        else
            fireCount = fireCount + fire()
        end
    end
end

function foo(t, Delay_1, Delay_2, Delay_3)
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

function lshape(t, Delay_1, Delay_2, Delay_3)
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

autoMode = foo
