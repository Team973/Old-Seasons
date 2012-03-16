-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer

local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local lcd = require("lcd")
local linearize = require("linearize")
local math = require("math")
local turret = require("turret")

local pairs = pairs
local tostring = tostring

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local hellautonomous, teleop, calibrateAll
local controlMap, strafe, rotation, gear, presetShift, fieldCentric
local deploySkid
local zeroMode, possessionTimer, rotationHoldTimer
local fudgeMode, fudgeWheel, fudgeMovement

local compressor, pressureSwitch, gearSwitch
local frontSkid
local wheels
local driveMode = 0

-- End Declarations

local dashboard = wpilib.SmartDashboard_GetInstance()

dashboard:PutString("mode", "Waiting for Gyro...")

function run()
    turret.initI2C()
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

    local RPM = 6600
    local HOOD_TARGET = 1200

    local t = wpilib.Timer()
    t:Start()
    while t:Get() < 2.0 and wpilib.IsAutonomous() and wpilib.IsEnabled() do
        -- Set up for key shot
        turret.setFlywheelTargetSpeed(RPM)
        turret.setHoodTarget(HOOD_TARGET)

        -- Don't run intake or conveyer yet.
        intake.setVerticalSpeed(0.0)
        intake.setCheaterSpeed(0.0)
        intake.setIntake(0.0)

        -- Update
        turret.update()
        intake.update()

        -- Pneumatics
        dashboard:PutBoolean("pressure", pressureSwitch:Get())
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end

        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    t:Reset()
    while t:Get() < 5.0 and wpilib.IsAutonomous() and wpilib.IsEnabled() do
        -- Set up for key shot
        turret.setFlywheelTargetSpeed(RPM)
        turret.setHoodTarget(HOOD_TARGET)

        -- Run conveyer, but no intake.
        intake.setVerticalSpeed(0.2)
        intake.setCheaterSpeed(1.0)
        intake.setIntake(0.0)

        -- Update
        turret.update()
        intake.update()

        -- Pneumatics
        dashboard:PutBoolean("pressure", pressureSwitch:Get())
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end

        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    turret.fullStop()
    intake.fullStop()
end

function teleop()
    turret.turnPID:start()
    for _, wheel in pairs(wheels) do
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
        wheel.turnPID:start()
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
            dashboard:PutString(wheel.shortName .. ".turnEncoder", wheel.turnEncoder:GetDistance())
        end

        -- Read controls
        if fudgeMode then
            controls.update(fudgeControlMap)
        else
            controls.update(controlMap)
        end
        feedWatchdog()

        -- Pneumatics
        dashboard:PutBoolean("pressure", pressureSwitch:Get())
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end

        frontSkid:Set(deploySkid)

        intake.update(true)
        turret.update()

        dashboard:PutDouble("Flywheel P", turret.flywheelPID.p)
        dashboard:PutDouble("Flywheel D", turret.flywheelPID.d)
        dashboard:PutDouble("Flywheel Speed", turret.getFlywheelSpeed())
        dashboard:PutInt("Flywheel Speed(Int)", turret.getFlywheelSpeed())
        dashboard:PutDouble("Flywheel Target Speed", turret.getFlywheelTargetSpeed())

        -- Drive
        if gear == "low" then
            gearSwitch:Set(true)
        elseif gear == "high" then
            gearSwitch:Set(false)
        else
            -- Unrecognized state, default to low gear
            -- TODO: log error
            gearSwitch:Set(false)
        end

        if zeroMode then
            for _, wheel in pairs(wheels) do
                local currentTurn = wheel.turnEncoder:GetDistance()
                wheel.turnPID.errFunc = drive.normalizeAngle
                wheel.turnPID.target = 0
                wheel.turnPID:update(currentTurn)
                wheel.turnMotor:Set(wheel.turnPID.output)
                wheel.driveMotor:Set(0)
            end
        elseif fudgeMode then
            -- Fudge mode
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
                local currentTurn = wheel.turnEncoder:GetDistance()

                if math.abs(strafe.x) > deadband or math.abs(strafe.y) > deadband or math.abs(appliedRotation) > deadband then
                    wheel.turnPID.target = drive.normalizeAngle(value.angleDeg)
                    local driveScale = drive.driveScale(drive.calculateTurn(currentTurn, wheel.turnPID.target))
                    wheel.driveMotor:Set(value.speed * driveScale)
                else
                    -- In deadband
                    if driveMode == 0 then
                        if wheelName == "leftFront" or wheelName == "rightBack" then
                            wheel.turnPID.target = 45
                        else
                            wheel.turnPID.target = -45
                        end
                    elseif driveMode == 1 then
                        wheel.turnPID.target = 0
                    elseif driveMode == 2 then
                        wheel.turnPID.target = 90 
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

