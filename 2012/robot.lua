-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

local auto = require("auto")
local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local math = require("math")
local turret = require("turret")

local pairs = pairs
local tostring = tostring

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local disabledIdle, hellautonomous, teleop, updateCompressor
local controlMap, fudgeControlMap, strafe, rotation
local deploySkid, deployStinger
local zeroMode, possessionTimer
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, stinger
local frontSkid
local squishMeter
local driveMode = 0

-- End Declarations

local dashboard = wpilib.SmartDashboard_GetInstance()

function run()
    turret.initI2C()
    dashboard:PutString("mode", "Ready")

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            disabledIdle()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            disableWatchdog()
            auto.run(updateCompressor)
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsAutonomous() or not wpilib.IsEnabled()
            enableWatchdog()
        else
            teleop()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsOperatorControl() or not wpilib.IsEnabled()
            enableWatchdog()
        end
    end
end

function disabledIdle()
    local gyroTimer = wpilib.Timer()

    local initAngle = drive.getGyroAngle()
    while wpilib.IsDisabled() do
        if gyroTimer and gyroTimer:Get() > 1 then
            if math.abs(drive.getGyroAngle() - initAngle) > 10 then
                drive.disableGyro()
            end
            gyroTimer:Stop()
            gyroTimer = nil
        end

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

function teleop()
    turret.turnPID:start()

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        if not fudgeMode then
            dashboard:PutString("mode", "Running")
        else
            dashboard:PutString("mode", "Fudge Mode")
        end

        -- Read controls
        if fudgeMode then
            controls.update(fudgeControlMap)
        else
            controls.update(controlMap)
        end
        feedWatchdog()

        stinger:Set(deployStinger)

        -- Pneumatics
        updateCompressor()

        frontSkid:Set(deploySkid)

        intake.update(true)
        turret.update()

        dashboard:PutDouble("Flywheel P", turret.flywheelPID.p)
        dashboard:PutDouble("Flywheel D", turret.flywheelPID.d)
        dashboard:PutDouble("Flywheel Speed", turret.getFlywheelSpeed())
        dashboard:PutInt("Flywheel Speed(Int)", turret.getFlywheelSpeed())
        dashboard:PutInt("Flywheel Speed(Filter Int)", turret.getFlywheelFilterSpeed())
        dashboard:PutDouble("Flywheel Target Speed", turret.getFlywheelTargetSpeed())
        dashboard:PutDouble("Squish Meter", squishMeter:GetVoltage())

        local followerX, followerY = drive.getFollowerPosition()
        dashboard:PutDouble("Follower X", followerX)
        dashboard:PutDouble("Follower Y", followerY)

        -- Drive
        if zeroMode then
            for _, wheel in pairs(drive.wheels) do
                local currentTurn = wheel.turnEncoder:GetDistance()
                wheel.turnPID.errFunc = drive.normalizeAngle
                wheel.turnPID.target = 0
                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
                wheel.driveMotor:Set(0)
            end
        elseif fudgeMode then
            for _, wheel in pairs(drive.wheels) do
                wheel.driveMotor:Set(0)
                wheel.turnMotor:Set(0)
            end

            if fudgeWheel then
                fudgeWheel.turnMotor:Set(fudgeMovement)
            end
        else
            drive.run(strafe, rotation, driveMode)
        end

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function updateCompressor()
    dashboard:PutBoolean("pressure", pressureSwitch:Get())
    if pressureSwitch:Get() then
        compressor:Set(wpilib.Relay_kOff)
    else
        compressor:Set(wpilib.Relay_kOn)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
compressor = wpilib.Relay(1, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1, 14)
frontSkid = wpilib.Solenoid(3)
stinger = wpilib.Solenoid(7)
squishMeter = wpilib.AnalogChannel(5)
-- End Inputs/Outputs

-- Controls
strafe = {x=0, y=0}
rotation = 0
turretDirection = {x=0, y=0} 

zeroMode = false
fudgeMode = false
fudgeWheel = nil
fudgeMovement = 0.0

local function fudgeButton(wheel)
    return {
        down=function()
            fudgeWheel = wheel
        end,
        up=function()
            if fudgeWheel == wheel then
                fudgeWheel = nil
            end
        end,
    }
end

local function incConstant(tbl, name, pid, delta)
    return function()
        local target = pid.target
        pid:start()
        tbl[name] = tbl[name] + delta
        pid:stop()
        pid:reset()
        pid.target = target
    end
end

local function deadband(axis, threshold)
    if axis < threshold and axis > -threshold then
        return 0
    else
        return axis
    end
end

local rpmPreset = 3300.0
local prevOperatorDpad = 0.0
local function presetValues (flywheelRPM, hoodAngle, turretAngle)
    rpmPreset = flywheelRPM
    turret.setHoodTarget(hoodAngle)
    turret.setTargetAngle(turretAngle)
