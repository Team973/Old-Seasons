-- robot.lua

local controls = require("controls")
local lcd = require("lcd")
local linearize = require("linearize")
local pid = require ("pid")
local wpilib = require("wpilib")
local pairs = pairs

local x, flyMotor, encoder
local flypid, feedforward, pDelta
local currentSpeed, prevPos
local timer= wpilib.Timer()

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

    encoder:Reset()
    timer:Start()
    timer:Reset()
	currentSpeed = 0
    prevPos = 0

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        lcd.print(1, "Running!")
        lcd.update()

        -- Read controls
        controls.update(controlMap)
		
		if timer:Get() >  .25 then
			local currentPos= encoder:Get()/100
			currentSpeed= (currentPos - prevPos)/timer:Get()*60
			prevPos= currentPos
            timer:Reset() 
		end
		
        dashboard:PutDouble("x", x)
        dashboard:PutDouble("encoder", encoder:Get())
        dashboard:PutDouble("speed", currentSpeed)
        dashboard:PutDouble("pDelta", pDelta)
        dashboard:PutDouble("p", flypid.p)
		dashboard:PutDouble("target", flypid.target)
		dashboard:PutDouble("feedforward", feedforward)
		flypid:update(currentSpeed)
		flyMotor:Set(flypid.target/feedforward+flypid.output)

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

flyMotor = LinearVictor(1, 6)
encoder = wpilib.Encoder(2, 1, 2, 2, true, wpilib.CounterBase_k1X)
encoder:SetDistancePerPulse(1.0 / 100.0)
flypid= pid.new(0)
dashboard:PutDouble("target",flypid.target)
dashboard:PutDouble("p",flypid.p)
-- End Inputs/Outputs

-- Controls

pDelta = 0.1
controlMap =
{
    -- Joystick 1
    {
        ["y"] = function(axis) x = -axis end,
        [1] = function() pDelta = pDelta / 10 end,
        [2] = function() pDelta = pDelta * 10 end,
        [3] = function() flypid.p = flypid.p - pDelta end,
        [4] = function() flypid.p = flypid.p + pDelta end,
        [5] = function() feedforward = feedforward + 100 end,
        ["ltrigger"] = function() feedforward = feedforward - 100 end,
        [6] = function() flypid.target = flypid.target + 100 end,
        ["rtrigger"] = function() flypid.target = flypid.target - 100 end,
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
