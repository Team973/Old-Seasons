-- robot.lua

local arm = require "arm"
local bit = require "bit"
local config = require "config"
local configmode = require "configmode"
local controls = require "controls"
local drive = require "drive"
local lcd = require "lcd"
local math = require "math"
local minibot = require "minibot"
local pid = require "pid"
local wpilib = require "wpilib"
local format = string.format

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

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
gearSwitch = config.gearSwitch
compressor = config.compressor
pressureSwitch = config.pressureSwitch

if config.features.grabber then
    grabberMotor = config.grabberMotor
end

local sendVisionData, sendIOPortData

-- Robot running
function run()
    lcd.print(1, "Ready")
    lcd.print(2, config.profileName)
    lcd.update()
    -- Initialize subsystems
    drive.init()
    arm.init()
    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            -- TODO: run disabled function
            disableWatchdog()
            repeat wpilib.Wait(0.01) until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            hellautonomous()
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

function hellautonomous()
    disableWatchdog()
    config.leftDriveEncoder:Reset()
    config.rightDriveEncoder:Reset()
    config.gyro:Reset()
    minibot.unready()


    local speed = 0.35
    local driveP = 3
    local targetDistance = 14.1 -- in feet
    local distanceBallpark = 0.5
    local drivePID, turnDrivePID
    local turnBias = 0
    local intakeTimer = wpilib.Timer()
    local intakeDuration = 1.0
    local calmTimer = wpilib.Timer()
    local rampUpTime = 2.0

    drivePID = pid.PID:new(driveP, 0, 0)
    drivePID.min, drivePID.max = -speed, speed
    drivePID:reset()
    drivePID:start()
    drivePID.target = -targetDistance

    turnDrivePID = pid.PID:new(0.18, 0, 0.017)
    -- Austin says no cap here.
    --turnDrivePID.min, turnDrivePID.max = -1, 1
    turnDrivePID:reset()
    turnDrivePID:start()
    turnDrivePID.target = 0

    local released = false

    local voltageBallpark = 0.1
    arm.closeClaw()
    drive.setGear(false)
    arm.setGripMotor(1)
    intakeTimer:Start()
    drive.getDrive():SetLeftRightMotorOutputs(0, 0)

    local autoMode = 0
    if config.modeSwitch1:Get() then autoMode = bit.bor(autoMode, 0x1) end
    if config.modeSwitch2:Get() then autoMode = bit.bor(autoMode, 0x2) end
    lcd.print(6, "MODE=" .. autoMode)
    lcd.update()
    if autoMode == 0 then
        return
    end
    
    calmTimer:Start()
    while wpilib.IsAutonomous() and not wpilib.IsDisabled() do
        local distance = drive.getDistance()
        if math.abs(distance - drivePID.target) < distanceBallpark then
            break
        end
        if intakeTimer:Get() > intakeDuration then
            arm.setGripMotor(0)
        end
        -- Update drive
        local angle = drive.getAngle()
        drivePID:update(distance)
        turnDrivePID:update(angle)
        local speedL = (turnDrivePID.output + turnBias)
        local speedR = -(turnDrivePID.output + turnBias)
        if calmTimer:Get() < rampUpTime then
            speedL = speedL - (calmTimer:Get() / rampUpTime * speed)
            speedR = speedR - (calmTimer:Get() / rampUpTime * speed)
        else
            speedL = speedL + drivePID.output
            speedR = speedR + drivePID.output
        end
        if math.abs(speedL) > 1 or math.abs(speedR) > 1 then
            local n = math.max(math.abs(speedL), math.abs(speedR))
            speedL = speedL / n
            speedR = speedR / n
        end
        drive.getDrive():SetLeftRightMotorOutputs(drive.linearize(speedL), drive.linearize(speedR))
        lcd.print(3, format("%.2f %.2f", angle, turnDrivePID.output))
        lcd.print(4, format("%.2f %.2f", config.leftDriveEncoder:GetDistance(), config.rightDriveEncoder:GetDistance()))
        lcd.update()
        -- Update arm
        arm.update()
        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    arm.setGripMotor(0)
    arm.setForward(false)
    if autoMode == 1 then
        arm.setPreset("high")
    else
        arm.setPreset("midHigh")
    end

    while wpilib.IsAutonomous() and not wpilib.IsDisabled() and not released do
        if math.abs(arm.getArmVoltage() - config.armPID.target) < voltageBallpark and math.abs(arm.getWristVoltage() - config.wristPID.target) < voltageBallpark then
            arm.openClaw()
            arm.runWristDown()
            released = true
        end
        drivePID:update(drive.getDistance())
        turnDrivePID:update(drive.getAngle())
        local speedL = drivePID.output + (turnDrivePID.output + turnBias)
        local speedR = drivePID.output - (turnDrivePID.output + turnBias)
        if math.abs(speedL) > 1 or math.abs(speedR) > 1 then
            local n = math.max(math.abs(speedL), math.abs(speedR))
            speedL = speedL / n
            speedR = speedR / n
        end
        drive.getDrive():SetLeftRightMotorOutputs(drive.linearize(speedL), drive.linearize(speedR))
        arm.update()
        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    local driveBackTimer = wpilib.Timer()
    local driveBackDelay = 2.0
    local driveBackDuration = 1.0
    driveBackTimer:Start()

    while wpilib.IsAutonomous() and not wpilib.IsDisabled() do
        if driveBackTimer:Get() > driveBackDelay + driveBackDuration then
            drive.getDrive():SetLeftRightMotorOutputs(0, 0)
            arm.setGripMotor(0)
        elseif driveBackTimer:Get() > driveBackDelay then
            drive.getDrive():SetLeftRightMotorOutputs(speed, speed)
            arm.setGripMotor(-0.4)
        else
            drive.getDrive():SetLeftRightMotorOutputs(0, 0)
            arm.setGripMotor(-0.4)
        end
        arm.update()
        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

