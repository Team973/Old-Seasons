-- robot.lua
-- Based on RossTest.cpp

require "config"
require "kicker"
require "wpilib"

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)
stick3 = wpilib.Joystick(3)

-- WPILib shortcuts
local printLCD, updateLCD
if config.features.lcd then
    local lcd = wpilib.DriverStationLCD_GetInstance()
    
    function printLCD(line, msg)
        lcd:PrintLine(line, msg)
    end
    
    function updateLCD()
        lcd:UpdateLCD()
    end
else
    printLCD = function() end
    updateLCD = function() end
end

-- Watchdog shortcuts
local feedWatchdog, enableWatchdog, disableWatchdog
if config.watchdogEnabled then
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

-- I/O
leftMotor1 = config.leftMotor1
leftMotor2 = config.leftMotor2
rightMotor1 = config.rightMotor1
rightMotor2 = config.rightMotor2
gearSwitch = config.gearSwitch
intakeMotor = config.intakeMotor
compressor = config.compressor
pressureSwitch = config.pressureSwitch

-- Globals
drive = wpilib.RobotDrive(leftMotor1, leftMotor2, rightMotor1, rightMotor2)
hiGear = false

-- Robot running
function run()
    printLCD(wpilib.DriverStationLCD_kUser_Line1, "Robot init")
    updateLCD()
    
    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            -- TODO: run disabled function
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
        elseif wpilib.IsAutonomous() then
            autonomous()
            repeat wpilib.Wait(0.01) until not wpilib.IsAutonomous() or not wpilib.IsEnabled()
        else
            teleop()
            repeat wpilib.Wait(0.01) until not wpilib.IsOperatorControl() or not wpilib.IsEnabled()
        end
    end
end

function autonomous()
    disableWatchdog()
    drive:Drive(0.5, 0.0)
    wpilib.Wait(2.0)
    drive:Drive(0.0, 0.0)
end

function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()
        
        --[==[
        if stick3:GetRawButton(6) then
            printLCD(wpilib.DriverStationLCD_kUser_Line2, "Did RESTART!")
            updateLCD()
            restartRobot()
        end
        --]==]
        
        driveJoysticks()
        feedWatchdog()
        
        printLCD(wpilib.DriverStationLCD_kUser_Line1, "Running!")
        updateLCD()
        
        -- Pneumatics
        
        if config.features.compressor then
            if pressureSwitch:Get() then
                compressor:Set(wpilib.Relay_kOff)
            else
                compressor:Set(wpilib.Relay_kOn)
            end
        end
        
        if config.features.gearSwitch then
            gearSwitch:Set(not hiGear)
        end
        
        feedWatchdog()
        
        -- Intake
        if stick3:GetRawButton(3) then
            intakeMotor:Set(-1.0)
        elseif stick3:GetRawButton(2) then
            intakeMotor:Set(1.0)
        else
            intakeMotor:Set(0.0)
        end
        feedWatchdog()
        
        -- Kicker
        kicker.setThirdZoneEnabled(stick3:GetRawButton(6))
        if stick3:GetRawButton(1) then
            kicker.fire()
        end
        kicker.update()
        feedWatchdog()
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

do
    local lastLo, lastHi = false, false
    function driveJoysticks()
        drive:ArcadeDrive(-stick1:GetY(), -stick2:GetX())
        
        if stick1:GetRawButton(2) and not lastLo then
            hiGear = false
        elseif stick1:GetRawButton(1) and not lastHi then
            hiGear = true
        end
        
        lastLo, lastHi = stick1:GetRawButton(2), stick1:GetRawButton(1)
    end
end
