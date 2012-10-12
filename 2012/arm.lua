-- arm.lua

local math = require("math")

module(...)

local presets = {
    pickup = {elevator=0.08, wrist=false},
    carry = {elevator=0.08, wrist=true},
    slot = {elevator=0.08, wrist=true},
    low = {elevator=0.08, wrist=true},
    middle = {elevator=3.00, wrist=true},
    high = {elevator=5.85, wrist=true},
    midLow = {elevator=0.08, wrist=true},
    midMiddle = {elevator=3.50, wrist=true},
    midHigh = {elevator=6.41, wrist=true},
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

local elevatorEncoderScale = (2 * math.pi / 100) / 12 -- feet / tick

function elevatorEncoderToFeet(ticks)
    return ticks * elevatorEncoderScale
end

function elevatorFeetToEncoder(feet)
    return feet / elevatorEncoderScale
end

UP_P = 0.3
DOWN_P = 0.3

function elevatorP(current, target)
    if current <= target then
        -- Elevator going up
        return UP_P
    else
        -- Elevator going down
        return DOWN_P
    end
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

local maxMotorRate = 1.0 -- in motor units/second
function limitRate(curr, target, t)
    local rate = (target - curr)/t
    if (curr > 0 and target < 0) or (curr < 0 and target > 0) then
        return 0.0
    elseif rate > maxMotorRate then
        return curr + maxMotorRate/t
    elseif rate < -maxMotorRate then
        return curr - maxMotorRate/t
    else
        return target
    end
end
