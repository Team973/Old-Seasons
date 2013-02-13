-- arm.lua

local wpilib = require("wpilib")
local pid = require("pid")

module(...)

encoder = wpilib.Encoder(1, 2)
motor = wpilib.Talon(8)

armPID = pid.new(0, 0, 0)
armPid:start()

encoder:Start()

function setArmTarget(val)
    target = val
end

function limitArm(x)
    if x > .3 then
        x = .3
    elseif x < -.3 then
        x = -.3
    else
        x = x 
    end
    return x
end

function update()

    armPID:update(encoder:Get()/50 * 3)

    armPID.target = target


    motor:Set(limitArm(armPID.output))
end
