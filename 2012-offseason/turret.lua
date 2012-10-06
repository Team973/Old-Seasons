-- turret.lua

local ipairs = ipairs

local bit = require("bit")
local intake = require("intake")
local pid = require("pid")
local linearize = require("linearize")
local math = require("math")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local flywheelSpeedTable = {
    numSamples=25,
}
local flywheelSpeedFilter = {
    prev=0,
    curr=0,
    weight=0.2,
}
local flywheelFired = false
flywheelPID = pid.new(0.05, 0.0, -0.0007,
    nil,
    function()
        return flywheelSpeedFilter:average()
    end)
local flywheelPIDGains = {
    {0, p=0.05, d=-0.0007},
    {3200, p=0.05, d=-0.0007},
    {4500, p=0.05, d=-0.0007},
    {6000, p=0.01, d=-0.0007},
    {7600, p=0.0095, d = -.0007}
}
local TURRET_ANGLE_OFFSET = 0

local currPresetName = nil
PRESETS = {
    cornerFender={flywheelRPM=3150, hoodAngle=0, targetAngle=-84},
    sideFender={flywheelRPM=3500, hoodAngle=0, targetAngle=-22},
    key={flywheelRPM=6700, hoodAngle=900, superSoftHoodAngle=750, targetAngle=0, hardFlywheelRPM=6000, superHardFlywheelRPM=5800},
    autoKey={flywheelRPM=6200, hoodAngle=1100, targetAngle=-TURRET_ANGLE_OFFSET},
    bridge={flywheelRPM=7000},
}

local dashboard = wpilib.SmartDashboard_GetInstance()

local flywheelTargetSpeed = PRESETS.key.flywheelRPM
local flywheelOn = false
local flywheelFeedforward = math.huge
local in4 = wpilib.DigitalInput(2, 4)
local in5 = wpilib.DigitalInput(2, 5)
local in6 = wpilib.DigitalInput(2, 6)
local flywheelCounter = wpilib.Counter(in4)
local flywheelMotor = linearize.wrap(wpilib.Victor(2, 6))
local flywheelTicksPerRevolution = 6.0
local turretEnabled = true

flywheelCounter:Start()

local hoodEncoder1, hoodEncoder2
local HOOD_ENCODER_RATIO = 627.2 / 392.0
local hoodMotor1= wpilib.Victor(2,7)
local hoodMotor2= wpilib.Victor(2,8)
hoodPID1 = pid.new(0.006, 0, 0)
hoodPID1:start()
hoodPID2 = pid.new(0.006, 0, 0)
hoodPID2:start()
runHood = 0

encoder = wpilib.Encoder(2, 2, 2, 3, true, wpilib.CounterBase_k1X)
encoder:Start()
motor = wpilib.Jaguar(2, 3)
turnPID = pid.new(0.05, 0, 0)

local HARD_LIMIT = 90

function initI2C()
    dashboard:PutString("mode", "I2C Init")

    -- Reset encoder addresses
    local i2c = wpilib.DigitalModule_GetInstance(I2C_SIDECAR):GetI2C(0x00)
    i2c:SetCompatibilityMode(true)
    -- ORDER IS SIGNIFICANT HERE
    i2c:Write(0x4f, 0x03)
    wpilib.Wait(I2C_DELAY)
    i2c:Write(0x4e, 0xca)
    wpilib.Wait(I2C_DELAY)

    local hood1Okay
    local hood2Okay
    hoodEncoder1, hood1Okay = initVexEncoder(0x20, "1", false)
    hoodEncoder2, hood2Okay = initVexEncoder(0x22, "2", true)
    hoodOkay = hood1Okay and hood2Okay
end

-- Calculate the current flywheel speed (in RPM).
function getFlywheelSpeed()
    return flywheelSpeedTable:average()
end

