-- intake.lua

local wpilib = require("wpilib")
local pid = require("pid")

module(...)

local deploy = false
local retract = false
local intakeDeploySpeed = .5
local intakeSpeed = 0

local motor = wpilib.Victor(4)
local intakeRollers = wpilib.Victor(5)
local intakePot = wpilib.AnalogChannel(1)

-- Intake States
local intakeMoveState = "noState"
local intakeState = "noState"
local STOWED = "stowed"
local DEPLOYED = "deployed"
local INTAKE_LOAD = "intake_load"
local HUMAN_LOAD = "human_load"
local DOWN = "down"

local intakePID = pid.new(5, 0, 0)
intakePID.min, intakePID.max = -1, 1
intakePID:start()

PRESETS = {
    Stow = { angle = 2.0 },
    Deployed = { angle = 4.2 },
    Human = { angle = 3.07 },
    Intake = { angle = 4.3 },
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

function goToStow(bool)
    stowed = bool
    if stowed then
        intakeMoveState = STOWED
        setPreset("Stow")
        stowed = false
    end
end

function goToDeploy(bool)
    deployed = bool
    if deployed then
        intakeMoveState = DEPLOYED
        setPreset("Deployed")
        deployed = false
    end
end

function goToIntake(bool)
    intaking = bool
    if intaking then
        intakeState = INTAKE_LOAD
        setPreset("Intake")
        intaking = false
    end
end

function goToHuman(bool)
    loading = bool
    if loading then
        intakeState = HUMAN_LOAD
        setPreset("Human")
        loading = false
    end
end

function goToDown(bool)
    down = bool
    if down then
        intakeState = DOWN
        setPreset("Deployed")
        down = false
    end
end

function update()
    intakeRollers:Set(intakeSpeed)

    if intakeMoveState == DEPLOYED  then
        if intakeState == INTAKE_LOAD then
            if getAngle() < 4.1 then
                motor:Set(intakePID:update(getAngle()))
            else
                motor:Set(0.0)
            end
        elseif intakeState == HUMAN_LOAD  then
            motor:Set(intakePID:update(getAngle()))
        elseif intakeState == DOWN then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(intakePID:update(getAngle()))
        end
    elseif intakeMoveState == STOWED then
        if getAngle() > 2.2 then
            motor:Set(intakePID:update(getAngle()))
        else
            motor:Set(0.0)
        end
    else
        motor:Set(0.0)
    end
end

function getState()
    return intakeState
end

function getMoveState()
    return intakeMoveState
end

function dashboardUpdate()
    wpilib.SmartDashboard_PutNumber("Intake Angle", getAngle())
    wpilib.SmartDashboard_PutNumber("Intake PID Output", intakePID:update(getAngle()))
    wpilib.SmartDashboard_PutNumber("Intake Motor Output", motor:Get())
    wpilib.SmartDashboard_PutNumber("Intake HIT", 0)
    wpilib.SmartDashboard_PutNumber("Intake Error", getTarget() - getAngle())
    wpilib.SmartDashboard_PutString("Intake State", intakeState)
end

function fullStop()
    motor:Set(0.0)
    intakeRollers:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
