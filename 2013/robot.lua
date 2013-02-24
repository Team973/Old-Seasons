-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer
wpilib.SmartDashboard_init()

local arm = require("arm")
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
local compressor, pressureSwitch, pressureTransducer, autoDriveSwitch, stinger
local driveX, driveY, quickTurn = 0, 0, false

-- STATES
local state = nil
local FIRE = "fire"
local HUMAN_LOAD = "human_load"
local STOW = "stow"
local FEED = "feed"

-- End Declarations

function run()
    local lw = wpilib.LiveWindow_GetInstance()
    lw:SetEnabled(false)

    local ds = wpilib.DriverStation_GetInstance()

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            ds:InDisabled(true)
            disabledIdle()
            ds:InDisabled(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            disableWatchdog()
            ds:InAutonomous(true)
            autonomous()
            ds:InAutonomous(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsAutonomous() or not wpilib.IsEnabled()
            enableWatchdog()
        elseif wpilib.IsTest() then
            disableWatchdog()
            lw:SetEnabled(true)
            --ds:InTest(true)
            repeat ds:WaitForData() until not wpilib.IsTest() or not wpilib.IsEnabled()
            --ds:InTest(false)
            lw:SetEnabled(false)
        else
            ds:InOperatorControl(true)
            teleop()
            ds:InOperatorControl(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsOperatorControl() or not wpilib.IsEnabled()
            enableWatchdog()
        end
    end
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutBoolean("pressure", pressureSwitch:Get())
    wpilib.SmartDashboard_PutNumber("Pressure Transducer", pressureTransducer:GetVoltage())
end

function disabledIdle()
    while wpilib.IsDisabled() do
        feedWatchdog()

        --Load Dashboard outputs
        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        feedWatchdog()
        wpilib.Wait(AUTO_LOOP_LAG)
        feedWatchdog()
    end


end

function autonomous()
    --TODO
end


function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()


        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        updateCompressor()

        intake.update(true)
        shooter.update()

        -- Drive
        drive.update(driveX, driveY, quickTurn)

        -- Arm
        arm.update()

        -- Dashboard
        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function updateCompressor()
    if pressureSwitch:Get() then
        compressor:Set(wpilib.Relay_kOff)
    else
        compressor:Set(wpilib.Relay_kOn)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
compressor = wpilib.Relay(1, 8, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(14)
pressureTransducer = wpilib.AnalogChannel(4)
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

local prevCoDriverDpad = 0.0

controlMap =
{
    -- Joystick 1 (Driver)
    {
        ["y"] = function(axis) 
            driveY = -deadband(axis, 0.1)
        end,
        ["rx"] = function(axis)
            driveX = deadband(axis, 0.1)
        end,
        [5] = {tick=function(held)
            quickTurn = held
        end},
    },
    -- Joystick 2 (Co-Driver)
    {
        ["y"] = function(axis)
            shooter.setConveyerManual(-deadband(axis, 0.1))
        end,
        ["ry"] = function(axis)
            shooter.setRollerManual(-deadband(axis, 0.1))
        end,

        [4] = function()
            arm.setPreset("Shooting")
            shooter.setFlapActive(false)
            shooter.setHardStopActive(false)
            state = FIRE
        end,

        [2] = function()
            arm.setPreset("Stow")
            shooter.setFlapActive(false)
            shooter.fire(false)
            state = STOW
        end,

        [7] = function()
            arm.setPreset("Loading")
            shooter.fire(false)
            shooter.setFlapActive(true)
            shooter.setHardStopActive(true)
            state = HUMAN_LOAD
        end,

        [5] = {tick=shooter.humanLoad},

        [6] = {tick=shooter.feed},

        [8] = function()
            if state == FIRE then
                shooter.fire(true)
            else
                shooter.fire(false)
            end
        end,

        [9] = function()
            if state == FIRE then
                shooter.fire(false)
            end
        end,

        [10] = function()
            if state == HUMAN_LOAD then
                shooter.setFlapActive(false)
            end
        end,

        ["haty"] = function(axis)
            local increment = 1
            if axis > 0.5 and prevCoDriverDpad <= 0.5 then
                -- Dpad down
                arm.setArmTarget(arm.getArmTarget() - 0.5)
            end
            if axis < -0.5 and prevCoDriverDpad >= -0.5 then
                -- Dpad down
                arm.setArmTarget(arm.getArmTarget() + 0.5)
            end
            prevCoDriverDpad = axis
        end,
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
--TODO(rlight): Add this back in when we have gyro
--drive.initGyro()
-- vim: ft=lua et ts=4 sts=4 sw=4
