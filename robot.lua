-- robot.lua

require "config"
require "intake"
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
compressor = config.compressor
pressureSwitch = config.pressureSwitch

-- Globals
drive = wpilib.RobotDrive(leftMotor1, leftMotor2, rightMotor1, rightMotor2)
hiGear = false

-- Robot running
function run()
    printLCD(wpilib.DriverStationLCD_kUser_Line1, "Robot init")
    updateLCD()
    config.intakeEncoder:Start()    
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
    local timer = wpilib.Timer() 
    local started = false
    local stopTime = -1
    local initialDelay = config.defaultInitialDelay
    if config.autonomousSwitch:Get() then
        initialDelay = config.longInitialDelay
    end
    timer:Start()
    drive:Drive(0.0,0.0)
    while wpilib.IsAutonomous() and not wpilib.IsDisabled() do
        if config.features.compressor then
            if pressureSwitch:Get() then
                compressor:Set(wpilib.Relay_kOff)
            else
                compressor:Set(wpilib.Relay_kOn)
            end
        end
        intake.update()
        kicker.update()
        if not started then
            if timer:Get() > initialDelay then
               started = true
               intake.changeState(1)
               timer:Reset()
            end
        else
            if stopTime >= 0 then
                if timer:Get() - stopTime >= 1 then
                    kicker.fire()
                    timer:Reset()
                    stopTime = -1
                end
            elseif timer:Get() > 1 and intake.hasBall() then
                drive:Drive(0.0,0.0)
                stopTime = timer:Get() 
            elseif kicker.isReady() then
                drive:Drive(0.4,0.0)
                stopTime = -1
            end
        end
        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

local lastKickTrigger = false
function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()
        
        if stick3:GetRawButton(9) then
            restartRobot()
        end
        
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
        -- Make sure this always runs before kicker.
	
    	if stick3:GetRawButton(2) or stick3:GetRawButton(3) or stick3:GetRawButton(4) or stick3:GetRawButton(5) then
            intake.changeState(1)
        elseif stick3:GetRawButton(6) then
            intake.changeState(-1)
        else
            intake.changeState(0)
        end
        intake.update()
        printLCD(wpilib.DriverStationLCD_kUser_Line4, "Ball: " .. tostring(intake.hasBall()) )
        updateLCD()
        feedWatchdog()
        
        -- Kicker
        if stick3:GetRawButton(1) and not lastKickerTrigger then
            kicker.fire()
        end
        kicker.update()
        lastKickerTrigger = stick3:GetRawButton(1)
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
