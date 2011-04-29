-- drive.lua

local config = require "config"
local math = require "math"
local pid = require "pid"
local wpilib = require "wpilib"

module(...)

local holdPosition = false

function init()
    config.leftDriveEncoder:Start()
    config.rightDriveEncoder:Start()
end

local d
if config.leftMotor2 and config.rightMotor2 then
    d = wpilib.RobotDrive(
        config.leftMotor1, config.leftMotor2,
        config.rightMotor1, config.rightMotor2
    )
else
    d = wpilib.RobotDrive(config.leftMotor1, config.rightMotor1)
end
d:SetSafetyEnabled(false)

local hiGear = false
local x, y = 0, 0

function setGear(gear)
    hiGear = gear
end

function arcade(newY, newX)
    if newY then y = newY end
    if newX then x = newX end
end

function getDrive()
    return d
end

local drivePID, turnDrivePID

function hold()
    if not holdPosition then
        -- TODO: Make this more DRY
        config.leftDriveEncoder:Reset()
        config.rightDriveEncoder:Reset()

        drivePID = pid.PID:new(3, 0, 0)
        drivePID.min, drivePID.max = -0.25, 0.25
        drivePID:reset()
        drivePID:start()
        drivePID.target = 0

        turnDrivePID = pid.PID:new(0.18, 0, 0.017)
        turnDrivePID:reset()
        turnDrivePID:start()
        turnDrivePID.target = 0
    end
    holdPosition = true
end

function unhold()
    holdPosition = false
    drivePID, turnDrivePID = nil, nil
end

function getDistance()
    return (config.leftDriveEncoder:GetDistance() + config.rightDriveEncoder:GetDistance()) / 2
end

-- CW is positive
function getAngle()
    return (config.leftDriveEncoder:GetDistance() * 1.105 - config.rightDriveEncoder:GetDistance()) / config.robotWidth * (180 / math.pi)
end

function update()
    if not holdPosition then
        if config.flipDriveY then
            d:ArcadeDrive(-y, x)
        else
            d:ArcadeDrive(y, x)
        end
    else
        drivePID:update(getDistance())
        turnDrivePID:update(getAngle())
        local speedL = drivePID.output + turnDrivePID.output
        local speedR = drivePID.output - turnDrivePID.output
        if math.abs(speedL) > 1 or math.abs(speedR) > 1 then
            local n = math.max(math.abs(speedL), math.abs(speedR))
            speedL = speedL / n
            speedR = speedR / n
        end
        d:SetLeftRightMotorOutputs(speedL, speedR)
    end

    if config.features.gearSwitch then
        config.gearSwitch:Set(hiGear)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
