-- arm.lua

local wpilib = require("wpilib")
local pid = require("pid")

module(...)

local encoder = wpilib.Encoder(1, 2, true)
local motor = wpilib.Talon(8)
local absoluteEncoder = wpilib.AnalogChannel(1)

local armPID = pid.new(0.05, 0, 0)
armPID.min, armPID.max = -1.0, 1.0
armPID:start()

encoder:Start()

PRESETS = {
    Arm1 = { armAngle = 10 }, 
    Arm2 = { armAngle = 30 }, 
}

function setArmTarget(target)
    armPID.target = target
end

function setPreset(name)
    local p = PRESETS[name]
    if p then
        setArmTarget(p.armAngle)
    end
end

function update()
    local angle = encoder:Get() / 50 * 3
    motor:Set(-armPID:update(angle))

    wpilib.SmartDashboard_PutNumber("Arm Angle", angle)
    wpilib.SmartDashboard_PutNumber("Arm PID Output", armPID.output)
    wpilib.SmartDashboard_PutNumber("Potentiometer Output", absoluteEncoder:GetVoltage())
end
