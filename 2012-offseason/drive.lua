-- drive.lua

local linearize = require("linearize")
local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

local pairs = pairs

local ROBOTNAME = ROBOTNAME

module(...)

local dashboard = wpilib.SmartDashboard_GetInstance()

local gyro = nil
local gyroOkay = true
local ignoreGyro = false
local gearSwitch
if ROBOTNAME == "hodgepodge" then
    gearSwitch = wpilib.Solenoid(1)
end
local brake1, brake2
if ROBOTNAME == "hodgepodge" then
    brake1 = wpilib.Solenoid(4)
    brake2 = wpilib.Solenoid(5)
end
local brakesFired = false

local function limit(x)
    if x > 1 then
        return 1
    elseif x < -1 then
        return -1
    else
        return x
    end
end

local function signSquare(x)
    if x < 0 then return -x*x end
    return x*x
end

local function arcade(move, rotate)
    move = signSquare(limit(move))
    rotate = signSquare(limit(rotate))
    if move > 0 then
        if rotate > 0 then
            return move - rotate, math.max(move, rotate)
        else
            return math.max(move, -rotate), move + rotate
        end
    else
        if rotate > 0 then
            return -math.max(-move, rotate), move + rotate
        else
            return move - rotate, -math.max(-move, -rotate)
        end
    end
end

function initGyro()
    gyro = wpilib.Gyro(1, 1)
    gyro:SetSensitivity(0.00703)
    gyro:Reset()
    gyroOkay = true
    dashboard:PutBoolean("Gyro Okay", true)
end

function resetGyro()
    gyro:Reset()
    ignoreGyro = false
end

function effTheGyro()
    ignoreGyro = true
end

function getGyroAngle()
    if not gyroOkay or ignoreGyro then
        return 0
    end
    return -gyro:GetAngle()
end

function disableGyro()
    gyroOkay = false
    dashboard:PutBoolean("Gyro Okay", false)
end

local gear = "high"

local leftDriveMotor = wpilib.Victor(1)
local rightDriveMotor = wpilib.Victor(2)

function getGear()
    return gear
end

function setGear(g)
    gear = g
    if not gearSwitch then
        return
    end

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

function toggleBrakes()
    brakesFired = not brakesFired
end

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end

function update(driveX,driveY)
	local leftSpeed, rightSpeed = arcade(driveY, driveX)
	leftDriveMotor:Set(-leftSpeed)
	rightDriveMotor:Set(rightSpeed)
        if brake1 and brake2 then
            brake1:Set(brakesFired)
            brake2:Set(not brakesFired)
        end
end

--[[
drive victors:
left 2,
right 1,
Intake victors:
3
shooter:
4
Cheater:
5
Vertical:
6
SENSORS:
pressure switch:
2
banner sensor:
3
Pneumatics:
drive:
1
intake:
2
stinger:
3
--]]
-- vim: ft=lua et ts=4 sts=4 sw=4