function calibrateAll()
    dashboard:PutString("mode", "Calibrating")

    local tolerance = 3.0 -- in degrees
    local numCalibratedWheels = 0
    local numWheels = 4
    local speed = 1.0
    for _, wheel in pairs(wheels) do
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
        if wheel.calibrateSwitch:Get() then
            -- Already calibrated
            wheel.calibrateState = 6
            numCalibratedWheels = numCalibratedWheels + 1
        else
            wheel.calibrateState = 1
        end
    end
    while numCalibratedWheels < numWheels do
        for _, wheel in pairs(wheels) do
            if wheel.calibrateState == 1 then
                -- Initial state: turn clockwise, wait until tripped
                wheel.turnMotor:Set(speed)
                if wheel.calibrateSwitch:Get() then
                    wheel.calibrateState = 2
                end
            elseif wheel.calibrateState == 2 then
                -- Keep turning clockwise until falling edge
                wheel.turnMotor:Set(speed)
                if not wheel.calibrateSwitch:Get() then
                    wheel.Angle1 = wheel.turnEncoder:GetDistance()
                    wheel.calibrateState = 3
                end
            elseif wheel.calibrateState == 3 then
                -- Turn counter-clockwise, wait until tripped
                wheel.turnMotor:Set(-speed)
                if wheel.calibrateSwitch:Get() then
                    wheel.calibrateState = 4
                end
            elseif wheel.calibrateState == 4 then
                -- Keep turning counter-clockwise until falling edge
                wheel.turnMotor:Set(-speed)
                if not wheel.calibrateSwitch:Get() then
                    wheel.Angle2 = wheel.turnEncoder:GetDistance()
                    wheel.calibrateState = 5
                end
            elseif wheel.calibrateState == 5 then
                -- Move clockwise back to zero
                wheel.turnMotor:Set(speed)
                local targetAngle = (wheel.Angle1 + wheel.Angle2)/2
                local dist = math.abs(wheel.turnEncoder:GetDistance() - targetAngle)
                if dist < tolerance then
                    wheel.turnMotor:Set(0)
                    wheel.turnEncoder:Reset()
                    numCalibratedWheels = numCalibratedWheels + 1
                    wheel.calibrateState = 6
                end
            end
        end

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
pressureSwitch = wpilib.DigitalInput(1, 14)
gearSwitch = wpilib.Solenoid(1, 1)
frontSkid = wpilib.Solenoid(3)

local turnPIDConstants = {p=0.06, i=0, d=0}

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
        turnMotor=LinearVictor(1, 2),

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
turretDirection = {x=0, y=0} 

zeroMode = false
fudgeMode = false
fudgeWheel = nil
fudgeMovement = 0.0

local function fudgeButton(wheel)
    return {
        down=function()
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

local rpmPreset = 3300.0
local prevOperatorDpad = 0.0
local function presetValues (flywheelRPM, hoodAngle, turretAngle)
    rpmPreset = flywheelRPM
    turret.setHoodTarget(hoodAngle)
    turret.setTargetAngle(turretAngle)
end

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = deadband(axis, 0.15) end,
        ["y"] = function(axis) strafe.y = deadband(-axis, 0.15) end,
        ["rx"] = function(axis)
            rotation = axis
        end,
        -- [5] = {tick=function(held)
            -- if held then
              -- gear = "low"
           -- else
             -- gear = "high"
          -- end
        -- end},
        [6] = {tick=function(held) deploySkid = held end},
        [7] = function() fudgeMode = true end,
        [8] = function() calibrateAll() end,
        [10] = {tick=function(held)
            if held then
                if gear == "low" then
                    rotation = rotation * 0.5
                elseif gear == "high" then
                    rotation = rotation * 0.5
                end
            end
        end},
                                
        ["update"] = function(stick)
            if controls.isLeftTriggerHeld(stick) then
                strafe.x = 0
                driveMode = 1
            elseif controls.isRightTriggerHeld(stick) then
                strafe.x = strafe.y
                strafe.y = 0
                driveMode = 2
            else
                driveMode = 0
            end
        end

    },
    -- Joystick 2
    {
        ["x"] = function(axis)
            turretDirection.x = deadband(axis, 0.2)
            turret.setFromJoy(turretDirection.x, turretDirection.y) 
        end,
        
        ["y"] = function(axis) 
            turretDirection.y = deadband(-axis, 0.2) 
            turret.setFromJoy(turretDirection.x, turretDirection.y)
        end,
        ["rx"] = function(axis)
            intake.setIntake(deadband(axis, 0.2))
        end,
        ["ry"] = function(axis)
            intake.setCheaterSpeed(deadband(-axis, 0.2))
            intake.setVerticalSpeed(deadband(-axis, 0.2))
        end,
        ["hatx"] = function(axis)
            local increment = 1
            if axis > 0.5 and prevOperatorDpad <= 0.5 then
                -- Dpad right
                turret.setHoodTarget(turret.getHoodTarget() + 10)
            end
            if axis < -0.5 and prevOperatorDpad >= -0.5 then
                -- Dpad left
                turret.setHoodTarget(turret.getHoodTarget() - 10)
            end
            prevOperatorDpad = axis
        end,
        [1] = function() presetValues(3300,20,0) end, -- Fender
        [2] = function() presetValues(4500,200,0) end, -- Side
        [3] = function() presetValues(6600,1200,0) end, -- Key
        [5] = {tick=function(held) intake.setLowered(held) end},   
        [6] = {tick=function(held)
            if held then
                intake.setVerticalSpeed(0.2)
                intake.setCheaterSpeed(1.0)
            end
        end},
        [7] = function()
            rpmPreset = rpmPreset - 100
        end,
        [8] = function()
            rpmPreset = rpmPreset + 100
        end,
        [9] = {tick=function(held) turret.allowRotate = held end},   
        ["ltrigger"] = {tick=function(held)
            if held then
                intake.setIntake(1.0)
            end
        end},
        ["rtrigger"] = {
            down=function()
                turret.resetFlywheel()
            end,
            tick=function(held)
                if held then
                    turret.setFlywheelTargetSpeed(rpmPreset)
                else
                    turret.setFlywheelTargetSpeed(0.0)
                end
            end,
        },
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

fudgeControlMap = {
    -- Joystick 1
    {
        ["x"] = function(axis)
            fudgeMovement = deadband(axis, 0.15)
        end,
        [1] = fudgeButton(wheels.rightBack),
        [2] = fudgeButton(wheels.rightFront),
        [3] = fudgeButton(wheels.leftBack),
        [4] = fudgeButton(wheels.leftFront),
        [7] = function()
            fudgeMode = false
            for _, wheel in pairs(wheels) do
                wheel.turnEncoder:Reset()
            end
        end,
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
