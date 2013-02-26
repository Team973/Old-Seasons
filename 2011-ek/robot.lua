-- robot.lua

local arm = require("arm")
local controls = require("controls")
local drive = require("drive")
local lcd = require("lcd")
local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")
local minibot = require("minibot")

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
local clawState, intakeControl, elevatorControl, wristUp
local zeroMode, possessionTimer, rotationHoldTimer
local wristIntakeTime = 0.1
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, gearSwitch
local gyroChannel, gyroPID, ignoreGyro
ignoreGyro = false
local wristPiston
local readyMinibotSolenoid, fireMinibotRelay
local clawOpenPiston1, clawOpenPiston2, clawClosePiston1, clawClosePiston2
local clawSwitch, clawIntakeMotor
local elevatorMotor1, elevatorMotor2
local elevatorEncoder, elevatorPID, elevatorRateTimer
local wheels

local hasTube = false
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

    minibot.startGameTimer()

    --calibrate()
    elevatorPID:start()
    elevatorEncoder:Reset()
    elevatorEncoder:Start()
    elevatorRateTimer = wpilib.Timer()
    elevatorRateTimer:Start()
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

        -- Arm
        if not hasTube and intakeControl > 0 and clawState == -1 then
            if not clawSwitch:Get() then
                if not possessionTimer then
                    -- The limit switch just got activated
                    possessionTimer = wpilib.Timer()
                    possessionTimer:Start()
                elseif possessionTimer:Get() > wristIntakeTime then
                    -- We've waited for the set time. We now have a tube.
                    possessionTimer:Stop()
                    possessionTimer = nil
                    hasTube = true
                    clawState = 0
                    wristUp = true
               end
            end
        end

        local open1, open2, close1, close2 = arm.clawPistons(clawState)
        clawOpenPiston1:Set(open1)
        clawOpenPiston2:Set(open2)
        clawClosePiston1:Set(close1)
        clawClosePiston2:Set(close2)

        clawIntakeMotor:Set(intakeControl)
        if hasTube and clawSwitch:Get() then
            clawIntakeMotor:Set(1)
        end

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
        
        readyMinibotOutput, fireMinibotOutput = minibot.update()    
        readyMinibotSolenoid:Set(readyMinibotOutput)
        if not fireMinibotOutput then
            fireMinibotRelay:Set(wpilib.Relay_kOff)
        else
            fireMinibotRelay:Set(wpilib.Relay_kOn)
        end

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

compressor = wpilib.Relay(1, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1, 13)
gearSwitch = wpilib.Solenoid(2, 3)

gyroChannel = wpilib.AnalogChannel(1, 2)
gyroPID = pid.new(0.05, 0, 0)

wristPiston = wpilib.Solenoid(2, 8)
readyMinibotSolenoid = wpilib.Solenoid(2, 4)
fireMinibotRelay = wpilib.Relay(1, 1, wpilib.Relay_kReverseOnly)

elevatorEncoder = wpilib.Encoder(2, 1, 2, 2, false, wpilib.CounterBase_k1X)

clawOpenPiston1 = wpilib.Solenoid(2, 6)
clawOpenPiston2 = wpilib.Solenoid(2, 7)
clawClosePiston1 = wpilib.Solenoid(2, 1)
clawClosePiston2 = wpilib.Solenoid(2, 2)
clawSwitch = wpilib.DigitalInput(2, 3)
clawIntakeMotor = LinearVictor(2, 3)
elevatorMotor1 = LinearVictor(2, 4)
elevatorMotor2 = LinearVictor(2, 5)

elevatorPID = pid.new(0, 0, 0.0005)
elevatorPID.min, elevatorPID.max = -1.0, 1.0

local turnPIDConstants = {p=0.05, i=0, d=0}

wheels = {
    frontLeft={
        shortName="FL",
        driveMotor=LinearVictor(1, 7),
        turnMotor=wpilib.Jaguar(1, 8),

        calibrateSwitch=wpilib.DigitalInput(1, 12),
        turnEncoder=wpilib.Encoder(1, 10, 1, 11),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    frontRight={
        shortName="FR",
        driveMotor=LinearVictor(1, 1),
        turnMotor=wpilib.Jaguar(1, 2),

        calibrateSwitch=wpilib.DigitalInput(1, 9),
        turnEncoder=wpilib.Encoder(1, 1, 1, 2),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearLeft={
        shortName="RL",
        driveMotor=LinearVictor(1, 6),
        turnMotor=wpilib.Jaguar(1, 5),

        calibrateSwitch=wpilib.DigitalInput(1, 3),
        turnEncoder=wpilib.Encoder(1, 7, 1, 8),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rearRight={
        shortName="RR",
        driveMotor=LinearVictor(1, 3),
        turnMotor=wpilib.Jaguar(1, 4),

        calibrateSwitch=wpilib.DigitalInput(1, 6),
        turnEncoder=wpilib.Encoder(2, 13, 2, 14),
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

local function doPreset(name)
    local wristPreset = arm.presetWrist(name)

    elevatorPID.target = arm.presetElevatorTarget(name)

    if wristPreset ~= nil then
        wristUp = wristPreset
    end
end

local function presetButton(nonShiftName, shiftName)
    return function()
        if presetShift then
            doPreset(shiftName)
        else
            doPreset(nonShiftName)
        end
    end
end

local function deadband(axis, threshold)
    if axis < threshold and axis > -threshold then
        return 0
    else
        return axis
    end
end

local function grabElevatorTarget()
    elevatorPID.target = arm.elevatorEncoderToFeet(elevatorEncoder:Get())
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
        ["y"] = function(axis)
            if axis < -0.5 then
                wristUp = true
            elseif axis > 0.5 then
                wristUp = false
            end
        end,
        ["ry"] = function(axis)
            if math.abs(axis) > 0.2 then
                elevatorControl = -axis * 0.3
            elseif elevatorControl then
                -- Now switching to manual
                grabElevatorTarget()
                elevatorControl = nil
            end
        end,
        ["hatx"] = function(axis)
            presetShift = (axis > 0.5)

            if lastHatX > -0.5 and axis < -0.5 then
                doPreset("carry")
            end

            lastHatX = axis
        end,
        ["rtrigger"] = function()
            if hasTube then
                clawState = -1
                local newTarget = arm.elevatorEncoderToFeet(elevatorEncoder:Get()) - 1.0
                if newTarget < 0 then
                    newTarget = 0
                end
                elevatorPID.target = newTarget

                -- The operator pulled the trigger. Let it go. JUST LET IT GO.
                hasTube = false
            else
                clawState = 1
            end
        end,
        [1] = presetButton("low", "midLow"),
        [2] = presetButton("middle", "midMiddle"),
        [4] = presetButton("high", "midHigh"),
        [5] = function()
            clawState = -1
            -- This also runs intake, see update.
        end,
        [6] = function() clawState = 0 end,
        [7] = minibot.toggleReady,
        [8] = {tick=function(held)
            if held and minibot.deploymentTimerFinished() then
                minibot.deploy()
            end
        end},
        update = function(stick)
            if stick:GetRawButton(5) then
                intakeControl = 1
            elseif stick:GetRawButton(3) then
                intakeControl = -1
            else
                intakeControl = 0
            end
        end,
    },
    -- Joystick 3
    {
        [2] = incConstant(gyroPID, "p", gyroPID, -0.01),
        [3] = incConstant(gyroPID, "p", gyroPID, 0.01),
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
