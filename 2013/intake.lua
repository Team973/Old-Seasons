-- intake.lua

local arm = require("arm")
local wpilib = require("wpilib")

module(...)

local lowered = false

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)

local intakePID = pid.new(0, 0, 0)
intakePID.min, intakePID.max = -0.3, 0.3
intakePID:start()

PRESETS = {
    Down = { intakeAngle = 45 },
    Stow = { intakeAngle = 5 },
}

function getIntakeTarget()
    return intakePID.target
end

function getIntakeAngle()
end

function setIntakeTarget(target)
    intakePID.target = target
end

function setIntakeSpeed(speed)
    intakeSpeed = speed
end

function intakeStopCheck(angle)
    local currAngle = angle - getIntakeAngle()
    if currAngle >= -.5 and currAngle <= .5 then
        return true
    else
        return false
    end
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setIntakeTarget(p.intakeAngle)
    end
end

function setLowered(val)
    if lowered ~= val and arm.isIntakeDeploySafe() then
        lowered = val
        setPreset("Down")
    end
end

function toggleRaise()
    setLowered(not lowered)
end

function update()
    intakeRollers:Set(intakeSpeed)

    if not intakeStopCheck() then
        motor:Set(intakePID:update(getAngle()))
    else
        fullStop()
    end
end

function fullStop()
    motor:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
