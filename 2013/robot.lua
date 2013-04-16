-- robot.lua

local error = error
local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer
wpilib.SmartDashboard_init()

local arm = require("arm")
local auto = require("auto")
local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local math = require("math")
local shooter = require("shooter")
local serial = require("serial")

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
local hangingPin, hangDeployOn, hangDeployOff
local driveX, driveY, quickTurn = 0, 0, false
local prepareHang, hanging = false, false
local deployIntake = false

-- STATES
local state = nil
local FIRE = "fire"
local HUMAN_LOAD = "human_load"
local STOW = "stow"
local INTAKE_LOAD = "intake_load"

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
        serial.dashboardUpdate()
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
        hangingPin:Set(false)
        hangDeployOn:Set(false)
        hangDeployOff:Set(true)
        arm.update()
        -- don't update drive, should be done in coroutine

        dashboardUpdate()
        arm.dashboardUpdate()
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
        hangingPin:Set(false)
        hangDeployOn:Set(false)
        hangDeployOff:Set(true)
        arm.update()
    end
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

        intake.update()
        shooter.update()

        hangingPin:Set(prepareHang)
        hangDeployOn:Set(hanging)
        hangDeployOff:Set(not hanging)

        -- Drive
        drive.update(driveX, driveY, quickTurn)

        -- Arm
        arm.update()

        -- Serial
        serial.update()

        -- Dashboard
        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()
        serial.dashboardUpdate()

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
hangingPin = wpilib.Solenoid(4)
hangDeployOn = wpilib.Solenoid(1)
hangDeployOff = wpilib.Solenoid(7)
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
            if lowGear then
                driveY = -deadband(axis, 0.1) / 3
            else
                driveY = -deadband(axis, 0.1)
            end
        end,

        ["rx"] = function(axis)
            if lowGear then
                driveX = deadband(axis, 0.1) / 3
            else
                driveX = deadband(axis, 0.1)
            end
        end,

        [1] = function()
            prepareHang = true
            arm.setPreset("Horizontal")
            shooter.setSideFlap(false)
        end,

        [3] = function()
            intake.goToDown(true)
        end,

        [5] = {tick=function(held)
            quickTurn = held
        end},

        [6] = {tick=function(held)
            lowGear = held
        end},

        [7] = function()
            intake.goToStow(true)
        end,

        --This is for the serial port testing
        --[[
        [9] = {tick=function(held)
            serial.setRead(held)
        end},
        -]]

        [10] = function()
            if prepareHang then
                hanging = false
            end
        end,

        ["rtrigger"] = function()
            if prepareHang then
                hanging = true
            end
        end,
    },
    -- Joystick 2 (Co-Driver)
    {
        ["y"] = function(axis)
            shooter.setRollerManual(-deadband(axis, 0.1))
            shooter.setConveyerManual(-deadband(axis, 0.1))
        end,

        ["ry"] = function(axis)
            if state == INTAKE_LOAD then
                intake.setIntakeSpeed(-deadband(axis, 0.1))
            end
        end,

        [1] = function()
            if not prepareHang then
                arm.setPreset("Intake")
                shooter.setFlywheelRunning(false)
                shooter.setFlapActive(false)
                shooter.setHardStopActive(true)
                shooter.setSideFlap(false)
                if intake.getState() == DEPLOYED then
                    intake.goToDown(true)
                end
                state = INTAKE_LOAD
            end
        end,

        [2] = function()
            if not prepareHang then
                arm.setPreset("Stow")
                shooter.setFlapActive(false)
                shooter.setFlywheelRunning(false)
                shooter.setSideFlap(false)
                if intake.getState() == DEPLOYED then
                    intake.goToDeploy(true)
                end
                state = STOW
            end
        end,

        [3] = function()
            if not prepareHang then
                arm.setPreset("sideShot")
                shooter.setFlapActive(false)
                shooter.setHardStopActive(false)
                shooter.setSideFlap(true)
                if intake.getState() == DEPLOYED then
                    intake.goToDeploy(true)
                end
                state = FIRE
            end
        end,

        [4] = function()
            if not prepareHang then
                arm.setPreset("Shooting")
                shooter.setFlapActive(false)
                shooter.setHardStopActive(false)
                shooter.setSideFlap(true)
                if intakeState == DEPLOYED then
                    intake.goToDeploy(true)
                end
                state = FIRE
            end
        end,

        [5] = {tick=shooter.humanLoad},

        [6] = {down=shooter.fireOne, up=function() shooter.fireOne(false) end},

        [7] = function()
            if not prepareHang then
                arm.setPreset("Loading")
                shooter.setFlywheelRunning(false)
                shooter.setFlapActive(true)
                shooter.setHardStopActive(true)
                shooter.setSideFlap(false)
                if intake.getState() == DEPLOYED then
                    intake.goToDown(true)
                end
                state = HUMAN_LOAD
            end
        end,


        [8] = function()
            if state == FIRE then
                shooter.setFlywheelRunning(true)
            else
                shooter.setFlywheelRunning(false)
            end
        end,

        [9] = function()
            if state == FIRE then
                shooter.setFlywheelRunning(false)
            end
        end,

        [10] = function()
            if state == HUMAN_LOAD then
                shooter.setFlapActive(false)
            elseif state == INTAKE_LOAD then
                shooter.setFlapActive(true)
            end
        end,
        
        -- TODO change back to buttong 6 when told to
        [11] = {down=shooter.fire, up=function() shooter.fire(false) end},

        ["haty"] = function(axis)
            local increment = 0.5
            if prepareHang then
                increment = 5
            end
            if axis > 0.5 and prevCoDriverDpad <= 0.5 then
                -- Dpad down
                arm.setTarget(arm.getTarget() - increment)
            end
            if axis < -0.5 and prevCoDriverDpad >= -0.5 then
                -- Dpad down
                arm.setTarget(arm.getTarget() + increment)
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

function getIntakeState()
    return intakeState
end

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
