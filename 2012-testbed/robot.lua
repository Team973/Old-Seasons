-- robot.lua

local controls = require("controls")
local io = require("io")
local string = require("string")
local wpilib = require("wpilib")

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Declarations
local watchdogEnabled = false
local teleop
local feedWatchdog, enableWatchdog, disableWatchdog

local encoders
local collectingData = false
local encoderTimer
local encoderData

local controlMap
-- End Declarations

function run()
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
dashboard:PutBoolean("Collecting", collectingData)

function teleop()
    disableWatchdog()

    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        controls.update(controlMap)

        if collectingData then
            if not encoderTimer then
                encoderTimer = wpilib.Timer()
                encoderTimer:Start()
                encoderData = {}
            end

            local datum = {time=encoderTimer:Get()}
            for k, enc in pairs(encoders) do
                datum[k] = enc:Get()
            end
            encoderData[#encoderData + 1] = datum
        elseif encoderTimer ~= nil then
            -- Open CSV file
            local f = io.open("encoder-data.csv", "w")

            -- Write header row
            f:write("Time")
            local cols = {"left", "right", "cross", "gyro", "driveLeft", "driveRight"}
            for _, col in ipairs(cols) do
                f:write(","..col)
            end
            f:write("\r\n")

            -- Write data
            for _, datum in ipairs(encoderData) do
                f:write(string.format("%.3f", datum.time))
                for _, col in ipairs(cols) do
                    f:write(string.format(",%d", datum[col]))
                end
                f:write("\r\n")
            end

            -- Clean up
            f:close()
            encoderData = nil
            encoderTimer = nil
        end

        wpilib.Wait(TELEOP_LOOP_LAG)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!

-- HERE BE ENCODERS
-- Parameters: sidecarA, channelA, sidecarB, channelB, flip, resolution
encoders = {
    left=wpilib.Encoder(1, 1, 1, 2, false, wpilib.CounterBase_k4X),
    right=wpilib.Encoder(1, 3, 1, 4, false, wpilib.CounterBase_k4X),
    cross=wpilib.Encoder(1, 5, 1, 6, false, wpilib.CounterBase_k4X),
    gyro=wpilib.Encoder(1, 7, 1, 8, false, wpilib.CounterBase_k4X),
    driveLeft=wpilib.Encoder(1, 9, 1, 10, false, wpilib.CounterBase_k1X),
    driveRight=wpilib.Encoder(1, 11, 1, 12, false, wpilib.CounterBase_k1X),
}
for _, e in pairs(encoders) do
    e:Start()
end

-- End Inputs/Outputs

-- Controls

controlMap =
{
    -- Joystick 1
    {
        [1] = function()
            collectingData = not collectingData
            dashboard:PutBoolean("Collecting", collectingData)
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
