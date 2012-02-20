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
local wheels

-- End Declarations

local dashboard = wpilib.SmartDashboard_GetInstance()

dashboard:PutString("mode", "Waiting for Gyro...")

function run()
    dashboard:PutString("mode", "Ready")

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
        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

function teleop()
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
            dashboard:PutString("mode", "Running")
        else
            dashboard:PutString("mode", "Fudge Mode")
        end
        for _, wheel in pairs(wheels) do
            dashboard:PutString(wheel.shortName .. ".turnEncoder", wheel.turnEncoder:GetRaw() / 4.0)
        end

        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        --[[
        dashboard:PutBoolean("pressure", pressureSwitch:Get())
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end
        --]]

        -- Drive
        --[[
        if gear == "low" then
            gearSwitch:Set(false)
        elseif gear == "high" then
            gearSwitch:Set(true)
        else
            -- Unrecognized state, default to low gear
            -- TODO: log error
            gearSwitch:Set(false)
        end
        --]]

        if zeroMode then
            for _, wheel in pairs(wheels) do
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0
                wheel.turnPID.errFunc = drive.normalizeAngle
                wheel.turnPID.target = 0
                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
                wheel.driveMotor:Set(0)
            end
        elseif fudgeMode then
            -- Fudge mode
            -- TODO: Don't use this, just calibrate
            for _, wheel in pairs(wheels) do
                wheel.driveMotor:Set(0)
                wheel.turnMotor:Set(0)
            end

            if fudgeWheel then
                fudgeWheel.turnMotor:Set(fudgeMovement)
            end
        else
            local appliedGyro = 0.0
            local appliedRotation = rotation
            local deadband = 0.1
            
            -- Keep rotation steady in deadband
            if math.abs(rotation) < deadband then
                if rotationHoldTimer == nil then
                    rotationHoldTimer = wpilib.Timer()
                    rotationHoldTimer:Start()
                elseif rotationHoldTimer:Get() > 0.5 then
                    rotationHoldTimer:Stop()
                    rotationHoldTimer = nil
                end
                appliedRotation = 0
            end
            
            local wheelValues = drive.calculate(
                strafe.x, strafe.y, appliedRotation, appliedGyro,
                31.5,     -- wheel base (in inches)
                21.4      -- track width (in inches)
            )

            for wheelName, value in pairs(wheelValues) do
                local wheel = wheels[wheelName]

                local deadband = 0.1
                local currentTurn = wheel.turnEncoder:GetRaw() / 4.0

                if math.abs(strafe.x) > deadband or math.abs(strafe.y) > deadband or math.abs(appliedRotation) > deadband then
                    wheel.turnPID.target = drive.normalizeAngle(value.angleDeg)
                    local driveScale = drive.driveScale(drive.calculateTurn(currentTurn, wheel.turnPID.target))
                    wheel.driveMotor:Set(value.speed * driveScale)
                else
                    -- In deadband
                    if wheelName == "leftFront" or wheelName == "rightBack" then
                        wheel.turnPID.target = 45
                    else
                        wheel.turnPID.target = -45
                    end
                    wheel.driveMotor:Set(0)
                end

                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
            end
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

--[[
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(4, 13)
gearSwitch = wpilib.Solenoid(7, 3)
--]]

local turnPIDConstants = {p=0.05, i=0, d=0}

wheels = {
    leftFront={
        shortName="LF",
        driveMotor=LinearVictor(1, 7),
        turnMotor=LinearVictor(1, 8),

        calibrateSwitch=wpilib.DigitalInput(1, 12),
        turnEncoder=wpilib.Encoder(1, 10, 1, 11),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    leftBack={
        shortName="LB",
        driveMotor=LinearVictor(1, 5),
        turnMotor=LinearVictor(1, 6),

        calibrateSwitch=wpilib.DigitalInput(1, 9),
        turnEncoder=wpilib.Encoder(1, 7, 1, 8),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rightFront={
        shortName="RF",
        driveMotor=LinearVictor(1, 3),
        turnMotor=LinearVictor(1, 4),

        calibrateSwitch=wpilib.DigitalInput(1, 6),
        turnEncoder=wpilib.Encoder(1, 4, 1, 5),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
    rightBack={
        shortName="RB",
        driveMotor=LinearVictor(1, 1),
        turnMotor=wpilib.Jaguar(1, 2),

        calibrateSwitch=wpilib.DigitalInput(1, 3),
        turnEncoder=wpilib.Encoder(1, 1, 1, 2),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, drive.angleError),
    },
}

for _, wheel in pairs(wheels) do
    wheel.turnEncoder:SetDistancePerPulse(50.0 / 38.0)
    wheel.turnEncoder:SetReverseDirection(true)
end
-- End Inputs/Outputs

-- Controls
strafe = {x=0, y=0}
rotation = 0
gear = "high"

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
        ["x"] = function(axis) strafe.x = deadband(axis, 0.15) end,
        ["y"] = function(axis) strafe.y = deadband(-axis, 0.15) end,
        ["rx"] = function(axis)
            if not fudgeMode then
                rotation = axis
            else
                fudgeMovement = deadband(axis, 0.15)
            end
        end,
        ["ltrigger"] = {tick=function(held) fieldCentric = held end},
        [1] = fudgeButton(wheels.rightBack),
        [2] = fudgeButton(wheels.rightFront),
        [3] = fudgeButton(wheels.leftBack),
        [4] = fudgeButton(wheels.leftFront),
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
