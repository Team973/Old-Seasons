-- robot.lua
-- Based on RossTest.cpp

module(..., package.seeall)

features =
{
    compressor = false,
    gearSwitch = false,
    lcd = true,
}

local TELEOP_LOOP_LAG = 0.005

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)
stick3 = wpilib.Joystick(3)

-- Drive
leftMotor1 = wpilib.Victor(1)
leftMotor2 = wpilib.Victor(3)
rightMotor1 = wpilib.Victor(2)
rightMotor2 = wpilib.Victor(4)

drive = wpilib.RobotDrive(leftMotor1, leftMotor2, rightMotor1, rightMotor2)

gearSwitch = wpilib.Solenoid(1)
hiGear = false

intakeMotor = wpilib.Victor(6, 3)

-- Pneumatics
compressor = wpilib.Relay(1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- WPILib shortcuts
local printLCD, updateLCD

if features.lcd then
    local lcd = wpilib.DriverStationLCD_GetInstance()
    
    function printLCD(line, msg)
        lcd:PrintLine(line, msg)
    end
    
    function updateLCD()
        lcd:UpdateLCD()
    end
else
    function printLCD() end
    function updateLCD() end
end

local function feedWatchdog()
    local dog = wpilib.getWatchdog()
    dog:Feed()
end

local function enableWatchdog()
    local dog = wpilib.getWatchdog()
    dog:SetEnabled(true)
end

local function disableWatchdog()
    local dog = wpilib.getWatchdog()
    dog:SetEnabled(false)
end

-- Robot running
function run()
    wpilib.getWatchdog():SetExpiration(0.25)
    
    printLCD(wpilib.DriverStationLCD_kUser_Line1, "Robot init")
    updateLCD()
    
    -- Main loop
    while true do
        if wpilib.isDisabled() then
            -- TODO: run disabled function
            repeat wpilib.Wait(0.01) until not wpilib.isDisabled()
        elseif wpilib.isAutonomous() then
            autonomous()
            repeat wpilib.Wait(0.01) until not wpilib.isAutonomous() or not wpilib.isEnabled()
        else
            teleop()
            repeat wpilib.Wait(0.01) until not wpilib.isOperatorControl() or not wpilib.isEnabled()
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
    while wpilib.isOperatorControl() and wpilib.isEnabled() do
        enableWatchdog()
        feedWatchdog()
        
        driveJoysticks()
        feedWatchdog()
        
        printLCD(wpilib.DriverStationLCD_kUser_Line1, "Running!")
        updateLCD()
        
        -- Pneumatics
        if features.compressor then
            if pressureSwitch:Get() then
                compressor:Set(wpilib.Relay_kOff)
            else
                compressor:Set(wpilib.Relay_kOn)
            end
        end
        
        if features.gearSwitch then
            gearSwitch:Set(not hiGear)
        end
        
        feedWatchdog()
        
        -- Intake
        if stick3:GetRawButton(2) then
            intakeMotor:Set(-1.0)
        elseif stick3:GetRawButton(1) then
            intakeMotor:Set(1.0)
        else
            intakeMotor:Set(0.0)
        end
        feedWatchdog()

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function driveJoysticks()
    drive:ArcadeDrive(-stick1:GetY(), -stick2:GetX())
end
