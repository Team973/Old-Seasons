-- robot.lua

-- MUST BE FIRST:
-- Load a file named robotname.lua to obtain the global variable ROBOTNAME.
ROBOTNAME = "hodgepodge"
pcall(dofile, "lua/robotname.lua")
-- END ROBOTNAME

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local math = require("math")
local turret = require("turret")

local ROBOTNAME = ROBOTNAME

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
	    autonomous()
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

function autonomous()
    local autoTimer = wpilib.Timer()
    autoTimer:Start()

    local startDriveTime = 8
    local endDriveTime = startDriveTime + 2.5

    while wpilib.IsAutonomous() and wpilib.IsEnabled() do
        feedWatchdog()

        turret.setPreset("autoKey")
        turret.runFlywheel(true)
        if autoTimer:Get() >= 3 then
            intake.setVerticalSpeed(1)
            if ROBOTNAME == "viper" then
                intake.setIntake(.5)
            end
        else
            intake.setVerticalSpeed(0)
            intake.setIntake(0)
        end


        if ROBOTNAME == "hodgepodge" then
            intake.setLowered(autoTimer:Get() >= startDriveTime)
            if autoTimer:Get() >= startDriveTime and autoTimer:Get() <= endDriveTime then
                drive.update(0, -0.5)
            else
                drive.update(0, 0)
            end
        else
            drive.update(0, 0)
            intake.setLowered(false)
        end

        intake.update(true)
        turret.update()
        updateCompressor()

        feedWatchdog()
        wpilib.Wait(AUTO_LOOP_LAG)
        feedWatchdog()
    end
end


function teleop()
    turret.setPreset("key")

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
        ["y"] = function(axis) 
            if ROBOTNAME == "viper" then
                driveY = -axis
            else
                driveY = axis
            end
        end,
        ["rx"] = function(axis) driveX = axis end,
        [2] = function() drive.setBrakesFired(true) end,
        [4] = function() drive.setBrakesFired(false) end,
        [5] = {tick=function(held)
            if held then
                drive.setGear("low")
            else
                drive.setGear("high")
            end
        end},
        [6] = {tick=function(held) deployStinger = held end},
        ["ltrigger"] = function() intake.setLowered(true) end,
        ["rtrigger"] = function() intake.setLowered(false) end,
    },
    -- Joystick 2 (Co-Driver)
    {
        ["x"] = function(axis)
            intake.setIntake(deadband(axis, 0.2))
        end,
        ["ry"] = function(axis)
            intake.setVerticalSpeed(deadband(-axis, 0.2))
        end,
        [1] = function() turret.setPreset("rightKey") end,
        [2] = function() turret.setPreset("Feeder") end,
        [3] = function() turret.setPreset("key") end,
        [5] = {tick=intake.setRepack},
        [6] = {
            down=turret.clearFlywheelFired,
            tick=function(held)
                if held and not turret.getFlywheelFired() then
                    intake.setVerticalSpeed(0.95)
                    if ROBOTNAME == "viper" then
                        intake.setIntake(.5)
                    end
                end
            end,
        },
        [7] = function()
            turret.setPreset(nil)
            turret.setFlywheelTargetSpeed(turret.getFlywheelTargetSpeed() - 50)
        end,
        [8] = function()
            turret.setPreset(nil)
            turret.setFlywheelTargetSpeed(turret.getFlywheelTargetSpeed() + 50)
        end,
        ["ltrigger"] = {
            down=turret.openBallFlap,
            tick=function(held)
                if held then
                    intake.setIntake(1.0)
                end
            end},
        ["rtrigger"] = {
            down=turret.closeBallFlap,
            tick=turret.runFlywheel
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
dashboard:PutString("mode", "Waiting for Gyro...")
--TODO(rlight): Add this back in when we have gyro
--drive.initGyro()
-- vim: ft=lua et ts=4 sts=4 sw=4
