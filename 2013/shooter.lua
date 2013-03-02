-- shooter.lua

local ipairs = ipairs

local bit = require("bit")
local coroutine = require("coroutine")
local intake = require("intake")
local linearize = require("linearize")
local pid = require("pid")
local reversemotor = require("reversemotor")
local math = require("math")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local conveyerSpeed = 0
local rollerSpeed = 0
local flywheelSpeed = 0

local conveyer = reversemotor.wrap(wpilib.Victor(6))
local roller = wpilib.Talon(3)
local flywheelMotor = wpilib.Talon(2)
local flywheelCounter1 = wpilib.Counter(wpilib.DigitalInput(8))
local flywheelCounter2 = wpilib.Counter(wpilib.DigitalInput(7))
local flywheelTicksPerRevolution = 1.0
local conveyerEncoder = wpilib.Encoder(6, 5)
local hardStop = wpilib.Solenoid(6)
local humanLoadFlap = wpilib.Solenoid(5)
local flapActivated = false
local hardStopActivated = true
local targetFlywheelRPM = 6000

local rollerFeedSpeed = 1
local rollerLoadSpeed = 0.2
local conveyerLoadSpeed = 1.0

local CONVEYER_SPEED = 0

local fireCoroutine = nil

flywheelCounter1:Start()
flywheelCounter2:Start()
conveyerEncoder:Start()

local feeding = false
local flywheelRunning = false

function setFlywheelRunning(bool)
    flywheelRunning = bool
end

function feed(bool)
    feeding = bool
end

function humanLoad(bool)
    loading = bool
end

function setConveyerManual(speed)
    conveyerSpeed = speed
end

function setRollerManual(speed)
    rollerSpeed = speed
end

local function RPMcontrol(rpm)
    local dangerRPM = 8000

    if rpm > dangerRPM then
        return 0
    elseif rpm < targetFlywheelRPM then
        return 1
    else
        return 0.6
    end
end

-- Return RPM of a flywheel counter.
local function computeFlywheelSpeed(counter)
    return 60.0 / (counter:GetPeriod() * flywheelTicksPerRevolution)
end

local function getFlywheelSpeed()
    return computeFlywheelSpeed(flywheelCounter1)
end

function setFlapActive(bool)
    flapActivated = bool
end

function setHardStopActive(bool)
    hardStopActivated = bool
end

local function getConveyerDistance()
    local diameter = 1.5
    local encoderTicks = 360
    local distancePerRevolution = math.pi * diameter
    return (conveyerEncoder:Get() / encoderTicks) * distancePerRevolution
end

local function performFire()
    local conveyerWait = 0.5 -- in seconds
    local conveyerStallSpeed = 120 -- in inches/second
    local rpmDropThreshold = 4500

    while true do
        local t = wpilib.Timer()
        t:Start()
        local now = t:Get()
        local conveyerDist = getConveyerDistance()
        local lastTime = now
        local lastConveyer = conveyerDist
        repeat
            CONVEYER_SPEED = (conveyerDist - lastConveyer) / (now - lastTime)
            conveyer:Set(conveyerLoadSpeed)
            roller:Set(0)

            lastTime = now
            lastConveyer = getConveyerDistance()
            coroutine.yield()
            now = t:Get()
            conveyerDist = getConveyerDistance()
        until now >= conveyerWait and (conveyerDist - lastConveyer) / (now - lastTime) < conveyerStallSpeed

        while getFlywheelSpeed() < targetFlywheelRPM do
            conveyer:Set(0)
            roller:Set(0)
            coroutine.yield()
        end

        while getFlywheelSpeed() >= rpmDropThreshold do
            conveyer:Set(0)
            roller:Set(rollerFeedSpeed)
            coroutine.yield()
        end
    end
end

function fire(firing)
    if firing == nil then
        firing = true
    end

    if firing and fireCoroutine == nil then
        fireCoroutine = coroutine.create(performFire)
    elseif not firing then
        fireCoroutine = nil
    end
end

function update()
    humanLoadFlap:Set(flapActivated)
    hardStop:Set(hardStopActivated)

    if flywheelRunning then
        flywheelMotor:Set(-RPMcontrol(getFlywheelSpeed()))
    else
        flywheelMotor:Set(0.0)
    end

    if fireCoroutine then
        coroutine.resume(fireCoroutine)
        if coroutine.status(fireCoroutine) == "dead" then
            fireCoroutine = nil
        end
    elseif conveyerSpeed == 0 and rollerSpeed == 0 then
        if feeding then
            roller:Set(rollerFeedSpeed)
        elseif loading then
            conveyer:Set(conveyerLoadSpeed)
            roller:Set(rollerLoadSpeed)
            -- Locked out so we can't run it during human loading
            flywheelMotor:Set(0.0)
        else
            conveyer:Set(0)
            roller:Set(0)
        end
    else
        conveyer:Set(conveyerSpeed)
        roller:Set(rollerSpeed)
    end
end

function fullStop()
    flywheelMotor:Set(0.0)
end

function dashboardUpdate()
    local flywheelSpeed = getFlywheelSpeed()
    wpilib.SmartDashboard_PutNumber("RPM Bang-Bang control", RPMcontrol(flywheelSpeed))
    wpilib.SmartDashboard_PutNumber("Flywheel RPM", flywheelSpeed)
    wpilib.SmartDashboard_PutNumber("RAW BANNER", flywheelCounter1:Get())
    wpilib.SmartDashboard_PutNumber("Conveyer Distance", getConveyerDistance())
    wpilib.SmartDashboard_PutNumber("Conveyer Speed", CONVEYER_SPEED)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
