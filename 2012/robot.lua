-- robot.lua

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

    minibot.startGameTimer()

    --calibrate()
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

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function calibrateAll(wheels)
    local numWheels = 0
    local numCalibratedWheels = 0
    local allCalibrated = false
    for _,wheel in pairs(wheels) do
        wheel.turnMotor:set(1)
        wheel.calibrateState = 1
        numWheels = numWheels + 1
    end
    while allCalibrated == false do
        for _,wheel in pairs(wheels) do
            if wheel.calibrateState == 1 then
                if wheel.calibrateSwitch1:Get() then
                    wheel.switch1Angle = wheel.turnEncoder:Get()
                    wheel.calibrateState = wheel.calibrateState + 1
                    wheel.turnMotor:Set(-1)
                end
            elseif wheel.calibrateState == 2 then
                if wheel.calibrateSwitch2:Get() then
                    wheel.turnMotor:Set(1)
                    wheel.switch2Angle = wheel.turnEncoder:Get()
                    wheel.calibrateState = 3
                    wheel.targetAngle = (wheel.switch1Angle + wheel.switch2Angle)/2
                end
            elseif wheel.calibrateState == 3 then
                if wheel.turnEncoder:Get() == wheel.targetAngle then
                    wheel.calibrateState = 4
                    numCalibratedWheels = numCalibratedWheels + 1
                    if numWheels == numCalibratedWheels then
                        allCalibrated = true
                    end
                end
            end    
        end
    end
end 


function calibrate(wheel)
    local middleAngle
	while not wheel.calibrateSwitch1:Get() do
		wheel.turnMotor:Set(1)
	end 

	local angle1 = wheel.turnEncoder:Get()

	while not wheel.calibrateSwitch2:Get() do
		wheel.turnMotor:Set(-1)
	end

	local angle2 = wheel.turnEncoder:Get() 
	
	while not -- zeroed do
        angle1 - angle2 = middleAngle 
	   wheel.turnMotor:Set = middleAngle 5--find middle (/2) stored angles
		-- move motor clockwise
        -- motor moves tell encoder hits middle angle and tells it to stop
	end 

	wheel.turnEncoder:Reset()
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

ate = 0
presetShift = false

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
