-- robot.lua

local controls = require("controls")
local lcd = require("lcd")
local wpilib = require("wpilib")
local leftMotor1, rightMotor1
local pairs = pairs
local driveX, driveY
module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local teleop

local controlMap

local turretEncoder, turretPID, turretSpeed`

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

        --Drive

        leftMotor1:Set(driveY)
        rightMotor1:Set(driveY)

        -- Read controls
        controls.update(controlMap)

        -- Arm
        local turretSpeed
        if turretControl then
            turretPID:stop()
            turretSpeed = turretControl
        else
            turretPid:start()
            turretPID.p = arm.turretP(currentTurretPosition, turretPID.target)
            turretSpeed = turretPID:update(currentTurretPosition)
        end
        
        turretMotor:Set(-turretSpeed)

        -- TODO

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
-- TODO
-- End Inputs/Outputs

-- Controls
-- TODO

controlMap =
{
    -- Joystick 1
    {
    ["x"] = function(axis) driveX = axis end,
    ["y"] = function(axis) driveY = axis end,
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

--Drive

-- vim: ft=lua et ts=4 sts=4 sw=4
