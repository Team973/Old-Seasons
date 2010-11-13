-- intake.Lua

require "wpilib"
require "config"

module(..., package.seeall)

local ballHeld = false
local state = 0 -- -1 is exhaust, 0 is don't run, 1 is run
local possessionTimer = wpilib.Timer()

function update()
    local speed = 1.0
    config.intakeMotor:Set(speed * state)
    if state == 1 then
        possessionTimer:Start()
        local timeElapsed = possessionTimer:Get()
        if timeElapsed > 0.1 then
            local rate = config.intakeEncoder:GetDistance() / timeElapsed
            ballHeld = (rate < 180)
            -- resetting possession checking
            possessionTimer:Reset()
            config.intakeEncoder:Reset()
        end
    else
        ballHeld = false
        possessionTimer:Reset()
        possessionTimer:Stop()
    end
end

function hasBall()
    return ballHeld
end

function changeState(newState)
    state = newState 
end
