-- robot.lua

local controls = require "controls"
local lcd = require "lcd"
local math = require "math"
local pid = require "pid"
local wpilib = require "wpilib"

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005

-- Watchdog shortcuts
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog
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

-- Robot running
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

-- Inputs/Outputs
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)
-- End Inputs/Outputs

function hellautonomous()
    disableWatchdog()
    -- If you need it. :)
end

function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()

        lcd.print(1, "Running!")

        -- Read controls
        controls.update(controls.defaultControls)
        feedWatchdog()

        -- Pneumatics
        if pressureSwitch:Get() then
            compressor:Set(wpilib.Relay_kOff)
        else
            compressor:Set(wpilib.Relay_kOn)
        end

        -- Drive
        local wheels = calculateDrive(0, 0, 0, 0, 32, 22)
        -- TODO
        
        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

--[[
    calculateDrive computes the wheel angles and speeds.

    x and y are the strafe inputs from the operator, w is the rotation speed
    about the z-axis.

    gyroDeg is the field-centric shift (in degrees).

    wheelBase and trackWidth are in [INSERT UNITS HERE].

    (Consult Adam for the math.)
--]]
function calculateDrive(x, y, w, gyroDeg, wheelBase, trackWidth)
    local r = math.sqrt(wheelBase ^ 2 + trackWidth ^ 2)
    local gyroRad = math.rad(gyroDeg)

    x, y = y * math.cos(gyroRad) + x * math.sin(gyroRad), -y * math.sin(gyroRad) + x * math.cos(gyroRad)
    local a = x - w * (wheelBase / r)
    local b = x + w * (wheelBase / r)
    local c = y - w * (trackWidth / r)
    local d = y + w * (trackWidth / r)

    local wheels = {frontRight={}, frontLeft={}, rearRight={}, rearLeft={}}
    wheels.frontRight.speed = math.sqrt(b * b + c * c)
    wheels.frontLeft.speed = math.sqrt(b * b + d * d)
    wheels.rearLeft.speed = math.sqrt(a * a + d * d)
    wheels.rearRight.speed = math.sqrt(a * a + c * c)
    wheels.frontRight.angleDeg = math.deg(math.atan2(b, c))
    wheels.frontLeft.angleDeg = math.deg(math.atan2(b, d))
    wheels.rearRight.angleDeg = math.deg(math.atan2(a, d))
    wheels.rearLeft.angleDeg = math.deg(math.atan2(a, c))

    -- Normalize wheel speeds
    local maxSpeed = math.max(
        wheels.frontRight.speed,
        wheels.frontLeft.speed,
        wheels.rearRight.speed,
        wheels.rearLeft.speed
    )
    if maxSpeed > 1 then
        for _, wheel in pairs(wheels) do
            wheel.speed = wheel.speed / maxSpeed
        end
    end

    return wheels
end

function angleError(current, target)
    local delta = target - current
    if delta > 180 then
        return delta - 360
    elseif delta < 180 then
        return 360 + delta
    else
        return delta
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
