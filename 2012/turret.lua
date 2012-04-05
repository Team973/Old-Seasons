-- turret.lua

local ipairs = ipairs

local bit = require("bit")
local pid = require("pid")
local linearize = require("linearize")
local math = require("math")
local matrix = require("matrix")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local flywheelSpeedTable = {
    numSamples=100,
}
local flywheelFired = false

local dashboard = wpilib.SmartDashboard_GetInstance()

local flywheelTargetSpeed = 0.0
local flywheelFeedforward = math.huge
local in4 = wpilib.DigitalInput(2, 4)
local in5 = wpilib.DigitalInput(2, 5)
local in6 = wpilib.DigitalInput(2, 6)
local flywheelCounter = wpilib.Counter(wpilib.CounterBase_k2X, in5, in5, false)
local flywheelMotor = linearize.wrap(wpilib.Victor(2, 6))
local flywheelTicksPerRevolution = 6.0

flywheelCounter:Start()

local hoodEncoder1, hoodEncoder2
local hoodMotor1= wpilib.Victor(2,7) 
local hoodMotor2= wpilib.Victor(2,8) 
hoodPID1 = pid.new(0.01, 0, 0)
hoodPID1:start()
hoodPID2 = pid.new(0.01, 0, 0)
hoodPID2:start()
runHood = 0

local numStates, numOutputs = 2, 1
-- State Space equations
-- x(n + 1) = A x(n) + B u(n)
-- Y(n) = C x(n) + D u(n)
local X = matrix(numStates, 1)
local X_hat = matrix(numStates, 1)
local Y = matrix(numOutputs, 1)
local R = matrix(numStates, 1)
local U = matrix(numOutputs, 1) -- control input
local U_ff = matrix(numOutputs, 1)

-- Number of states 2 Number of outputs 1
local A = matrix{{1.0000000000, 0.0247014661}, {0.0000000000, 0.9762127298}}
local B = matrix{{0.0180233658}, {1.4361069986}}
local C = matrix{{1.0000000000, 0.0000000000}}
local D = matrix{{0.0000000000}}
local L = matrix{{0.6362127298}, {3.9943473648}}
local K = matrix{{0.2088980837, 0.1757861242}}
local U_max = matrix{{12.0000000000}}
local U_min = matrix{{-1.0000000000}}

local positionGoal -- double

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
    dashboard:PutBoolean("VEX Encoder Check " .. num, encoder:VerifySensor(0x08, 3, vexMagic))
    wpilib.Wait(I2C_DELAY)

    -- Terminate (or not)
    if terminated then
        encoder:Write(0x4c, 0xff)
    else
        encoder:Write(0x4b, 0xff)
    end
    wpilib.Wait(I2C_DELAY)

    return encoder
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

    hoodEncoder1 = initVexEncoder(0x20, "1", false)
    hoodEncoder2 = initVexEncoder(0x22, "2", true)
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
    return X_hat[2][1] / (2*math.pi) * 60
end

function clearFlywheelFired()
    flywheelFired = false
end

function getFlywheelFired()
    return flywheelFired
end

-- Retrieve the target flywheel speed
function getFlywheelTargetSpeed(speed)
    return flywheelTargetSpeed
end

-- Change the target flywheel speed
function setFlywheelTargetSpeed(speed)
    flywheelTargetSpeed = speed
end