function flywheelSpeedTable:add(val)
    if #self < self.numSamples then
        self[#self + 1] = val
    else
        for i = 1, #self - 1 do
            self[i] = self[i + 1]
        end
        self[#self] = val
    end
end

function flywheelSpeedTable:average()
    local sum = 0.0
    for _, val in ipairs(self) do
        sum = sum + val
    end
    return sum / #self
end

function getFlywheelFilterSpeed()
    return flywheelSpeedFilter:average()
end

function flywheelSpeedFilter:add(val)
    self.prev, self.curr = self:average(), val
end

function flywheelSpeedFilter:average()
    return (1 - self.weight) * self.prev + self.weight * self.curr
end

function clearFlywheelFired()
    flywheelFired = false
end

function getFlywheelFired()
    return flywheelFired
end

local function tableStep(t, x)
    if x < t[1][1] then
        return t[1]
    end
    for i = 2, #t do
        if x >= t[i - 1][1] and x < t[i][1] then
            return t[i - 1]
        end
    end
    return t[#t]
end

-- Retrieve the target flywheel speed
function getFlywheelTargetSpeed(speed)
    return flywheelTargetSpeed
end

-- Change the target flywheel speed
function setFlywheelTargetSpeed(speed)
    flywheelTargetSpeed = speed
    local gain = tableStep(flywheelPIDGains, speed)
    if gain.p then flywheelPID.p = gain.p else flywheelPID.p = 0 end
    if gain.i then flywheelPID.i = gain.i else flywheelPID.i = 0 end
    if gain.d then flywheelPID.d = gain.d else flywheelPID.d = 0 end
end

function resetFlywheel()
    flywheelCounter:Reset()
    flywheelPID.target = 0
end

function runFlywheel(on, speed)
    if not flywheelOn and on then
        resetFlywheel()
    end
    flywheelOn = on
    if speed then
        setFlywheelTargetSpeed(speed)
    end
end

function update()
    -- Turret rotation
    dashboard:PutBoolean("Input 4", in4:Get())
    dashboard:PutBoolean("Input 5", in5:Get())
    dashboard:PutBoolean("Input 6", in6:Get())
    dashboard:PutInt("TURN.TARGET", turnPID.target)
    dashboard:PutInt("TURN.ANGLE", encoder:Get()/25 + TURRET_ANGLE_OFFSET)
    turnPID:update(encoder:Get()/25 + TURRET_ANGLE_OFFSET)
    if turretEnabled then
        motor:Set(turnPID.output)
    else
        motor:Set(0)
    end

    -- Update flywheel target speed from intake's squish meter
    local softnessValue = intake.getLastBallSoftness()
    if currPresetName and softnessValue ~= nil then
        local p = PRESETS[currPresetName]
        if softnessValue == 1 and p.hardFlywheelRPM then
            setFlywheelTargetSpeed(p.hardFlywheelRPM)
        elseif softnessValue == -1 and p.superSoftFlywheelRPM then
            setFlywheelTargetSpeed(p.superSoftFlywheelRPM)
        elseif softnessValue == 2 and p.superHardFlywheelRPM then
            setFlywheelTargetSpeed(p.superHardFlywheelRPM)
        else
            setFlywheelTargetSpeed(p.flywheelRPM)
        end
        if softnessValue == 1 and p.hardHoodAngle then
            setHoodTarget(p.hardHoodAngle)
        elseif softnessValue == -1 and p.superSoftHoodAngle then
            setHoodTarget(p.superSoftHoodAngle)
        elseif softnessValue == 2 and p.superHardHoodAngle then
            setHoodTarget(p.superHardHoodAngle)
        else
            setHoodTarget(p.hoodAngle)
        end
    end

    -- Add flywheel velocity sample
    local speedSample = 60.0 / (flywheelCounter:GetPeriod() * flywheelTicksPerRevolution)
    flywheelSpeedTable:add(speedSample)
    flywheelSpeedFilter:add(speedSample)

    if flywheelSpeedTable:average() - flywheelSpeedFilter:average() > 300 then
        flywheelFired = true
    end
    dashboard:PutBoolean("Flywheel Fired", flywheelFired)

    -- Get flywheel position and time
    flywheelPID.timer:Start()
    local pos = flywheelCounter:Get() / flywheelTicksPerRevolution -- in revolutions
    local dt = flywheelPID.timer:Get() / 60.0 -- in minutes
    flywheelPID.timer:Reset()

    -- Update flywheel PID
    do
        local speed = flywheelTargetSpeed
        if not flywheelOn then
            speed = 0
        end

        flywheelPID.target = flywheelPID.target + speed * dt
        local extraTerm = speed * (1/flywheelFeedforward - flywheelPID.d)
        flywheelPID.target = math.min(pos - (1 - flywheelPID.d - extraTerm) / flywheelPID.p, flywheelPID.target)
        local flywheelOutput = flywheelPID:update(pos, dt) + extraTerm
        if flywheelOutput > 0.0 then
            flywheelMotor:Set(flywheelOutput)
        else
            flywheelMotor:Set(0.0)
        end
    end

    -- Print flywheel diagnostics
    dashboard:PutDouble("Flywheel P", flywheelPID.p)
    dashboard:PutDouble("Flywheel D", flywheelPID.d)
    dashboard:PutDouble("Flywheel Speed", getFlywheelSpeed())
    dashboard:PutInt("Flywheel Speed(Int)", getFlywheelSpeed())
    dashboard:PutInt("Flywheel Speed(Filter Int)", getFlywheelFilterSpeed())
    dashboard:PutDouble("Flywheel Target Speed", getFlywheelTargetSpeed())
    dashboard:PutBoolean("Flywheel On", flywheelOn)
    if currPresetName then
        dashboard:PutString("Turret Preset", currPresetName)
    else
        dashboard:PutString("Turret Preset", "<MANUAL>")
    end

    -- Update hood
    local e1 = readVexEncoder(hoodEncoder1) * HOOD_ENCODER_RATIO
    local e2 = -readVexEncoder(hoodEncoder2) * HOOD_ENCODER_RATIO
    dashboard:PutDouble("Hood 1", e1)
    dashboard:PutDouble("Hood 2", e2)
    hoodPID1:update(e1)
    hoodPID2:update(e2)
    if hoodOkay then
        hoodMotor1:Set(hoodPID1.output)
        hoodMotor2:Set(-hoodPID2.output)
    else
        hoodMotor1:Set(0)
        hoodMotor2:Set(0)
    end
end

function fullStop()
    motor:Set(0.0)
    flywheelMotor:Set(0.0)
    hoodMotor1:Set(0.0)
    hoodMotor2:Set(0.0)
end

function setPreset(name)
    currPresetName = name
    local p = PRESETS[name]
    if not p then return end

    if p.flywheelRPM then
        setFlywheelTargetSpeed(p.flywheelRPM)
    end
    if p.hoodAngle then
        setHoodTarget(p.hoodAngle)
    end
    if p.targetAngle then
        setTargetAngle(p.targetAngle)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
