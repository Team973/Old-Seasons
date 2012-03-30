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

function linearize(goalSpeed)
    local deadbandValue = 0.082
    if goalSpeed > deadbandValue then
        goalSpeed = goalSpeed - deadbandValue
    elseif goalSpeed < -deadbandValue then
        goalSpeed = goalSpeed + deadbandValue
    else
        goalSpeed = 0
    end
    goalSpeed = goalSpeed / (1 - deadbandValue)

    local goalSpeed2 = goalSpeed * goalSpeed
    local goalSpeed3 = goalSpeed2 * goalSpeed
    local goalSpeed4 = goalSpeed3 * goalSpeed
    local goalSpeed5 = goalSpeed4 * goalSpeed
    local goalSpeed6 = goalSpeed5 * goalSpeed
    local goalSpeed7 = goalSpeed6 * goalSpeed

    -- Constants for the 5th order polynomial
    local victorFitE1 = 0.437239
    local victorFitC1 = -1.56847
    local victorFitA1 = -(125.0 * victorFitE1 + 125.0 * victorFitC1 - 116.0) / 125.0
    
    local answer5thOrder = (victorFitA1 * goalSpeed5 + victorFitC1 * goalSpeed3 + victorFitE1 * goalSpeed)

    -- Constants for the 7th order polynomial
	local victorFitC2 = -5.46889
	local victorFitE2 = 2.24214
	local victorFitG2 = -0.042375
	local victorFitA2 = -(125.0 * (victorFitC2 + victorFitE2 + victorFitG2) - 116.0) / 125.0
	local answer7thOrder = victorFitA2 * goalSpeed7 + victorFitC2 * goalSpeed5 + victorFitE2 * goalSpeed3 + victorFitG2 * goalSpeed

    -- Average the 5th and 7th order polynomials
    local answer = 0.85 * 0.5 * (answer7thOrder + answer5thOrder) + .15 * goalSpeed * (1.0 - deadbandValue)

    if answer > 0.001 then
        answer = answer + deadbandValue
    elseif answer < -0.001 then
        answer = answer - deadbandValue
    end
    
    return answer
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
