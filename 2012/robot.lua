-- robot.lua

local controls = require("controls")
local drive = require("drive")
local lcd = require("lcd")
local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs
local tostring = tostring

module(..., package.seeall)

-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local hellautonomous, teleop, calibrate
local controlMap, strafe, rotation, gear, presetShift, fieldCentric
local zeroMode, possessionTimer, rotationHoldTimer
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, gearSwitch
local gyroChannel, gyroPID, ignoreGyro
ignoreGyro = false
local wheels

-- End Declarations

lcd.print(1, "RESETTING GYRO")
lcd.update()

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
    while wpilib.IsAutonomous() and wpilib.IsEnabled() do
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end
        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

function teleop()
    lcd.print(1, "Calibrating...")
    lcd.update()
    gyroPID:start()

    for _, wheel in pairs(wheels) do
        wheel.turnPID:start()

        -- TODO: Don't do this once we're calibrating right.
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
    end

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        if not fudgeMode then
            lcd.print(1, "Running!")
        else
            lcd.print(1, "Fudge Mode")
        end
        local i = 2
        for _, wheel in pairs(wheels) do
            lcd.print(i, "%s A%.1f", wheel.shortName, wheel.turnEncoder:GetRaw() / 4.0)
            i = i + 1
        end

        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end

        -- Drive
        if gear == "low" then
            gearSwitch:Set(false)
        elseif gear == "high" then
            gearSwitch:Set(true)
        else
            -- Unrecognized state, default to low gear
            -- TODO: log error
            gearSwitch:Set(false)
        end

        local gyroAngle = 0

        if zeroMode then
            for _, wheel in pairs(wheels) do
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0
                wheel.turnPID.errFunc = drive.normalizeAngle
                wheel.turnPID.target = 0
                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
                wheel.driveMotor:Set(0)
            end
        elseif not fudgeMode then
            local appliedGyro, appliedRotation = gyroAngle, rotation
            local deadband = 0.1
            
            if not fieldCentric then
                appliedGyro = 0
            end
            -- Keep rotation steady in deadband
            if math.abs(rotation) < deadband then
                if gyroPID.target == nil then
                    if rotationHoldTimer == nil then
                        rotationHoldTimer = wpilib.Timer()
                        rotationHoldTimer:Start()
                    elseif rotationHoldTimer:Get() > 0.5 then
                        rotationHoldTimer:Stop()
                        rotationHoldTimer = nil
                        gyroPID.target = gyroAngle
                        gyroPID:start()
                    end
                    appliedRotation = 0
                else
                    appliedRotation = gyroPID:update(gyroAngle)
                end
            else
                gyroPID.target = nil
                gyroPID:stop()
            end
            
            local wheelValues = drive.calculate(
                strafe.x, strafe.y, appliedRotation, appliedGyro,
                31.4,     -- wheel base (in inches)
                21.4      -- track width (in inches)
            )

            for wheelName, value in pairs(wheelValues) do
                local wheel = wheels[wheelName]

                local deadband = 0.1
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0

                if math.abs(strafe.x) > deadband or math.abs(strafe.y) > deadband or math.abs(appliedRotation) > deadband then
                    wheel.turnPID.target = drive.normalizeAngle(value.angleDeg)
                    local driveScale = drive.driveScale(drive.calculateTurn(currentTurn, wheel.turnPID.target))
                    wheel.driveMotor:Set(value.speed * -driveScale)
                else
                    -- In deadband
                    if wheelName == "frontLeft" or wheelName == "rearRight" then
                        wheel.turnPID.target = 45
                    else
                        wheel.turnPID.target = -45
                    end
                    wheel.driveMotor:Set(0)
                end

                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
            end
        else
            -- Fudge mode
            -- TODO: Don't use this, just calibrate
            for _, wheel in pairs(wheels) do
                wheel.driveMotor:Set(0)
                wheel.turnMotor:Set(0)
            end

            if fudgeWheel then
                fudgeWheel.turnMotor:Set(fudgeMovement)
            end
        end

        lcd.print(4, "P%.2f %.2f", gyroPID.p, gyroPID.previousError)
        if gyroPID.target then
            lcd.print(5, "%.2f %.2f", gyroAngle, gyroPID.target)
        else
            lcd.print(5, "%.2f none", gyroAngle)
        end
        --[[
        lcd.print(5, "P%.2f D%.5f", arm.DOWN_P, elevatorPID.d)
        lcd.print(4, "%s %.1f", tostring(presetShift), controls.sticks[2]:GetRawAxis(6))
        --]]
        lcd.update()
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function calibrate()
    local calibState = {}
    local TURN_SPEED = 1.0
    for name, _ in pairs(wheels) do
        calibState[name] = false
    end

    local keepGoing = true
    while keepGoing and wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        keepGoing = false
        for name, calibrated in pairs(calibState) do
            local wheel = wheels[name]
            if calibrated then
                wheel.turnMotor:Set(0)
            elseif wheel.calibrateSwitch:Get() then
                -- Stop running motor
                wheel.turnMotor:Set(0)

                -- Mark as calibrated
                calibState[name] = true
                wheel.turnEncoder:Reset()
                wheel.turnEncoder:Start()
            else
                -- Have not reached point yet
                keepGoing = true
                wheel.turnMotor:Set(TURN_SPEED)
            end
            wheel.driveMotor:Set(0)
        end

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(4, 13)
gearSwitch = wpilib.Solenoid(7, 3)

