-- robot.lua

local controls = require("controls")
local drive = require("drive")
local lcd = require("lcd")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local hellautonomous, teleop
local controlMap, strafe, rotation
local compressor, pressureSwitch, wheels
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
    -- TODO: Calibrate

    for _, wheel in pairs(wheels) do
        wheel.turnPID:start()
    end

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        lcd.print(1, "Running!")

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
        -- TODO: gyro
        local wheelValues = drive.calculate(
            strafe.x, strafe.y, rotation, 0,
            32,     -- wheel base (in inches)
            22      -- track width (in inches)
        )
        for wheelName, value in pairs(wheelValues) do
            local wheel = wheels[wheelName]
            wheel.driveMotor:Set(value.speed)
            -- TODO: wheel.turnPID:update(current)
            wheel.turnMotor:Set(wheel.turnPID.output)
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
pressureSwitch = wpilib.DigitalInput(1)
wheels = {
    frontLeft={
        driveMotor=wpilib.Victor(1),
        turnMotor=wpilib.Victor(2),
        -- TODO: Tune loop
        turnPID=pid.new(1, 0, 0, drive.angleError),
    },
    frontRight={
        driveMotor=wpilib.Victor(3),
        turnMotor=wpilib.Victor(4),
        -- TODO: Tune loop
        turnPID=pid.new(1, 0, 0, drive.angleError),
    },
    rearLeft={
        driveMotor=wpilib.Victor(5),
        turnMotor=wpilib.Victor(6),
        -- TODO: Tune loop
        turnPID=pid.new(1, 0, 0, drive.angleError),
    },
    rearRight={
        driveMotor=wpilib.Victor(7),
        turnMotor=wpilib.Victor(8),
        -- TODO: Tune loop
        turnPID=pid.new(1, 0, 0, drive.angleError),
    },
}
-- End Inputs/Outputs

-- Controls
strafe = {x=0, y=0}
rotation = 0

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) strafe.x = axis end,
        ["y"] = function(axis) strafe.y = axis end,
        [1] = {down=function() end}
    },
    -- Joystick 2
    {
        ["x"] = function(axis) rotation = axis end,
        [1] = {down=function() end}
    },
    -- Joystick 3
    {
        [1] = {
            down=function() end,
            up=function() end,
        },
        update = function(stick) end,
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
