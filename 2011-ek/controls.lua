-- controls.lua

local wpilib = require("wpilib")

local ipairs = ipairs
local type = type

module(...)

sticks = {}
local cypress = nil
local numButtons = 12
local numCypressButtons = 16

for i = 1, 4 do
    sticks[i] = wpilib.Joystick(i)
end

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
local function handleButton(buttonHandler, prev, curr)
    if type(buttonHandler) == "function" then
        -- If the handler is a function, then it's the button down event.
        if curr and not prev then buttonHandler() end
        return
    end

    if curr and not prev then
        if buttonHandler.down then buttonHandler.down() end
    elseif not curr and prev then
        if buttonHandler.up then buttonHandler.up() end
    end
    if buttonHandler.tick then buttonHandler.tick(curr) end
end

-- update calls the event handlers.
function update(map)
    for i, stick in ipairs(sticks) do
        local stickMap = map[i]
        -- Update axes
        -- [
        if stickMap.x then stickMap.x(stick:GetX()) end
        if stickMap.y then stickMap.y(stick:GetY()) end
        if stickMap.rx then stickMap.rx(stick:GetRawAxis(3))end
        if stickMap.ry then stickMap.ry(sitck:GetRawAxis(4))end
    
        -- Update button events
        for button = 1, numButtons do
            local currValue = stick:GetRawButton(button)
            local buttonHandler = stickMap[button]
            if buttonHandler then
                handleButton(buttonHandler, previousState[i][button], currValue)
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
