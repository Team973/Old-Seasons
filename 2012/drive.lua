-- drive.lua

local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

module(...)

local dashboard = wpilib.SmartDashboard_GetInstance()

local gyro = nil
local gyroOkay = true
local ignoreGyro = false 
local rotationPID = pid.new(0.01, 0, 0)
local rotationHoldTimer
local gearSwitch = wpilib.Solenoid(1, 1)
local frontSkid = wpilib.Solenoid(3)
local followerDeploy = wpilib.Solenoid(1, 6)
rotationPID.min, rotationPID.max = -1, 1

function initGyro()
    gyro = wpilib.Gyro(1, 1)
    gyro:SetSensitivity(0.002*2940/1800)
    gyro:Reset()
    gyroOkay = true
    dashboard:PutBoolean("Gyro Okay", true)
end

function resetGyro()
    gyro:Reset()
    if rotationPID.target then
        rotationPID.target = 0
    end
    ignoreGyro = false 
end

function effTheGyro()
    ignoreGyro = true
end

function getGyroAngle()
    if not gyroOkay or ignoreGyro then
        return 0
    end
    return gyro:GetAngle()
end

function disableGyro()
    gyroOkay = false
    dashboard:PutBoolean("Gyro Okay", false)
end

local gear = "high"

function getGear()
    return gear
end

function setGear(g)
    gear = g
    if gear == "low" then
        gearSwitch:Set(true)
    elseif gear == "high" then
        gearSwitch:Set(false)
    else
        -- Unrecognized state, default to low gear
        -- TODO: log error
        gearSwitch:Set(false)
    end
end

--[[
    calculate computes the wheel angles and speeds.

    x and y are the strafe inputs from the operator, w is the rotation speed
    about the z-axis.

    gyroDeg is the field-centric shift (in degrees).

    The units for wheelBase and trackWidth are irrelevant as long as they are
    consistent.

    (Consult Adam for the math.)
--]]
function calculate(x, y, w, gyroDeg, wheelBase, trackWidth)
    local r = math.sqrt(wheelBase ^ 2 + trackWidth ^ 2)
    local gyroRad = math.rad(gyroDeg)

    -- XXX: There is something in the wheel calculation below that flips the
    -- coordinate system (see comment).  As a kludge, we swap the x and y.
    x, y = y, x
    x, y = x * math.cos(gyroRad) + y * math.sin(gyroRad), -x * math.sin(gyroRad) + y * math.cos(gyroRad)

    -- XXX: The problem lies somewhere below here.
    local a = y - w * (wheelBase / r)
    local b = y + w * (wheelBase / r)
    local c = x - w * (trackWidth / r)
    local d = x + w * (trackWidth / r)

    local wheels = {rightFront={}, leftFront={}, rightBack={}, leftBack={}}
    wheels.rightFront.speed = math.sqrt(b * b + c * c)
    wheels.leftFront.speed = math.sqrt(b * b + d * d)
    wheels.leftBack.speed = math.sqrt(a * a + d * d)
    wheels.rightBack.speed = math.sqrt(a * a + c * c)
    wheels.rightFront.angleDeg = math.deg(math.atan2(b, c))
    wheels.leftFront.angleDeg = math.deg(math.atan2(b, d))
    wheels.leftBack.angleDeg = math.deg(math.atan2(a, d))
    wheels.rightBack.angleDeg = math.deg(math.atan2(a, c))

    -- Normalize wheel speeds
    local maxSpeed = math.max(
        wheels.rightFront.speed,
        wheels.leftFront.speed,
        wheels.rightBack.speed,
        wheels.leftBack.speed
    )
    if maxSpeed > 1 then
        for _, wheel in pairs(wheels) do
            wheel.speed = wheel.speed / maxSpeed
        end
    end

    return wheels
end

-- Wraps an angle (in degrees) to (-180, 180].
function normalizeAngle(theta)
    while theta > 180 do
        theta = theta - 360
    end
    while theta < -180 do
        theta = theta + 360
    end
    return theta
end

-- Calculate the error and the flip of the motor.
function calculateTurn(current, target)
    local err, flip = normalizeAngle(target - current), false
    if math.abs(err) > 90 then
        err, flip = normalizeAngle(err + 180), true
    end
    return err, flip
