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
    {6300, p=0.05, d=-0.0007},
}

local currPresetName = nil
PRESETS = {
    fender={flywheelRPM=3200, hoodAngle=20, targetAngle=0, softFlywheelRPM=3200},
    side={flywheelRPM=4500, hoodAngle=200, targetAngle=0},
    key={flywheelRPM=6300, hoodAngle=1100, targetAngle=0},
    autoKey={flywheelRPM=6200, hoodAngle=1100},
    bridge={flywheelRPM=7000},
}

local dashboard = wpilib.SmartDashboard_GetInstance()

local flywheelTargetSpeed = PRESETS.fender.flywheelRPM
local flywheelOn = false
local flywheelFeedforward = math.huge
local in4 = wpilib.DigitalInput(2, 4)
local in5 = wpilib.DigitalInput(2, 5)
local in6 = wpilib.DigitalInput(2, 6)
local flywheelCounter = wpilib.Counter(in5)
local flywheelMotor = linearize.wrap(wpilib.Victor(2, 6))
local flywheelTicksPerRevolution = 6.0

flywheelCounter:Start()

local hoodEncoder1, hoodEncoder2
local HOOD_ENCODER_RATIO = 627.2 / 392.0
local hoodMotor1= wpilib.Victor(2,7)
local hoodMotor2= wpilib.Victor(2,8)
hoodPID1 = pid.new(0.01, 0, 0)
hoodPID1:start()
hoodPID2 = pid.new(0.01, 0, 0)
hoodPID2:start()
runHood = 0

function getHoodTarget()
    return hoodPID1.target
end

function setHoodTarget(target)
    hoodPID1.target = target
    hoodPID2.target = target
end

encoder = wpilib.Encoder(2, 2, 2, 3, true, wpilib.CounterBase_k1X)
encoder:Start()
motor = wpilib.Jaguar(2, 3)
turnPID = pid.new(0.05, 0, 0)

allowRotate = false

local HARD_LIMIT = 90

local function readVexEncoder(encoder)
    local val = 0
    local data = wpilib.new_UINT8array(4)

    -- TODO: The error result should probably be examined.
    encoder:Read(0x40, 4, data)
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 0), 8))
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 1), 0))
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 2), 24))
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 3), 16))

    -- XXX: For a few bits more:
    --[[
    if not encoder:Read(0x46, 2, data) then
        --return nil
    end
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 0), 40))
    val = bit.bor(val, bit.lshift(wpilib.UINT8array_getitem(data, 1), 32))
    --]]

    wpilib.delete_UINT8array(data)

    return val
end

local vexMagic = wpilib.new_UINT8array(3)
wpilib.UINT8array_setitem(vexMagic, 0, string.byte("V"))
wpilib.UINT8array_setitem(vexMagic, 1, string.byte("E"))
wpilib.UINT8array_setitem(vexMagic, 2, string.byte("X"))

local I2C_DELAY = 0.1
local I2C_SIDECAR = 2
hoodOkay = nil

local function initVexEncoder(address, num, terminated)
    local mod = wpilib.DigitalModule_GetInstance(I2C_SIDECAR)
    local encoder

    local i2c = mod:GetI2C(0x60)
    i2c:SetCompatibilityMode(true)

    -- Change address
    i2c:Write(0x4d, address)
    wpilib.Wait(I2C_DELAY)

    -- Get encoder at new address
    encoder = mod:GetI2C(address)
    encoder:SetCompatibilityMode(true)

    -- Verify
    local Okay = encoder:VerifySensor(0x08, 3, vexMagic)

    dashboard:PutBoolean("VEX Encoder Check " .. num, Okay)
    wpilib.Wait(I2C_DELAY)

    -- Terminate (or not)
    if terminated then
        encoder:Write(0x4c, 0xff)
    else
        encoder:Write(0x4b, 0xff)
    end
    wpilib.Wait(I2C_DELAY)

    return encoder, Okay
end

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

local function calculateTarget(turretAngle, desiredAngle)
    --calculates shortest desired angle
    while desiredAngle - turretAngle > 180 do
        desiredAngle = desiredAngle - 360
    end
    while desiredAngle - turretAngle < -180 do
        desiredAngle = desiredAngle + 360
    end

    --make sure the turret doesn't crash
    if desiredAngle > HARD_LIMIT then
        desiredAngle = HARD_LIMIT
    end
    if desiredAngle < -HARD_LIMIT then
        desiredAngle = -HARD_LIMIT
    end

    return desiredAngle
end

function getTargetAngle()
    return turnPID.target
end

function setTargetAngle(angle)
    turnPID.target = calculateTarget(encoder:Get()/25, angle)
end

function setFromJoy(x,y)
    local THRESHOLD = 0.5
    if allowRotate then
        local angle
        if x*x + y*y > THRESHOLD*THRESHOLD then
            angle = math.atan2(x, y)
        else
            angle = 0
        end
        angle = angle*180/math.pi
        turnPID.target = calculateTarget(encoder:Get()/25, angle)
    end
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
    dashboard:PutInt("TURN.ANGLE", encoder:Get()/25)
    turnPID:update(encoder:Get()/25)
    motor:Set(turnPID.output)

    -- Update flywheel target speed from intake's squish meter
    local isSoft = intake.getLastBallSoftness()
    if currPresetName and isSoft ~= nil then
        if isSoft then
            setFlywheelTargetSpeed(PRESETS[currPresetName].softFlywheelRPM)
        else
            setFlywheelTargetSpeed(PRESETS[currPresetName].flywheelRPM)
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
