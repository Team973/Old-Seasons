-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")
local pid = require("pid")
local robot = require("robot")

module(...)

local deploy = false
local retract = false
local intakeDeploySpeed = .5
local intakeSpeed = 0

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)
local intakePot = wpilib.AnalogChannel(8)

local intakePID = pid.new(0, 0, 0)
intakePID.min, intakePID.max = -.5, .5
intakePID:start()

PRESETS = {
    Stow = { angle = 0.0 },
}

function getAngle()
    return intakePot:GetVoltage()
end

function getTarget()
    return intakePID.target
end

function setTarget(target)
    intakePID.target = target
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setTarget(p.angle)
    end
end

function setIntakeSpeed(speed)
    intakeSpeed = speed
end

function update()
    local currState = robot.getIntakeState()
    intakeRollers:Set(intakeSpeed)

    if currState == DEPLOYED then
        if arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif currState == STOW then
        if getAngle() > 1 and arm.isIntakeDeploySafe() then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    elseif currState == DOWN then
        if getAngle() < 3.4 then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    else
        motor:Set(0.0)
    end
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Intake Angle", getAngle())
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
