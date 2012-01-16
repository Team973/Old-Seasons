-- robot.lua

local controls = require("controls")
local lcd = require("lcd")
local linearize = require("linearize")
local pid = require ("pid")
local wpilib = require("wpilib")
local pairs = pairs

local x, flyMotor, encoder, flypid, feedforward

module(..., package.seeall)

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

    encoder:Start()

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
feedforward=100
dashboard:PutDouble("feedforward", feedforward)

function teleop()
    disableWatchdog()

    

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        lcd.print(1, "Running!")
        lcd.update()

        -- Read controls
        controls.update(controlMap)

        dashboard:PutDouble("x", x)
        dashboard:PutDouble("encoder", encoder:Get())
        dashboard:PutDouble("speed", encoder:GetRate())
        flypid.p=dashboard:GetDouble("p")
		flypid.target=dashboard: GetDouble("target")
		feedforward=dashboard:GetDouble("feedforward")
		flypid:update(encoder:GetRate())
		flyMotor:Set(flypid.target/feedforward+flypid.output)

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

flyMotor = wpilib.Victor(1, 6)
encoder = wpilib.Encoder(2, 1, 2, 2, true, wpilib.CounterBase_k1X)
encoder:SetDistancePerPulse(1.0 / 100.0)
flypid= pid.new(0)
dashboard:PutDouble("target",flypid.target)
dashboard:PutDouble("p",flypid.p)
-- End Inputs/Outputs

-- Controls

controlMap =
{
    -- Joystick 1
    {
        ["y"] = function(axis) x = -axis end,
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
