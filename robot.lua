-- robot.lua

require "config"
require "drive"
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
compressor = config.compressor
pressureSwitch = config.pressureSwitch

if config.features.grabber then
    grabberMotor = config.grabberMotor
end

-- Robot running
function run()
    printLCD(wpilib.DriverStationLCD_kUser_Line1, "Robot init")
    updateLCD()
    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            -- TODO: run disabled function
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
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
    disableWatchdog()
    -- Do nothing...
end

function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()
        
        if stick3:GetRawButton(9) then
            restartRobot()
        end
        
        drive.drive(stick1, stick2)
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

        -- Manual Gripper Control
        if config.features.grabber then
            if stick3:GetRawButton(6) then
                grabberMotor:Set(config.grabberManualSpeed) 
            elseif stick3:GetRawButton(7) then
                grabberMotor:Set(-(config.grabberManualSpeed))
            else
                grabberMotor:Set(0)
            end
        end    
        
        feedWatchdog()
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