local function bool2yn(bool)
    if bool then
        return "Y"
    else
        return "N"
    end
end

function teleop()
    local inConfig = false
    minibot.startGameTimer()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        if controls.sticks[4]:GetRawButton(11) and not inConfig then
            configmode.start()
            inConfig = true
        elseif not controls.sticks[4]:GetRawButton(11) and inConfig then
            configmode.finish()
            inConfig = false
        end

        if inConfig then
            lcd.print(1, "CONFIG MODE")
        else
            lcd.print(1, "Running!")
        end

        -- Read controls
        if not inConfig then
            controls.update(controls.defaultControls)
            local armPIDOut = -config.armPID.output
            local wristPIDOut = config.wristPID.output
            lcd.print(2, format("L=%.2f %d", config.leftDriveEncoder:GetDistance(), config.leftDriveEncoder:Get()))
            lcd.print(3, format("R=%.2f %d", config.rightDriveEncoder:GetDistance(), config.rightDriveEncoder:Get()))
            --[[
            lcd.print(2, format("Arm=%.2f Out=%.2f", arm.getArmVoltage(), armPIDOut))
            lcd.print(3, format("Err=%.2f", config.armPID.target - arm.getArmVoltage()))
            lcd.print(4, format("Wrist=%.2f Out=%.2f", arm.getWristVoltage(), wristPIDOut))
            lcd.print(5, format("Err=%.2f", config.wristPID.target - arm.getWristVoltage()))
            --]]
            lcd.print(6, format("Tube=%s Switch=%s", bool2yn(arm.getHasTube()), bool2yn(not config.wristIntakeSwitch:Get())))
            lcd.update()
        else
            controls.update(configmode.controlMap)
            configmode.update()
        end
        feedWatchdog()

        -- Update subsystems
        drive.update()
        arm.update()
        minibot.update()
        feedWatchdog()

        -- Pneumatics
        if config.features.compressor then
            if pressureSwitch:Get() then
                compressor:Set(wpilib.Relay_kOff)
            else
                compressor:Set(wpilib.Relay_kOn)
            end
        end

        -- Send dashboard data
        --[[
        sendVisionData()
        feedWatchdog()
        sendIOPortData()
        feedWatchdog()
        --]]
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

-- Dashboard Data
local presetTable = {
    pickup=0,
    stow=1,
    slot=2,
    vertical=3,
    carry=4,
    low=5,
    middle=6,
    high=7,
    midLow=8,
    midMiddle=9,
    midHigh=10,
}
local visionTimer = wpilib.Timer()

function sendVisionData()
    visionTimer:Start()
    if not visionTimer:HasPeriodPassed(0.1) then return end
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
    end
    dash:FinalizeCluster()
    dash:Finalize()
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
    ioTimer:Start()
    if not ioTimer:HasPeriodPassed(0.1) then return end
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

        --Robot Variables
        if arm.getPreset() ~= nil then
            dash:AddU8(presetTable[arm.getPreset()])
        else
            dash:AddU8(255)
        end
        
        local stateTable = arm.getGripperState()
        if stateTable.clawNumb ~= nil then
            dash:AddI8(stateTable.clawNumb)
        else
            dash:AddI8(100)
        end                

        dash:AddBoolean(arm.getForward())

    end
    dash:FinalizeCluster()
    dash:Finalize()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