end

function angleError(current, target)
    local err, flip = calculateTurn(current, target)
    return err
end

function driveScale(err, flip)
    local scale
    if math.abs(err) < 45 then
        scale = math.cos(math.rad(err))
    else
        scale = 0
    end
    if flip then
        scale = -scale
    end
    return scale
end

local followerEncoderX = wpilib.Encoder(2, 13, 2, 14, true, wpilib.CounterBase_k1X)
local followerEncoderY = wpilib.Encoder(2, 11, 2, 12, false, wpilib.CounterBase_k1X)

followerEncoderX:Start()
followerEncoderY:Start()

local function convertFollowerToFeet(ticks)
    local followerEncoderCPR = 360
    local followerWheelDiameter = 2.75 -- inches
    return ticks / followerEncoderCPR * (followerWheelDiameter * math.pi) / 12
end

function getFollowerPosition()
    local x = convertFollowerToFeet(followerEncoderX:Get())
    local y = convertFollowerToFeet(followerEncoderY:Get())
    return x, y
end

function resetFollowerPosition()
    followerEncoderX:Reset()
    followerEncoderY:Reset()
end

function deployFollower()
    followerDeploy:Set(true)
end

function undeployFollower()
    followerDeploy:Set(false)
end

function setFrontSkid(deploy)
    frontSkid:Set(deploy)
end

function isFollowerStopped()
    local THRESHOLD = 0.1 -- feet per second
    local feetPerSecondX = convertFollowerToFeet(1 / followerEncoderX:GetPeriod())
    local feetPerSecondY = convertFollowerToFeet(1 / followerEncoderY:GetPeriod())
    return math.abs(feetPerSecondX) < THRESHOLD or math.abs(feetPerSecondY) < THRESHOLD
end

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

local turnPIDConstants = {p=0.06, i=0, d=0}
wheels = {
    leftFront={
        shortName="LF",
        driveMotor=LinearVictor(1, 7),
        turnMotor=LinearVictor(1, 8),

        calibrateSwitch=wpilib.DigitalInput(1, 12),
        turnEncoder=wpilib.Encoder(1, 10, 1, 11),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, angleError),
    },
    leftBack={
        shortName="LB",
        driveMotor=LinearVictor(1, 5),
        turnMotor=LinearVictor(1, 6),

        calibrateSwitch=wpilib.DigitalInput(1, 9),
        turnEncoder=wpilib.Encoder(1, 7, 1, 8),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, angleError),
    },
    rightFront={
        shortName="RF",
        driveMotor=LinearVictor(1, 3),
        turnMotor=LinearVictor(1, 4),

        calibrateSwitch=wpilib.DigitalInput(1, 6),
        turnEncoder=wpilib.Encoder(1, 4, 1, 5),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, angleError),
    },
    rightBack={
        shortName="RB",
        driveMotor=LinearVictor(1, 1),
        turnMotor=LinearVictor(1, 2),

        calibrateSwitch=wpilib.DigitalInput(1, 3),
        turnEncoder=wpilib.Encoder(1, 1, 1, 2),
        turnPID=pid.new(turnPIDConstants.p, turnPIDConstants.i,
                        turnPIDConstants.d, angleError),
    },
}

for _, wheel in pairs(wheels) do
    wheel.turnEncoder:SetDistancePerPulse(50.0 / 38.0)
    wheel.turnEncoder:SetReverseDirection(true)
    wheel.turnEncoder:Start()
    wheel.turnPID:start()
end

