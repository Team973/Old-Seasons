-- robot.lua

local arm = require("arm")
local controls = require("controls")
local drive = require("drive")
local lcd = require("lcd")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

module(..., package.seeall)

-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local hellautonomous, teleop, calibrate
local controlMap, strafe, rotation, gear
local clawState, intakeControl, elevatorControl, wristUp
local zeroMode
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, gearSwitch
local wristPiston
local readyMinibotSolenoid, fireMinibotSolenoid
local clawOpenPiston1, clawOpenPiston2, clawClosePiston1, clawClosePiston2
local clawSwitch, clawIntakeMotor
local elevatorMotor1, elevatorMotor2
local elevatorEncoder, elevatorPID, elevatorRateTimer
local wheels
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
    -- If you need it. :)
end

function teleop()
    lcd.print(1, "Calibrating...")
    lcd.update()

    --calibrate()
    elevatorPID:start()
    elevatorEncoder:Reset()
    elevatorEncoder:Start()
    elevatorRateTimer = wpilib.Timer()
    elevatorRateTimer:Start()

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

        if zeroMode then
            -- TODO
            for _, wheel in pairs(wheels) do
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0
                wheel.turnPID.errFunc = drive.normalizeAngle
                wheel.turnPID.target = 0
                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
                wheel.driveMotor:Set(0)
            end
        elseif not fudgeMode then
            -- TODO: gyro
            local wheelValues = drive.calculate(
                strafe.x, strafe.y, rotation, 0,
                31.4,     -- wheel base (in inches)
                21.4      -- track width (in inches)
            )
            for wheelName, value in pairs(wheelValues) do
                local wheel = wheels[wheelName]

                local deadband = 0.1
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0

                if math.abs(strafe.x) > deadband or math.abs(strafe.y) > deadband or math.abs(rotation) > deadband then
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

        -- Arm
        open1, open2, close1, close2 = arm.clawPistons(clawState)
        clawOpenPiston1:Set(open1)
        clawOpenPiston2:Set(open2)
        clawClosePiston1:Set(close1)
        clawClosePiston2:Set(close2)

        clawIntakeMotor:Set(intakeControl)

        wristPiston:Set(not wristUp)

        local elevatorSpeed
        local currentElevatorPosition = arm.elevatorEncoderToFeet(elevatorEncoder:Get())
        if elevatorControl then
            elevatorPID:stop()
            elevatorSpeed = elevatorControl
        else
            elevatorPID:start()
            elevatorPID.p = arm.elevatorP(currentElevatorPosition, elevatorPID.target)
            elevatorSpeed = elevatorPID:update(currentElevatorPosition)
        end

        elevatorMotor1:Set(-elevatorSpeed)
        elevatorMotor2:Set(-elevatorSpeed)

        lcd.print(4, "P%.2f D%.2f", arm.UP_P, elevatorPID.d)
        lcd.print(5, "P%.2f D%.2f", arm.DOWN_P, elevatorPID.d)
        --lcd.print(6, "E%.1f' T%.1f'", elevatorPID.previousError, elevatorPID.target)
        lcd.print(6, "%.2f'", currentElevatorPosition)
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
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(4, 13)
gearSwitch = wpilib.Solenoid(7, 3)

wristPiston = wpilib.Solenoid(7, 8)
readyMinibotSolenoid = wpilib.Solenoid(7, 4)
fireMinibotSolenoid = wpilib.Solenoid(7, 5)

elevatorEncoder = wpilib.Encoder(6, 1, 6, 2, false, wpilib.CounterBase_k1X)

clawOpenPiston1 = wpilib.Solenoid(7, 6)
clawOpenPiston2 = wpilib.Solenoid(7, 7)
clawClosePiston1 = wpilib.Solenoid(7, 1)
clawClosePiston2 = wpilib.Solenoid(7, 2)
clawSwitch = wpilib.DigitalInput(6, 3)
clawIntakeMotor = wpilib.Victor(6, 3)
elevatorMotor1 = wpilib.Victor(6, 4)
elevatorMotor2 = wpilib.Victor(6, 5)

elevatorPID = pid.new(1, 0, 0.01)
elevatorPID.min, elevatorPID.max = -0.5, 0.5

local turnPIDConstants = {p=0.05, i=0, d=0}

wheels = {
    frontLeft={
        shortName="FL",
        driveMotor=wpilib.Victor(4, 7),
        turnMotor=wpilib.Jaguar(4, 8),

        calibrateSwitch=wpilib.DigitalInput(4, 12),
        turnEncoder=wpilib.Encoder(4, 10, 4, 11),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    frontRight={
        shortName="FR",
        driveMotor=wpilib.Victor(4, 1),
        turnMotor=wpilib.Jaguar(4, 2),

        calibrateSwitch=wpilib.DigitalInput(4, 9),
        turnEncoder=wpilib.Encoder(4, 1, 4, 2),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearLeft={
        shortName="RL",
        driveMotor=wpilib.Victor(4, 6),
        turnMotor=wpilib.Jaguar(4, 5),

        calibrateSwitch=wpilib.DigitalInput(4, 3),
        turnEncoder=wpilib.Encoder(4, 7, 4, 8),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearRight={
        shortName="RR",
        driveMotor=wpilib.Victor(4, 3),
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
gear = false

clawState = 0
elevatorControl = nil
wristUp = true
intakeControl = 0

zeroMode = false
fudgeMode = false
fudgeWheel = nil
fudgeMovement = 0.0

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

local function presetButton(name)
    return function()
        local wristPreset = arm.presetWrist(name)
        
        elevatorPID.target = arm.presetElevatorTarget(name)

        if wristPreset ~= nil then
            wristUp = wristPreset
        end
    end
end

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = axis end,
        ["y"] = function(axis) strafe.y = -axis end,
        ["rx"] = function(axis)
            if not fudgeMode then
                rotation = axis
            else
                fudgeMovement = axis
            end
        end,
        [1] = fudgeButton(wheels.frontLeft),
        [2] = fudgeButton(wheels.rearLeft),
        [3] = fudgeButton(wheels.frontRight),
        [4] = fudgeButton(wheels.rearRight),
        [5] = {
           down=function() gear = "low" end,
           up=function() gear = "high" end,  
        },
        [10] = function() zeroMode = true end,
    },
    -- Joystick 2
    {},
    -- Joystick 3
    {
        [1] = function() clawState = 1 end,
        [2] = function() clawState = 0 end,
        [3] = {
            down=function()
                clawState = -1
                intakeControl = 1
            end,
            up=function()
                intakeControl = 0
            end,
        },
        [4] = function() wristUp = false end,
        [5] = function() wristUp = true end,
        [6] = {
            down=function() intakeControl = -1 end,
            up=function() intakeControl = 0 end,
        },
        update = function(stick)
            if stick:GetRawButton(10) then
                elevatorControl = -stick:GetY()
            elseif elevatorControl ~= nil then
                elevatorControl = 0
            end
        end,
    },
    -- Joystick 4 (eStop Module)
    {
        [2] = presetButton("slot"),
        [3] = presetButton("carry"),
        [4] = presetButton("pickup"),
        [5] = presetButton("low"),
        [6] = presetButton("middle"),
        [7] = presetButton("high"),
        [8] = presetButton("midLow"),
        [9] = presetButton("midMiddle"),
        [10] = presetButton("midHigh"),
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
