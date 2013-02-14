-- arm.lua

local wpilib = require("wpilib")
local pid = require("pid")

module(...)

encoder = wpilib.Encoder(1, 2)
motor = wpilib.Talon(8)

armPID = pid.new(1000, 0, 0)
armPID:start()

encoder:Start()

PRESETS = {
    Arm1 = { armAngle = 10 },
    Arm2 = { armAngle = 50 },
}

function setPreset(name)
    local p = PRESETS[name]

    if p.Arm1 then
        setArmTarget(p.Arm1)
    end

    if p.Arm2 then
        setArmTarget(p.Arm2)
    end
end

function setArmTarget(target)
    armPID.target = target
end

--armPID.min = -.3
--armPID.max = .3


function update()

    armPID:update(encoder:Get()/3 * 50)
    -- divide by 50 and * 3

    motor:Set(armPID.output)

    wpilib.SmartDashboard_PutNumber("Arm PID", armPID.output)
end
