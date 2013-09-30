-- shooter.lua

local error = error
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

local flywheelSpeed = 0

local flywheelMotor = wpilib.Talon(3)
local flywheelCounter1 = wpilib.Counter(wpilib.DigitalInput(1))
local flywheelTicksPerRevolution = 1.0
local targetFlywheelRPM = 5500
local flywheelFullSpeed = false
local discsFired = 0
local RPMpower = 0.4

local indexer = wpilib.Solenoid(1)
local shotAngler = wpilib.Solenoid(4)
local roller = wpilib.Talon(6)

local fireCoroutine = nil

flywheelCounter1:Start()

local flywheelRunning = false

PRESETS = {
    Pyramid = {power = 0.6,
    RPM = 5500, angle = true, running = false,},

    fullCourt = {power = 0.7,
    RPM = 5500, angle = false, running = true,},

    midGoal = {power = 0.5,
    RPM = 5500, angle = false, running = false,},
}

function setPreset(name)
    local p = PRESETS[name]
    if p then
        RPMpower = p.power
        targetFlywheelRPM = p.RPM
        shotAngler:Set(p.angle)
        rollerRunning = p.running
    end
end

function setFlywheelRunning(bool)
    flywheelRunning = bool
end

function getTargetRPM()
    return targetFlywheelRPM
end

function setTargetRPM(target)
    targetFlywheelRPM = target
end

local function RPMcontrol(rpm, minPower)
    local dangerRPM = 10000

    if rpm > dangerRPM then
        return 0
    elseif rpm < targetFlywheelRPM then
        return 1
    else
        return minPower
    end
end

-- Return RPM of a flywheel counter.
local function computeFlywheelSpeed(counter)
    return 60.0 / (counter:GetPeriod() * flywheelTicksPerRevolution)
end

local function getFlywheelSpeed()
    return computeFlywheelSpeed(flywheelCounter1)
end

function setIndexer(bool)
    indexer:Set(bool)
end

function setRollerRunning(bool)
    rollerRunning = bool
end

function clearDiscsFired()
    discsFired = 0
end

local function performFire()
    local rpmDropThreshold = targetFlywheelRPM - 300

    clearDiscsFired()
    indexer:Set(false)
    while true do
        while getFlywheelSpeed() < targetFlywheelRPM do
            indexer:Set(false)
            coroutine.yield()
        end

        while getFlywheelSpeed() > rpmDropThreshold do
            indexer:Set(true)
            coroutine.yield()
        end

        indexer:Set(false)

        discsFired = discsFired + 1
    end
end

local function performFireOne()
    local rpmDropThreshold = 5500
    indexer:Set(false)

    while getFlywheelSpeed() < targetFlywheelRPM do
        coroutine.yield()
    end

    while getFlywheelSpeed() >= rpmDropThreshold do
        indexer:Set(true)
        coroutine.yield()
    end
    indexer:Set(false)
    discsFired = discsFired + 1

    -- Extra Safe Stop
    indexer:Set(false)
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

function fireOne(firing)
    if firing == nil then
        firing = true
    end

    if firing and fireCoroutine == nil then
        fireCoroutine = coroutine.create(performFireOne)
    elseif not firing then
        fireCoroutine = nil
    end
end

function update()
    if flywheelRunning then
        flywheelMotor:Set(RPMcontrol(getFlywheelSpeed(), RPMpower))
    else
        flywheelMotor:Set(0.0)
        indexer:Set(false)
    end

    if fireCoroutine then
        if coroutine.status(fireCoroutine) == "dead" then
            indexer:Set(false)
        else
            local success, err = coroutine.resume(fireCoroutine)
            if not success then
                error(err)
            end
        end
    end

    if indexer:Get() then
        roller:Set(.2)
    else
        if rollerRunning then
            roller:Set(.8)
        else
            roller:Set(0)
        end
    end

end

function getDiscsFired()
    return discsFired
end


function fullStop()
    flywheelRunning = false
    indexer:Set(false)
    fireCoroutine = nil
end

function dashboardUpdate()
    local flywheelSpeed = getFlywheelSpeed()
    wpilib.SmartDashboard_PutNumber("RPM Bang-Bang control", RPMcontrol(flywheelSpeed, RPMpower))
    wpilib.SmartDashboard_PutNumber("Flywheel RPM", flywheelSpeed)
    wpilib.SmartDashboard_PutNumber("Target Flywheel RPM", targetFlywheelRPM)
    wpilib.SmartDashboard_PutNumber("RAW BANNER", flywheelCounter1:Get())
    wpilib.SmartDashboard_PutNumber("Discs Fired", discsFired)
    wpilib.DriverStationLCD_GetInstance():PrintLine(wpilib.DriverStationLCD_kUser_Line1, string.format("Flywheel RPM: %.2f", flywheelSpeed))
    wpilib.DriverStationLCD_GetInstance():UpdateLCD()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