gyroChannel = wpilib.AnalogChannel(1, 2)
gyroPID = pid.new(0.05, 0, 0)

local turnPIDConstants = {p=0.05, i=0, d=0}

wheels = {
    frontLeft={
        shortName="FL",
        driveMotor=LinearVictor(4, 7),
        turnMotor=wpilib.Jaguar(4, 8),

        calibrateSwitch=wpilib.DigitalInput(4, 12),
        turnEncoder=wpilib.Encoder(4, 10, 4, 11),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    frontRight={
        shortName="FR",
        driveMotor=LinearVictor(4, 1),
        turnMotor=wpilib.Jaguar(4, 2),

        calibrateSwitch=wpilib.DigitalInput(4, 9),
        turnEncoder=wpilib.Encoder(4, 1, 4, 2),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearLeft={
        shortName="RL",
        driveMotor=LinearVictor(4, 6),
        turnMotor=wpilib.Jaguar(4, 5),

        calibrateSwitch=wpilib.DigitalInput(4, 3),
        turnEncoder=wpilib.Encoder(4, 7, 4, 8),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearRight={
        shortName="RR",
        driveMotor=LinearVictor(4, 3),
        turnMotor=wpilib.Jaguar(4, 4),

        calibrateSwitch=wpilib.DigitalInput(4, 6),
        turnEncoder=wpilib.Encoder(6, 13, 6, 14),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
}

for _, wheel in pairs(wheels) do
    wheel.turnEncoder:SetDistancePerPulse(1.0)
    wheel.turnEncoder:SetReverseDirection(true)
end
-- End Inputs/Outputs

-- Controls
strafe = {x=0, y=0}
rotation = 0
gear = "high"

clawState = 0
elevatorControl = nil
presetShift = false
wristUp = true
intakeControl = 0

zeroMode = false
fudgeMode = false
fudgeWheel = nil
fudgeMovement = 0.0

local lastHatX = 0

local function fudgeButton(wheel)
    return {
        down=function()
            fudgeMode = true
            fudgeWheel = wheel
        end,
        up=function()
            if fudgeWheel == wheel then
                fudgeWheel = nil
            end
        end,
    }
end

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

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = deadband(-axis, 0.15) end,
        ["y"] = function(axis) strafe.y = deadband(axis, 0.15) end,
        ["rx"] = function(axis)
            if not fudgeMode then
                rotation = axis
            else
                fudgeMovement = deadband(axis, 0.15)
            end
        end,
        ["ltrigger"] = {tick=function(held) fieldCentric = held end},
        [1] = fudgeButton(wheels.rearRight),
        [2] = fudgeButton(wheels.frontRight),
        [3] = fudgeButton(wheels.rearLeft),
        [4] = fudgeButton(wheels.frontLeft),
        [5] = {tick=function(held)
            if held then
                gear = "low"
            else
                gear = "high"
            end
        end},
        [6] = {tick=function(held)
            if held then
                if gear == "low" then
                    rotation = rotation * 0.5
                elseif gear == "high" then
                    rotation = rotation * 0.5
                end
            end
        end},
        [7] = function () ignoreGyro = true end, 
        [10] = function() zeroMode = true end,
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

-- vim: ft=lua et ts=4 sts=4 sw=4
