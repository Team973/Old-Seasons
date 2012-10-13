-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

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
local controlMap
local deployStinger
local compressor, pressureSwitch, stinger
local driveX, driveY = 0,0
-- End Declarations

local dashboard = wpilib.SmartDashboard_GetInstance()

function run()
    dashboard:PutString("mode", "Ready")

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            --disabledIdle()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            disableWatchdog()
	    --TODO autonomous
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
--[[
function disabledIdle()
    local gyroTimer = wpilib.Timer()

    local modes = {
        {
            name="Sitting Keyshot",
            func=auto.sittingKeyshot,
        },
    }
    local modeNum = 3

    local initAngle = drive.getGyroAngle()
    while wpilib.IsDisabled() do
        local gyroAngle = drive.normalizeAngle(-drive.getGyroAngle())
        dashboard:PutInt("Gyro Angle", gyroAngle)

        if gyroTimer and gyroTimer:Get() > 1 then
            if math.abs(drive.getGyroAngle() - initAngle) > 10 then
                drive.disableGyro()
            end
            gyroTimer:Stop()
            gyroTimer = nil
        end

        controls.update({
            -- Joystick 1
            {
                [2] = function() modeNum = modeNum + 1
                    if modeNum > #modes then
                        modeNum = 1
                    end
                    -- TODO... etc...
                end,

                [1] = function() modeNum = modeNum - 1
                    if modeNum < 1 then
                        modeNum = #modes
                    end
                end,

                [4] = function() auto.Delay_1 = auto.Delay_1 + 1
                end,

                [3] = function() auto.Delay_1 = auto.Delay_1 - 1
                end,

                [6] = function() auto.Delay_2 = auto.Delay_2 + 1
                end,

                [5] = function() auto.Delay_2 = auto.Delay_2 - 1
                end,

                [8] = function() auto.Delay_3 = auto.Delay_3 + 1
                end,

                [7] = function() auto.Delay_3 = auto.Delay_3 - 1
                end,

            },
            -- Joystick 2
            {},
            -- Joystick 3
            {},
            -- Joystick 4
            {},
        })

        dashboard:PutString("Auto Mode", modes[modeNum].name)
        dashboard:PutInt("Delay_1", auto.Delay_1)
        dashboard:PutInt("Delay_2", auto.Delay_2)
        dashboard:PutInt("Delay_3", auto.Delay_3)
        auto.autoMode = modes[modeNum].func


        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end
--]]
function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        dashboard:PutString("mode", "Running")

        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        stinger:Set(deployStinger)

        -- Pneumatics
        updateCompressor()

        intake.update(true)
        turret.update()

        -- Drive
        drive.update(driveX, driveY)

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
pressureSwitch = wpilib.DigitalInput(2)
stinger = wpilib.Solenoid(3)
-- End Inputs/Outputs

-- Controls
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

local prevOperatorDpad = 0.0

controlMap =
{
    -- Joystick 1 (Driver)
    {
        ["y"] = function(axis) driveY = axis end,
	["rx"] = function(axis) driveX = axis end,
        [2] = drive.resetGyro,
        [4] = drive.effTheGyro,
        [5] = {tick=function(held)
            if held then
                drive.setGear("low")
            else
                drive.setGear("high")
            end
        end},
    },
    -- Joystick 2 (Co-Driver)
    {
        ["rx"] = function(axis)
            intake.setIntake(deadband(axis, 0.2))
        end,
        ["ry"] = function(axis)
            intake.setVerticalSpeed(deadband(-axis, 0.2))
        end,
        [1] = function() turret.setPreset("rightKey") end,
        [2] = function() turret.setPreset("Feeder") end,
        [3] = function() turret.setPreset("key") end,
        [5] = {tick=intake.setLowered},
	[6] = {tick=turret.runFlywheel},
        [7] = function()
            turret.setPreset(nil)
            turret.setFlywheelTargetSpeed(turret.getFlywheelTargetSpeed() - 50)
        end,
        [8] = function()
            turret.setPreset(nil)
            turret.setFlywheelTargetSpeed(turret.getFlywheelTargetSpeed() + 50)
        end,
        [9] = {tick=function(held) deployStinger = held end},
        [10] = {tick=intake.setRepack},
        ["ltrigger"] = {tick=function(held)
            if held then
                intake.setIntake(1.0)
            end
        end},
        --["rtrigger"] = {tick=turret.runFlywheel},
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
--TODO(rlight): Add this back in when we have gyro
--drive.initGyro()
-- vim: ft=lua et ts=4 sts=4 sw=4