function run(strafe, rotation, driveMode)
    driveMode = driveMode or 0

    for _, wheel in pairs(wheels) do
        dashboard:PutString(wheel.shortName .. ".turnEncoder", wheel.turnEncoder:GetDistance())
    end

    local gyroAngle = normalizeAngle(-getGyroAngle())
    dashboard:PutInt("Gyro Angle", gyroAngle)

    local appliedGyro = gyroAngle
    local appliedRotation = rotation

    if driveMode ~= 0 then
        appliedGyro = 0
    end
    
    -- Keep rotation steady in deadband
    if rotation == 0 then
        if rotationPID.target == nil then
            if rotationHoldTimer == nil then
                rotationHoldTimer = wpilib.Timer()
                rotationHoldTimer:Start()
            elseif rotationHoldTimer:Get() > 0.5 then
                rotationHoldTimer:Stop()
                rotationHoldTimer = nil
                rotationPID.target = gyroAngle
                rotationPID:start()
            end
            appliedRotation = 0
        else
            appliedRotation = rotationPID:update(gyroAngle)
        end
    else
        rotationPID.target = nil
        rotationPID:stop()
    end
    
    local wheelValues = calculate(
        strafe.x, strafe.y, appliedRotation, appliedGyro,
        31.5,     -- wheel base (in inches)
        21.4      -- track width (in inches)
    )

    for wheelName, value in pairs(wheelValues) do
        local wheel = wheels[wheelName]

        local currentTurn = wheel.turnEncoder:GetDistance()

        if strafe.x ~= 0 or strafe.y ~= 0 or rotation ~= 0 then
            wheel.turnPID.target = normalizeAngle(value.angleDeg)
            local driveScale = driveScale(calculateTurn(currentTurn, wheel.turnPID.target))
            wheel.driveMotor:Set(value.speed * driveScale)
        else
            -- In deadband
            if driveMode == 1 then
                wheel.turnPID.target = 0
            elseif driveMode == 2 then
                wheel.turnPID.target = 90 
            else
                if wheelName == "leftFront" or wheelName == "rightBack" then
                    wheel.turnPID.target = 45
                else
                    wheel.turnPID.target = -45
                end
            end
            wheel.driveMotor:Set(0)
        end

        wheel.turnPID:update(currentTurn)
        wheel.turnMotor:Set(wheel.turnPID.output)
    end
end

function calibrateAll()
    dashboard:PutString("mode", "Calibrating")

    local dog = wpilib.GetWatchdog()
    local tolerance = 3.0 -- in degrees
    local numCalibratedWheels = 0
    local numWheels = 4
    local speed = 1.0
    for _, wheel in pairs(wheels) do
        wheel.turnEncoder:Reset()
        wheel.turnEncoder:Start()
        if wheel.calibrateSwitch:Get() then
            -- Already calibrated
            wheel.calibrateState = 6
            numCalibratedWheels = numCalibratedWheels + 1
        else
            wheel.calibrateState = 1
        end
    end
    while numCalibratedWheels < numWheels do
        for _, wheel in pairs(wheels) do
            if wheel.calibrateState == 1 then
                -- Initial state: turn clockwise, wait until tripped
                wheel.turnMotor:Set(speed)
                if wheel.calibrateSwitch:Get() then
                    wheel.calibrateState = 2
                end
            elseif wheel.calibrateState == 2 then
                -- Keep turning clockwise until falling edge
                wheel.turnMotor:Set(speed)
                if not wheel.calibrateSwitch:Get() then
                    wheel.Angle1 = wheel.turnEncoder:GetDistance()
                    wheel.calibrateState = 3
                end
            elseif wheel.calibrateState == 3 then
                -- Turn counter-clockwise, wait until tripped
                wheel.turnMotor:Set(-speed)
                if wheel.calibrateSwitch:Get() then
                    wheel.calibrateState = 4
                end
            elseif wheel.calibrateState == 4 then
                -- Keep turning counter-clockwise until falling edge
                wheel.turnMotor:Set(-speed)
                if not wheel.calibrateSwitch:Get() then
                    wheel.Angle2 = wheel.turnEncoder:GetDistance()
                    wheel.calibrateState = 5
                end
            elseif wheel.calibrateState == 5 then
                -- Move clockwise back to zero
                wheel.turnMotor:Set(speed)
                local targetAngle = (wheel.Angle1 + wheel.Angle2)/2
                local dist = math.abs(wheel.turnEncoder:GetDistance() - targetAngle)
                if dist < tolerance then
                    wheel.turnMotor:Set(0)
                    wheel.turnEncoder:Reset()
                    numCalibratedWheels = numCalibratedWheels + 1
                    wheel.calibrateState = 6
                end
            end
        end

        dog:Feed()
        wpilib.Wait(0.005)
        dog:Feed()
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
