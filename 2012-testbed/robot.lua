-- robot.lua

local controls = require("controls")
local lcd = require("lcd")
local linearize = require("linearize")
local pid = require ("pid")
local wpilib = require("wpilib")
local pairs = pairs

local x, motors, encoders
local motorNum, encoderNum = 1, 1

module(..., package.seeall)

pid.PID.timerNew = wpilib.Timer

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local teleop

local controlMap
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
            -- TODO: autonomous
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

local dashboard = wpilib.SmartDashboard_GetInstance()

dashboard:PutDouble("x", 0.0)

function teleop()
    disableWatchdog()

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        lcd.print(1, "Running!")
        lcd.update()

        -- Read controls
        controls.update(controlMap)

        for _, motor in ipairs(motors) do
            motor:Set(0.0)
        end
        motors[motorNum]:Set(x)

        dashboard:PutDouble("x", x)
        dashboard:PutDouble("encoder", encoders[encoderNum]:Get())
        dashboard:PutInt("motorNum", motorNum)
        dashboard:PutInt("encoderNum", encoderNum)
        dashboard:PutBoolean("switch (LF)", switches[1]:Get())
        dashboard:PutBoolean("switch (LB)", switches[2]:Get())
        dashboard:PutBoolean("switch (RF)", switches[3]:Get())
        dashboard:PutBoolean("switch (RB)", switches[4]:Get())

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

motors = {
    LinearVictor(1, 7),
    LinearVictor(1, 8),
    LinearVictor(1, 5),
    LinearVictor(1, 6),
    LinearVictor(1, 3),
    LinearVictor(1, 4),
    LinearVictor(1, 1),
    LinearVictor(1, 2),
}

encoders = {
    wpilib.Encoder(1, 10, 1, 11, false, wpilib.CounterBase_k1X),
    wpilib.Encoder(1, 7, 1, 8, false, wpilib.CounterBase_k1X),
    wpilib.Encoder(1, 4, 1, 5, false, wpilib.CounterBase_k1X),
    wpilib.Encoder(1, 1, 1, 2, false, wpilib.CounterBase_k1X),
}
for _, e in ipairs(encoders) do
    e:Start()
end

switches = {
    wpilib.DigitalInput(1, 12),
    wpilib.DigitalInput(1, 9),
    wpilib.DigitalInput(1, 6),
    wpilib.DigitalInput(1, 3),
}
-- End Inputs/Outputs

-- Controls

controlMap =
{
    -- Joystick 1
    {
        ["x"] = function(axis) x = axis end,
        [1] = function()
            motorNum = motorNum - 1
            if motorNum < 1 then
                motorNum = #motors
            end
        end,
        [2] = function()
            motorNum = motorNum + 1
            if motorNum > #motors then
                motorNum = 1
            end
        end,
        [3] = function()
            encoderNum = encoderNum - 1
            if encoderNum < 1 then
                encoderNum = #encoders
            end
        end,
        [4] = function()
            encoderNum = encoderNum + 1
            if encoderNum > #encoders then
                encoderNum = 1
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
