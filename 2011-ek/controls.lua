-- controls.lua

local math = require("math")
local wpilib = require("wpilib")

local ipairs = ipairs
local restartRobot = restartRobot

module(...)

sticks = {}
local cypress = nil
local numButtons = 12
local numCypressButtons = 16

for i = 1, 4 do
    sticks[i] = wpilib.Joystick(i)
end

defaultControls =
{
    -- Joystick 1
    {
        ["x"] = function(axis)
            -- code here...
        end,
        ["y"] = function(axis) end,
        [1] = {down=function() end}
    },
    -- Joystick 2
    {
        ["x"] = function(axis) end,
        [1] = {down=function() end}
    },
    -- Joystick 3
    {
        [1] = {
            down=function() end,
            up=function() end,
        },
        update = function(stick) end,
    },
    -- Joystick 4 (eStop Module)
    {
    },
    -- Cypress Module
    cypress={},
}

-- Initialize previous state table
local previousState = {}
for i = 1, #sticks do
    previousState[i] = {}
end
previousState.cypress = {}

-- storeState stores the current state of the controls into the previousState
-- table.
local function storeState()
    for i, stick in ipairs(sticks) do
        for button = 1, numButtons do
            previousState[i][button] = stick:GetRawButton(button)
        end
    end
    if cypress then
        for button = 1, numCypressButtons do
            previousState.cypress[button] = cypress:GetDigital(button)
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
    if buttonTable.tick then buttonTable.tick(curr) end
end

-- update calls the event handlers.
function update(map)
    for i, stick in ipairs(sticks) do
        local stickMap = map[i]
        -- Update axes
        if stickMap.x then stickMap.x(stick:GetX()) end
        if stickMap.y then stickMap.y(stick:GetY()) end
        -- Update button events
        for button = 1, numButtons do
            local currValue = stick:GetRawButton(button)
            local buttonTable = stickMap[button]
            if buttonTable then
                handleButton(buttonTable, previousState[i][button], currValue)
            end
        end
        -- Call update
        if stickMap.update then stickMap.update(stick) end
    end
    -- Cypress
    if cypress then
        for button = 1, numCypressButtons do
            local currValue = cypress:GetDigital(button)
            local buttonTable = map.cypress[button]
            if buttonTable then
                handleButton(buttonTable, previousState.cypress[button], currValue)
            end
        end
        -- Call update
        if map.cypress.update then map.cypress.update(cypress) end
    end

    -- Call update
    if map.update then map.update() end
    -- Save previous state
    storeState()
end

-- vim: ft=lua et ts=4 sts=4 sw=4
