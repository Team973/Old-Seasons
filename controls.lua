-- controls.lua

local wpilib = require("wpilib")
local ipairs = ipairs

module(...)

local sticks = {}

for i = 1, 3 do
    sticks[i] = wpilib.Joystick(i)
end

controls =
{
    -- Joystick 1
    {
    },
    -- Joystick 2
    {
    },
    -- Joystick 3
    {
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
