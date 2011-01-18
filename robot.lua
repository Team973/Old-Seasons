-- robot.lua

require "pid"
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
    local lineConstants = {
        wpilib.DriverStationLCD_kUser_Line1,
        wpilib.DriverStationLCD_kUser_Line2,
        wpilib.DriverStationLCD_kUser_Line3,
        wpilib.DriverStationLCD_kUser_Line4,
        wpilib.DriverStationLCD_kUser_Line5,
        wpilib.DriverStationLCD_kUser_Line6,
    }

    function printLCD(line, msg)
        lcd:PrintLine(lineConstants[line], msg)
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
armMotor = config.armMotor

if config.features.grabber then
    grabberMotor = config.grabberMotor
end

local sendVisionData, sendIOPortData

-- Robot running
function run()
    printLCD(1, "Robot init")
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


local armPID = config.armPID
function teleop()
    armPID:reset()
    armPID:start()
    armPID.target = config.armPreset1
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()
        
        if stick3:GetRawButton(9) then
            restartRobot()
        end
        
        drive.drive(stick1, stick2)
        feedWatchdog()
        
        printLCD(1, "Running!")
        updateLCD()
        
        -- Pneumatics
        if config.features.compressor then
            if pressureSwitch:Get() then
                compressor:Set(wpilib.Relay_kOff)
            else
                compressor:Set(wpilib.Relay_kOn)
            end
        end

        -- Arm joint
        if stick3:GetRawButton(4) then
            armPID.target = config.armPreset1
            armMotor:Set(armPID:update(config.armPot:GetVoltage())) 
        elseif stick3:GetRawButton(3) then
            armPID.target = config.armPreset2
            armMotor:Set(armPID:update(config.armPot:GetVoltage())) 
        elseif stick3:GetRawButton(5) then
            armPID.target = config.armPreset3
            armMotor:Set(armPID:update(config.armPot:GetVoltage())) 
        else
            armMotor:Set(0)
        end
        

        -- Manual grabber control
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

        -- Send dashboard data
        sendVisionData()
        feedWatchdog()
        sendIOPortData()
        feedWatchdog()
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

-- Dashboard Data
local visionTimer = wpilib.Timer()
function sendVisionData()
    if not visionTimer:HasPeriodPassed(0.1) then
        return
    end
    local dash = wpilib.DriverStation_GetInstance():GetHighPriorityDashboardPacker()

    dash:AddCluster()
    do
        dash:AddCluster()   -- tracking data
        do
            dash:AddDouble(0.0) -- Joystick X
            dash:AddDouble(0.0) -- Angle
            dash:AddDouble(0.0) -- Angular Rate
            dash:AddDouble(0.0) -- Other X
        end
        dash:FinalizeCluster()
        dash:AddCluster()   -- target info (2 elements)
        do
            dash:AddArray() -- targets
            dash:FinalizeArray()

            dash:AddU32(0)  -- Timestamp
        end
        dash:FinalizeCluster()
        dash:Finalize()
    end
end

local function dIOHardware2Logical(dio)
    local result = 0
    local bit = require "bit"

    for i = 0, 15 do
        if bit.band(dio, bit.lshift(1, i)) ~= 0 then
            result = bit.bor(result, bit.rshift(1, 16 - i - 1))
        end
    end
    return result
end

local ioTimer = wpilib.Timer()
function sendIOPortData()
    if not visionTimer:HasPeriodPassed(0.1) then return end
    local dash = wpilib.DriverStation_GetInstance():GetLowPriorityDashboardPacker()
    dash:AddCluster()
    do
        dash:AddCluster()   -- analog modules
        do
            dash:AddCluster()
            for i = 1, 8 do
                dash:AddFloat(wpilib.AnalogModule_GetInstance(1):GetAverageVoltage(i))
            end
            dash:FinalizeCluster()
            dash:AddCluster()
            for i = 1, 8 do
                --dash:AddFloat(AnalogModule_GetInstance(2):GetAverageVoltage(i))
                dash:AddFloat(0.0)
            end
            dash:FinalizeCluster()
        end
        dash:FinalizeCluster()

        dash:AddCluster()   -- digital modules
        do
            dash:AddCluster()
            do
                dash:AddCluster()
                do
                    local m = wpilib.DigitalModule_GetInstance(4)
                    dash:AddU8(m:GetRelayForward())
                    dash:AddU8(m:GetRelayReverse())
                    dash:AddU16(dIOHardware2Logical(m:GetDIO()))
                    dash:AddU16(dIOHardware2Logical(m:GetDIODirection()))
                    dash:AddCluster()
                    do
                        for i = 1, 10 do
                            dash:AddU8(m:GetPWM(i))
                        end
                    end
                    dash:FinalizeCluster()
                end
                dash:FinalizeCluster()
            end
            dash:FinalizeCluster()
            dash:AddCluster()
            do
                dash:AddCluster()
                do
                    local m = wpilib.DigitalModule_GetInstance(4)
                    dash:AddU8(0)
                    dash:AddU8(0)
                    dash:AddU16(0)
                    dash:AddU16(0)
                    dash:AddCluster()
                    do
                        for i = 1, 10 do
                            dash:AddU8(0)
                        end
                    end
                    dash:FinalizeCluster()
                end
                dash:FinalizeCluster()
            end
            dash:FinalizeCluster()
        end
        dash:FinalizeCluster()

        -- Solenoids
        dash:AddU8(0)
    end
    dash:FinalizeCluster()
    dash:Finalize()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
