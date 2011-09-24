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
local armPreset, elevatorMovement, clawState, intakeOn, wristUp
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, gearSwitch
local elevatorEncoder, elevatorMotorA, elevatorMotorB
local wristUpPiston, wristDownPiston
local clawOpenPiston, clawClosePiston, clawSwitch, clawIntakeMotor
local elevatorPID
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

    for _, wheel in pairs(wheels) do
        wheel.turnPID:start()

        -- TODO: Don't do this once we're calibrating right.
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
    end

    elevatorEncoder:Reset()
    elevatorEncoder:Start()
    elevatorPID:start()

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
            --lcd.print(i, "%s T%.1f O%.1f", wheel.shortName, wheel.turnPID.target, wheel.turnPID.output)
            lcd.print(i, "%s A%.1f", wheel.shortName, wheel.turnEncoder:GetRaw() / 4.0)
            i = i + 1
        end
        local turnPID = wheels.frontLeft.turnPID
        lcd.print(6, "P%.4f D%.4f", turnPID.p, turnPID.d)
        lcd.update()

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

        if not fudgeMode then
            -- TODO: gyro
            local wheelValues = drive.calculate(
                strafe.x, strafe.y, rotation, 0,
                31.4,     -- wheel base (in inches)
                21.4      -- track width (in inches)
            )
            for wheelName, value in pairs(wheelValues) do
                local wheel = wheels[wheelName]

                local deadband = 0.1
                if math.abs(strafe.x) > deadband or math.abs(strafe.y) > deadband or math.abs(rotation) > deadband then
                    wheel.driveMotor:Set(-value.speed)
                    wheel.turnPID.target = value.angleDeg
                else
                    -- In deadband
                    wheel.driveMotor:Set(0)
                end

                wheel.turnPID:update(wheel.turnEncoder:GetRaw() / 4.0)
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
        if elevatorMovement then
            elevatorMotorA:Set(elevatorMovement)
            elevatorMotorB:Set(elevatorMovement)
        else
            if armPreset then
                elevatorPID.p = arm.presetElevatorTarget(armPreset)
            end

            local currentElevatorPosition = elevatorEncoder:GetRaw() / 4.0
            elevatorPID.p = arm.elevatorP(currentElevatorPosition, elevatorPID.target)
            elevatorPID:update(currentElevatorPosition)
            elevatorMotorA:Set(elevatorPID.output)
            elevatorMotorB:Set(elevatorPID.output)
        end

        openPistonValue, closePistonValue = arm.clawPistons(clawState)
        clawOpenPiston:Set(openPistonValue)
        clawClosePiston:Set(closePistonValue)

        if intakeOn then
            clawIntakeMotor:Set(1)
        else
            clawIntakeMotor:Set(0)
        end

        if wristUp then
            wristUpPiston:Set(true)
            wristDownPiston:Set(false)
        else
            wristUpPiston:Set(false)
            wristDownPiston:Set(true)
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
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(4, 13)
gearSwitch = wpilib.Solenoid(7, 1)

elevatorEncoder = wpilib.Encoder(6, 1, 6, 2)
elevatorMotorA = wpilib.Victor(6, 1)
elevatorMotorB = wpilib.Victor(6, 2)

wristUpPiston = wpilib.Solenoid(7, 4)
wristDownPiston = wpilib.Solenoid(7, 5)

clawOpenPiston = wpilib.Solenoid(7, 2)
clawClosePiston = wpilib.Solenoid(7, 3)
clawSwitch = wpilib.DigitalInput(6, 3)
clawIntakeMotor = wpilib.Victor(6, 3)

elevatorPID = pid.new(arm.elevatorP(0, 0), 0, 0)

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
        turnEncoder=wpilib.Encoder(6, 10, 6, 11),
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
gear = "low"

armPreset = "bottom"
elevatorMovement = 0.0
clawState = 0
intakeOn = false

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

local function incConstant(constant, delta)
    for _, wheel in pairs(wheels) do
        wheel.turnPID:stop()
        wheel.turnPID[constant] = wheel.turnPID[constant] + delta
        wheel.turnPID:reset()
        wheel.turnPID:start()
    end
end

local function presetButton(name)
    return function()
        armPreset = name
        local wristPreset = arm.presetWrist(name)
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
        [1] = {down=function() gear = "low" end},
        [6] = {down=function() incConstant("p", 0.001) end}, -- up
        [7] = {down=function() incConstant("p", -0.001) end}, -- down
        [11] = {down=function() incConstant("d", 0.001) end}, -- up
        [10] = {down=function() incConstant("d", -0.001) end}, -- down
    },
    -- Joystick 2
    {
        ["x"] = function(axis)
            if not fudgeMode then
                rotation = axis
            else
                fudgeMovement = axis
            end
        end,
        [1] = {down=function() gear = "high" end},
        [6] = fudgeButton(wheels.frontLeft),
        [7] = fudgeButton(wheels.rearLeft),
        [11] = fudgeButton(wheels.frontRight),
        [10] = fudgeButton(wheels.rearRight),
    },
    -- Joystick 3
    {
        ["y"] = function(axis) elevatorControl = axis end,
        [1] = function() clawState = 1 end,
        [2] = function() clawState = 0 end,
        [3] = {
            down=function()
                clawState = -1
                intakeOn = true
            end,
            up=function()
                intakeOn = false
            end,
        },
        [4] = function() wristUp = true end,
        [5] = function() wristUp = false end,
        -- TODO: presets
        update = function(stick)
            if math.abs(stick:GetX()) > 0.5 then
                armPreset = nil
                elevatorMovement = stick:GetY()
            else
                elevatorMovement = nil
            end
        end,
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