local flywheelTimeStep = 0.025
local flywheelTimer = wpilib.Timer()
local prevFlywheelPos = 0
local prevFlywheelTime
local nextFlywheelTime
function update()
    flywheelTimer:Start()

    -- Turret rotation
    dashboard:PutBoolean("Input 4", in4:Get())
    dashboard:PutBoolean("Input 5", in5:Get())
    dashboard:PutBoolean("Input 6", in6:Get())
    dashboard:PutInt("TURN.TARGET", turnPID.target)
    dashboard:PutInt("TURN.ANGLE", encoder:Get()/25)
    turnPID:update(encoder:Get()/25)
    motor:Set(turnPID.output)

    if nextFlywheelTime == nil then
        prevFlywheelTime = wpilib.Timer_GetFPGATimestamp()
        nextFlywheelTime = prevFlywheelTime + flywheelTimeStep
    end
    local flywheelTime = wpilib.Timer_GetFPGATimestamp()
    if flywheelTime > nextFlywheelTime then
        -- <AUSTIN MAGIC>
        local flywheelPos = flywheelCounter:Get()/2 / flywheelTicksPerRevolution * 2*math.pi
        if positionGoal == nil then
            positionGoal = flywheelPos
        end
        local velocityGoal = flywheelTargetSpeed / 60 * 2*math.pi
        Y[1][1] = flywheelPos
        local velocityWeightScalar = 0.35
        local maxReference = (U_max[1][1] - velocityWeightScalar * (velocityGoal - X_hat[2][1]) * K[1][2]) / K[1][1] + X_hat[1][1]
        local minReference = (U_min[1][1] - velocityWeightScalar * (velocityGoal - X_hat[2][1]) * K[1][2]) / K[1][1] + X_hat[1][1]
        positionGoal = math.max(math.min(positionGoal, maxReference), minReference)
        R = matrix{{positionGoal}, {velocityGoal}}
        positionGoal = positionGoal + velocityGoal * flywheelTimeStep
        -- Update()
        do
            local updateObserver = true
            U = K * (R - X_hat)
            for i = 1, numOutputs do
                if U[i][1] > U_max[i][1] then
                    U[i][1] = U_max[i][1]
                elseif U[i][1] < U_min[i][1] then
                    U[i][1] = U_min[i][1]
                end
            end
            if updateObserver then
                X_hat = (A - L * C) * X_hat + L * Y + B * U
            else
                X_hat = A * X_hat + B * U
            end
        end
        -- /Update()
        local out = 0.0
        if velocityGoal < 1.0 then
            flywheelMotor:Set(0.0)
            -- Reset the positional error
            positionGoal = flywheelPos
        else
            flywheelMotor:Set(U[1][1] / 12.0)
            out = U[1][1] / 12.0
        end
        dashboard:PutDouble("Fly Out", out)
        dashboard:PutDouble("Fly Pos", flywheelPos)
        dashboard:PutDouble("Fly xhat[0]", X_hat[1][1])
        dashboard:PutDouble("Fly xhat[1]", X_hat[2][1])
        dashboard:PutDouble("Fly R[0]", X_hat[1][1])
        dashboard:PutDouble("Fly R[1]", X_hat[2][1])
        dashboard:PutDouble("Fly Dx/Dt", (flywheelPos - prevFlywheelPos) / flywheelTimeStep)
        dashboard:PutDouble("Fly Dt", math.min(flywheelTime - prevFlywheelTime, flywheelTimeStep * 1.5))

        if flywheelMotor:Get() > 0 then
            flywheelMotor:Set(flywheelMotor:Get() + 0.01)
        elseif flywheelMotor:Get() < 0 then
            flywheelMotor:Set(flywheelMotor:Get() - 0.01)
        end

        prevFlywheelPos = flywheelPos
        prevFlywheelTime = flywheelTime
        local nextFlyTimeBefore = nextFlywheelTime
        nextFlywheelTime = nextFlywheelTime + flywheelTimeStep
        dashboard:PutDouble("Fly ideal Dt", math.min(nextFlywheelTime - nextFlyTimeBefore, flywheelTimeStep * 1.5))
        -- </AUSTIN MAGIC>
    end

    -- Add flywheel velocity sample
    local speedSample = X_hat[2][1] / (2*math.pi) * 60
    flywheelSpeedTable:add(speedSample)

    if flywheelTargetSpeed - speedSample > 200 then
        flywheelFired = true
    end
    dashboard:PutBoolean("Flywheel Fired", flywheelFired)

    local e1 = -readVexEncoder(hoodEncoder1)
    local e2 = readVexEncoder(hoodEncoder2)
    dashboard:PutDouble("Hood 1", e1)
    dashboard:PutDouble("Hood 2", e2)
    hoodPID1:update(e1)
    hoodPID2:update(e2)
    hoodMotor1:Set(-hoodPID1.output)
    hoodMotor2:Set(hoodPID2.output)
end

function calculateTarget(turretAngle, desiredAngle)
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

function fullStop()
    motor:Set(0.0)
    flywheelMotor:Set(0.0)
    hoodMotor1:Set(0.0)
    hoodMotor2:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
