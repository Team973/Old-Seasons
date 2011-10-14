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

function elevatorOutput(control)
    local steady = 0.0
    local upScale = 1.0
    local downScale = 1.0

    local scale
    if control >= 0 then
        scale = upScale
    else
        scale = downScale
    end

    local output = control * scale + steady
    if output > 1.0 then
        output = 1.0
    elseif output < -1.0 then
        output = -1.0
    end
    return output
end

-- Get piston outputs from a chosen claw state.
-- state is 1 for open, 0 for closed, -1 for neutral.
-- Returns openPiston1, openPiston2, closePiston1, closePiston2.
function clawPistons(state)
    -- Remember that close piston commands opposite for safety reasons.
    if state == 1 then
        -- Open
        return true, false, false, true
    elseif state == 0 then
        -- Closed
        return false, true, true, false
    else
        -- Neutral
        return false, true, false, true
    end
end
