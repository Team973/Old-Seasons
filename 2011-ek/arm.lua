-- arm.lua

module(...)

local presets = {
    bottom = {
        elevator = 0.0,
        wrist = nil,
    },
}

function presetElevatorTarget(presetName)
    if not presets[presetName] then
        return nil
    end
    return presets[presetName].elevator
end

function presetWrist(presetName)
    if not presets[presetName] then
        return nil
    end
    return presets[presetName].wrist
end

-- Return the P constant for the elevator.
-- It depends on the current position and the target position.
function elevatorP(current, target)
    if target >= current then
        -- Going up
        return 1.0
    else
        -- Going down
        return 1.0
    end
end

-- Get piston outputs from a chosen claw state.
-- state is 1 for open, 0 for closed, -1 for neutral.
-- Returns openPiston, closePiston.
function clawPistons(state)
    -- Remember that close piston commands opposite for safety reasons.
    if state == 1 then
        -- Open
        return true, true
    elseif state == 0 then
        -- Closed
        return false, false
    else
        -- Neutral
        return false, true
    end
end
