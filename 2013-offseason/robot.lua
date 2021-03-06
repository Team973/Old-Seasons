-- robot.lua

local error = error
local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer
wpilib.SmartDashboard_init()

local auto = require("auto")
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
local hanger = wpilib.Solenoid(5)
local hanging = false
local setHanging

local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local disabledIdle, autonomous, teleop, updateCompressor
local controlMap
local deployStinger
local compressor, pressureSwitch, pressureTransducer, autoDriveSwitch
local driveX, driveY, quickTurn = 0, 0, false

-- End Declarations

function run()
    --local lw = wpilib.LiveWindow_GetInstance()
    --lw:SetEnabled(false)

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
            --lw:SetEnabled(true)
            ds:InTest(true)
            repeat ds:WaitForData() until not wpilib.IsTest() or not wpilib.IsEnabled()
            ds:InTest(false)
            --lw:SetEnabled(false)
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
    -- local pressureVoltageMin = 1.17
    wpilib.SmartDashboard_PutBoolean("pressure", pressureSwitch:Get())
    wpilib.SmartDashboard_PutBoolean("HANGING", hanging)
    wpilib.SmartDashboard_PutNumber("Pressure Transducer", pressureTransducer:GetVoltage())
end

function disabledIdle()
    while wpilib.IsDisabled() do
        feedWatchdog()

        --Load Dashboard outputs
        dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()
        intake.dashboardUpdate()

        feedWatchdog()
        wpilib.Wait(AUTO_LOOP_LAG)
        feedWatchdog()
    end
end

function autonomous()
    disableWatchdog()

    local c = coroutine.create(auto.run)

    while wpilib.IsAutonomous() and wpilib.IsEnabled() and coroutine.status(c) ~= "dead" do
        local success, err = coroutine.resume(c)
        if not success then
            error(err)
        end

        updateCompressor()
        intake.update()
        shooter.update()
        -- don't update drive, should be done in coroutine

        dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    -- Clean up
    shooter.fullStop()
    drive.update(0, 0, false)

    -- Make sure systems don't do dangerous things at the end of autonomous
    while wpilib.IsAutonomous() and wpilib.IsEnabled() do
        drive.update(0, 0, false)
        updateCompressor()
        intake.update()
        shooter.update()
    end
end


function teleop()
    local HANG_CONSTANT = 1.5
    local hangTimer = wpilib.Timer()
    hangTimer:Start()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()


        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        updateCompressor()

        intake.update()
        shooter.update()

        -- Drive
        drive.update(driveX, driveY, quickTurn)

        --Hanger
        hanger:Set(hanging)

        -- Dashboard
        dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()
        shooter.dashboardUpdate()
        intake.dashboardUpdate()

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

function setHanging(bool)
    hanging = bool
end

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
            driveX = deadband(-axis, 0.1)
        end,

        [1] = function()
            setHanging(false)
        end,

        [3] = function()
            setHanging(true)
        end,

        [4] = function()
        end,

        [5] = {tick=function(held)
            quickTurn = held
        end},

        [6] = {tick=function(held)
            drive.setLowGear(held)
        end},

        [7] = {tick=function(held)
            drive.punchTheGround(held)
        end},

        [10] = function()
        end,

        ["rtrigger"] = function()
        end,
    },
    -- Joystick 2 (Co-Driver)
    {
        ["y"] = function(axis)
        end,

        ["ry"] = function(axis)
        end,

        [1] = function()
            shooter.setPreset("fullCourt")
        end,

        [2] = function()
            shooter.setPreset("Pyramid")
        end,

        [3] = function()
            shooter.setPreset("midGoal")
        end,

        [4] = function()
        end,


        [5] = {tick=function(held)
            shooter.setRollerRunning(held)
        end},

        --[6] = {down=shooter.fire, up=function() shooter.fire(false) end},

        [6] = {tick=function(held)
            shooter.setIndexer(held)
        end},

        [7] = function()
        end,

        [8] = {tick=function(held)
            shooter.setFlywheelRunning(held)
        end},

        [9] = function()
            shooter.setTargetRPM(shooter.getTargetRPM() - 50)
        end,
        
        [10] = function()
            shooter.setTargetRPM(shooter.getTargetRPM() + 50)
        end,
        
        [11] = {down=shooter.fireOne, up=function() shooter.fireOne(false) end},

        [12] = function()
        end,

        ["haty"] = function(axis)
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
