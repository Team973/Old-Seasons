-- robot.lua

local controls = require("controls")
local drive = require("drive")
local lcd = require("lcd")
local math = require("math")
local pid = require("pid")
local string = require("string")
local wpilib = require("wpilib")

local pairs = pairs

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local hellautonomous, teleop, calibrate
local controlMap, strafe, rotation, gear
local compressor, pressureSwitch, gearSwitch, wheels
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

    local fudgeMode = false

    for _, wheel in pairs(wheels) do
        wheel.turnPID:start()

        -- TODO: Don't do this once we're calibrating right.
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
    end

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        lcd.print(1, "Running!")
        local i = 2
        for _, wheel in pairs(wheels) do
            lcd.print(i, string.format("%s E%.1f O%.1f", wheel.shortName, wheel.turnPID.previousError, wheel.turnPID.output))
            i = i + 1
        end
        local turnPID = wheels.frontLeft.turnPID
        lcd.print(6, string.format("P%.4f D%.4f", turnPID.p, turnPID.d))
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
        --[==[
        if gear == "low" then
            gearSwitch:Set(false)
        elseif gear == "high" then
            gearSwitch:Set(true)
        else
            -- Unrecognized state, default to low gear
            -- TODO: log error
            gearSwitch:Set(false)
        end

        -- TODO: gyro
        local wheelValues = drive.calculate(
            strafe.x, strafe.y, rotation, 0,
            32,     -- wheel base (in inches)
            22      -- track width (in inches)
        )
        for wheelName, value in pairs(wheelValues) do
            local wheel = wheels[wheelName]
            wheel.driveMotor:Set(value.speed)
            wheel.turnPID.target = value.angleDeg
            wheel.turnPID:update(wheel.turnEncoder:GetRaw() / 4.0)
            wheel.turnMotor:Set(wheel.turnPID.output)
        end
        --]==]

        -- FL: 6
        -- RL: 7
        -- FR: 11
        -- RR: 10
        local fudgeStick = controls.sticks[2]
        if fudgeStick:GetRawButton(6) or fudgeStick:GetRawButton(7) or fudgeStick:GetRawButton(10) or fudgeStick:GetRawButton(11) then
            fudgeMode = true
        end

        if not fudgeMode then
            -- Target a direction
            local angle = math.deg(math.atan2(controls.sticks[1]:GetX(), -controls.sticks[1]:GetY()))
            for _, wheel in pairs(wheels) do
                wheel.turnPID.target = angle
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

            local wheel = nil
            if fudgeStick:GetRawButton(6) then
                wheel = wheels.frontLeft
            elseif fudgeStick:GetRawButton(7) then
                wheel = wheels.rearLeft
            elseif fudgeStick:GetRawButton(11) then
                wheel = wheels.frontRight
            elseif fudgeStick:GetRawButton(10) then
                wheel = wheels.rearRight
            end
            if wheel then
                wheel.turnMotor:Set(fudgeStick:GetX())
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
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(4, 13)
gearSwitch = wpilib.Solenoid(8, 1)

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
        turnEncoder=wpilib.Encoder(4, 4, 4, 5),
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

local function incConstant(constant, delta)
    for _, wheel in pairs(wheels) do
        wheel.turnPID:stop()
        wheel.turnPID[constant] = wheel.turnPID[constant] + delta
        wheel.turnPID:reset()
        wheel.turnPID:start()
    end
end

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = axis end,
        ["y"] = function(axis) strafe.y = axis end,
        [1] = {down=function() gear = "low" end},
        [6] = {down=function() incConstant("p", 0.001) end}, -- up
        [7] = {down=function() incConstant("p", -0.001) end}, -- down
        [11] = {down=function() incConstant("d", 0.001) end}, -- up
        [10] = {down=function() incConstant("d", -0.001) end}, -- down
    },
    -- Joystick 2
    {
        ["x"] = function(axis) rotation = axis end,
        [1] = {down=function() gear = "high" end}
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
