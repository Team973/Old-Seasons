-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer
wpilib.SmartDashboard_init()

local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local math = require("math")
local shooter = require("shooter")

local pairs = pairs
local tostring = tostring

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005
local AUTO_LOOP_LAG = 0.005 * 1.50

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local disabledIdle, autonomous, teleop, updateCompressor
local controlMap
local deployStinger
local compressor, pressureSwitch, autoDriveSwitch, stinger
local colinGyro, colinGyroTicksPerRevolution
local driveX, driveY = 0, 0
-- End Declarations

function run()
    wpilib.SmartDashboard_PutString("mode", "Ready")

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            disabledIdle()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            disableWatchdog()
            autonomous()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsAutonomous() or not wpilib.IsEnabled()
            enableWatchdog()
        elseif wpilib.IsTest() then
            disableWatchdog()
            wpilib.LiveWindow_GetInstance():SetEnabled(true)
            repeat wpilib.Wait(0.01) until not wpilib.IsTest() or not wpilib.IsEnabled()
            wpilib.LiveWindow_GetInstance():SetEnabled(false)
        else
            teleop()
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsOperatorControl() or not wpilib.IsEnabled()
            enableWatchdog()
        end
    end
end

function disabledIdle()
    while wpilib.IsDisabled() do
        feedWatchdog()

        wpilib.SmartDashboard_PutBoolean("Auto Drive", autoDriveSwitch:Get())

        feedWatchdog()
        wpilib.Wait(AUTO_LOOP_LAG)
        feedWatchdog()
    end
end

function autonomous()
    --TODO
end


function teleop()
    shooter.setPreset("key")

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        wpilib.SmartDashboard_PutString("mode", "Running")
        wpilib.SmartDashboard_PutNumber("Colin Gyro (Degrees)", colinGyro:Get() / colinGyroTicksPerRevolution * 360.0)

        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        updateCompressor()

        intake.update(true)
        shooter.update()

        -- Drive
        drive.update(driveX, driveY)

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function updateCompressor()
    wpilib.SmartDashboard_PutBoolean("pressure", pressureSwitch:Get())
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
autoDriveSwitch = wpilib.DigitalInput(10)
colinGyroTicksPerRevolution = 512
colinGyro = wpilib.Encoder(11, 12)
colinGyro:Start()
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
    -- TODO Add controls back in (some should still be there but you will have to check)
    {
        ["y"] = function(axis) 
            driveY = axis
        end,
        ["rx"] = function(axis)
            driveX = axis
        end,
        ["ltrigger"] = function() intake.setLowered(true) end,
        ["rtrigger"] = function() intake.setLowered(false) end,
    },
    -- Joystick 2 (Co-Driver)
    {
        ["x"] = function(axis)
            intake.setIntake(deadband(axis, 0.2))
        end,

        [7] = function()
            shooter.setPreset(nil)
            shooter.setFlywheelTargetSpeed(shooter.getFlywheelTargetSpeed() - 50)
        end,
        [8] = function()
            shooter.setPreset(nil)
            shooter.setFlywheelTargetSpeed(shooter.getFlywheelTargetSpeed() + 50)
        end,

        ["rtrigger"] = {
            tick=shooter.runFlywheel
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
wpilib.SmartDashboard_PutString("mode", "Waiting for Gyro...")
--TODO(rlight): Add this back in when we have gyro
--drive.initGyro()
-- vim: ft=lua et ts=4 sts=4 sw=4
