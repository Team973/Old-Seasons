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
local gearSwitch = wpilib.Solenoid(1, 1)

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

local function LinearVictor(...)
    return linearize.wrap(wpilib.Victor(...))
end
-- vim: ft=lua et ts=4 sts=4 sw=4
