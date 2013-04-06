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
local hardStopActivated = false
local targetFlywheelRPM = 6000
local flywheelFullSpeed = false
local discsFired = 0

local rollerFeedSpeed = 1
local rollerLoadSpeed = 0.5
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
    local rpmDropThreshold = 5500

    while true do
        while getFlywheelSpeed() < targetFlywheelRPM do
            conveyer:Set(0)
            roller:Set(0)
            coroutine.yield()
        end

        while getFlywheelSpeed() >= rpmDropThreshold do
            conveyer:Set(conveyerLoadSpeed)
            roller:Set(rollerFeedSpeed)
            calcDiscsFired()
            coroutine.yield()
        end
    end
end

local function performFireOne()
    local rpmDropThreshold = 5500

    while getDiscsFired ~= 1 do
        while getFlywheelSpeed() < targetFlywheelRPM do
            conveyer:Set(0)
            roller:Set(0)
            coroutine.yield()
        end

        while getFlywheelSpeed() >= rpmDropThreshold do
            conveyer:Set(conveyerLoadSpeed)
            roller:Set(rollerFeedSpeed)
            calcDiscsFired()
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

function fireOne(firingOne)
    if firingOne == nil then
        firingOne = true
    end

    if firingOne and fireOneCoroutine == nil then
        fireOneCoroutine = coroutine.create(performFireOne)
    elseif not firingOne then
        fireOneCoroutine = nil
    end
end

function calcDiscsFired()
    local firedSpeedDrop = 5500
    if flywheelFullSpeed and getFlywheelSpeed() < firedSpeedDrop and getFlywheelSpeed > 5100 then
        discsFired = discsFired + 1
    end
    if getFlywheelSpeed() < firedSpeedDrop then
        flywheelFullSpeed = true
    end
end

function getDiscsFired()
    return discsFired
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
    elseif fireOneCoroutine then
        coroutine.resume(fireOneCoroutine)
        if coroutine.status(fireOneCoroutine) == "dead" then
            fireOneCoroutine = nil
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
    flywheelRunning = false
    fireCoroutine = nil
    conveyerSpeed = 0
    rollerSpeed = 0
    feeding = false
    loading = false
    hardStopActivated = false
    flapActivated = false
end

function dashboardUpdate()
    local flywheelSpeed = getFlywheelSpeed()
    wpilib.SmartDashboard_PutNumber("RPM Bang-Bang control", RPMcontrol(flywheelSpeed))
    wpilib.SmartDashboard_PutNumber("Flywheel RPM", flywheelSpeed)
    wpilib.SmartDashboard_PutNumber("RAW BANNER", flywheelCounter1:Get())
    wpilib.SmartDashboard_PutNumber("Conveyer Distance", getConveyerDistance())
    wpilib.SmartDashboard_PutNumber("Discs Fired", getDiscsFired())
    wpilib.DriverStationLCD_GetInstance():PrintLine(wpilib.DriverStationLCD_kUser_Line1, string.format("Flywheel RPM: %.2f", flywheelSpeed))
    wpilib.DriverStationLCD_GetInstance():UpdateLCD()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
