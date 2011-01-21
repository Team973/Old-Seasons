-- controls.lua

-- Imports
local arm = require("arm")
local drive = require("drive")
local wpilib = require("wpilib")

-- Globals
local ipairs = ipairs
local restartRobot = restartRobot

module(...)

local sticks = {}

for i = 1, 3 do
    sticks[i] = wpilib.Joystick(i)
end

defaultControls =
{
    -- Joystick 1
    {
        ["y"] = function(axis) drive.arcade(-axis, nil) end,
        [1] = {down=function() drive.setGear(false) end}
    },
    -- Joystick 2
    {
        ["x"] = function(axis) drive.arcade(nil, -axis) end,
        [1] = {down=function() drive.setGear(true) end}
    },
    -- Joystick 3
    {
        ["y"] = arm.setMovement,
        [3] = {
            down=function() arm.setPreset(2); arm.setManual(false) end,
            up=function() arm.setManual(true) end
        },
        [4] = {
            down=function() arm.setPreset(1); arm.setManual(false) end,
            up=function() arm.setManual(true) end
        },
        [5] = {
            down=function() arm.setPreset(3); arm.setManual(false) end,
            up=function() arm.setManual(true) end
        },
        [9] = {down=restartRobot}
    }
}

-- Initialize previous state table
local previousState = {}
for i = 1, #sticks do
    previousState[i] = {}
end

-- storeState stores the current state of the controls into the previousState
-- table.
local function storeState()
    for i, stick in ipairs(sticks) do
        for button = 1, 11 do
            previousState[i][button] = stick:GetRawButton(button)
        end
    end
end

storeState()

-- handleButton calls the appropriate event handlers for a single button.
local function handleButton(buttonTable, prev, curr)
    if curr and not prev then
        if buttonTable.down then buttonTable.down() end
    elseif not curr and prev then
        if buttonTable.up then buttonTable.up() end
    end
end

-- update calls the event handlers.
function update(map)
    for i, stick in ipairs(sticks) do
        local stickMap = map[i]
        -- Update axes
        if stickMap.x then stickMap.x(stick:GetX()) end
        if stickMap.y then stickMap.y(stick:GetY()) end
        -- Update button events
        for button = 1, 11 do
            local currValue = stick:GetRawButton(button)
            local buttonTable = stickMap[button]
            if buttonTable then
                handleButton(buttonTable, previousState[i][button], currValue)
            end
        end
    end
    -- Save previous state
    storeState()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
