-- robot.lua

local controls = require("controls")
local lcd = require("lcd")
local wpilib = require("wpilib")

local pairs = pairs

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local teleop

local controlMap
local driveX, driveY, wristControl, intakeControl
local robotDrive

local leftDriveMotor, rightDriveMotor, wristMotor, intakeMotor
-- End Declarations

function run()
    lcd.print(1, "Ready")
    lcd.update()

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            -- TODO: run disabled function
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            -- TODO: autonomous
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

function teleop()
    disableWatchdog()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        lcd.print(1, "Running!")
        lcd.update()

        -- Read controls
        controls.update(controlMap)

        -- Drive
        robotDrive:ArcadeDrive(driveY, driveX)

        -- Arm
        wristMotor:Set(wristControl)
        intakeMotor:Set(intakeControl)

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
leftDriveMotor = wpilib.Victor(4, 10)
rightDriveMotor = wpilib.Victor(4, 9)
wristMotor = wpilib.Victor(4, 7)
intakeMotor = wpilib.Victor(4, 8)
-- End Inputs/Outputs

-- Controls
driveX, driveY = 0, 0
intakeSpeed = 0

robotDrive = wpilib.RobotDrive(leftDriveMotor, rightDriveMotor)
robotDrive:SetInvertedMotor(wpilib.RobotDrive_kFrontLeftMotor, true)
robotDrive:SetInvertedMotor(wpilib.RobotDrive_kFrontRightMotor, true)
robotDrive:SetInvertedMotor(wpilib.RobotDrive_kRearLeftMotor, true)
robotDrive:SetInvertedMotor(wpilib.RobotDrive_kRearRightMotor, true)

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) driveX = axis end,
        ["y"] = function(axis) driveY = axis end,
    },
    -- Joystick 2
    {
        ["y"] = function(axis) wristControl = axis end,
        update=function(stick)
            if stick:GetRawButton(1) then
                intakeControl = 0.6
            elseif stick:GetRawButton(2) or stick:GetRawButton(3) or stick:GetRawButton(4) or stick:GetRawButton(5) then
                intakeControl = -1
            else
                intakeControl = 0
            end
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

-- vim: ft=lua et ts=4 sts=4 sw=4