end

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = deadband(axis, 0.15) end,
        ["y"] = function(axis) strafe.y = deadband(-axis, 0.15) end,
        ["rx"] = function(axis) rotation = deadband(axis, 0.15) end,
        [1] = {tick=function(held) deployStinger = held end},
        [2] = drive.resetGyro,
        [5] = {tick=function(held)
            if held then
                drive.setGear("low")
            else
                drive.setGear("high")
            end
        end},
        [7] = function() fudgeMode = true end,
        [8] = function() drive.calibrateAll() end,
        [10] = {tick=function(held)
            if held then
                if drive.getGear() == "low" then
                    rotation = rotation * 0.5
                elseif drive.getGear() == "high" then
                    rotation = rotation * 0.5
                end
            end
        end},
                                
        ["update"] = function(stick)
            if controls.isLeftTriggerHeld(stick) then
                strafe.x = 0
                driveMode = 1
            elseif controls.isRightTriggerHeld(stick) then
                strafe.x = strafe.y
                strafe.y = 0
                driveMode = 2
            else
                driveMode = 0
            end
        end
    },
    -- Joystick 2
    {
        ["x"] = function(axis)
            turretDirection.x = deadband(axis, 0.2)
            turret.setFromJoy(turretDirection.x, turretDirection.y) 
        end,
        
        ["y"] = function(axis) 
            turretDirection.y = deadband(-axis, 0.2) 
            turret.setFromJoy(turretDirection.x, turretDirection.y)
        end,
        ["rx"] = function(axis)
            intake.setIntake(deadband(axis, 0.2))
        end,
        ["ry"] = function(axis)
            intake.setVerticalSpeed(deadband(-axis, 0.2))
        end,
        ["hatx"] = function(axis)
            local increment = 1
            if axis > 0.5 and prevOperatorDpad <= 0.5 then
                -- Dpad right
                turret.setTargetAngle(turret.getTargetAngle() + 1)
            end
            if axis < -0.5 and prevOperatorDpad >= -0.5 then
                -- Dpad left
                turret.setTargetAngle(turret.getTargetAngle() - 1)
            end
            prevOperatorDpad = axis
        end,
        [1] = function() presetValues(3300,20,0) end, -- Fender
        [2] = function() presetValues(4500,200,0) end, -- Side
        [3] = function() presetValues(6400,950,0) end, -- Key
        [4] = {tick=function(held) deploySkid = held end},
        [5] = {tick=function(held) intake.setLowered(held) end},   
        [6] = {
            down=turret.clearFlywheelFired,
            tick=function(held)
                if held and not turret.getFlywheelFired() then
                    intake.setVerticalSpeed(1.0)
                end
            end,
        },
        [7] = function()
            rpmPreset = rpmPreset - 100
        end,
        [8] = function()
            rpmPreset = rpmPreset + 100
        end,
        [9] = {tick=function(held) turret.allowRotate = held end},   
        [10] = {tick=intake.setRepack},
        ["ltrigger"] = {tick=function(held)
            if held then
                intake.setIntake(1.0)
            end
        end},
        ["rtrigger"] = {
            down=function()
                turret.resetFlywheel()
            end,
            tick=function(held)
                if held then
                    turret.setFlywheelTargetSpeed(rpmPreset)
                else
                    turret.setFlywheelTargetSpeed(0.0)
                end
            end,
        },
    },
    -- Joystick 3
    {
    },
    -- Joystick 4 (eStop Module)
    {
    },
    -- Cypress Module
    cypress={},
}

fudgeControlMap = {
    -- Joystick 1
    {
        ["x"] = function(axis)
            fudgeMovement = deadband(axis, 0.15)
        end,
        [1] = fudgeButton(drive.wheels.rightBack),
        [2] = fudgeButton(drive.wheels.rightFront),
        [3] = fudgeButton(drive.wheels.leftBack),
        [4] = fudgeButton(drive.wheels.leftFront),
        [7] = function()
            fudgeMode = false
            for _, wheel in pairs(drive.wheels) do
                wheel.turnEncoder:Reset()
            end
        end,
    },
    -- Joystick 2
    {
    },
    -- Joystick 3
    {
    },
    -- Joystick 4 (eStop Module)
    {
    },
    -- Cypress Module
    cypress={},
}
-- End Controls

-- Watchdog shortcuts
if watchdogEnabled then
    wpilib.GetWatchdog():SetExpiration(0.25)

    function feedWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:Feed()
    end

    function enableWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:SetEnabled(true)
    end

    function disableWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:SetEnabled(false)
    end
else
    local dog = wpilib.GetWatchdog()
    dog:SetEnabled(false)

    feedWatchdog = function() end
    enableWatchdog = function() end
    disableWatchdog = function() end
end

-- Only create the gyro at the end, because it blocks the entire thread.
dashboard:PutString("mode", "Waiting for Gyro...")
drive.initGyro()

-- vim: ft=lua et ts=4 sts=4 sw=4
