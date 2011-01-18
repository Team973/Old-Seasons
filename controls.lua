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

controls =
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
        [9] = restartRobot
    }
}

local previousState = {}

for i, stick in ipairs(sticks) do
    previousState[i] = {}
    for button = 1, 11 do
        previousState[i][button] = stick:GetRawButton(button)
    end
end

local function updateButton(buttonTable, prev, curr)
    if curr and not prev then
        if buttonTable.down then buttonTable.down() end
    elseif not curr and prev then
        if buttonTable.up then buttonTable.up() end
    end
end

function update(map)
    for i, stick in ipairs(sticks) do
        local stickMap = map[i]
        -- Update axes
        if stickMap.x then stickMap.x(stick:GetX()) end
        if stickMap.y then stickMap.y(stick:GetY()) end
        -- Update button events
        for button = 1, 11 do
            local buttonTable = stickMap[button]
            if buttonTable then
                updateButton(buttonTable, previousState[i][button], stick:GetRawButton(button))
            end
        end
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
