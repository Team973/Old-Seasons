-- controls.lua

local arm = require("arm")
local drive = require("drive")
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
        [1] = {down=function() arm.openClaw() end},
        [9] = {down=restartRobot},

        update = function(stick)
            -- Intake
            if stick:GetRawButton(2) or stick:GetRawButton(3) or stick:GetRawButton(4) or stick:GetRawButton(5) then
                arm.closeClaw()
                arm.setGripMotor(1)
            else
                arm.setGripMotor(0)
            end
            -- Arm Control
            if stick:GetRawButton(7) then
                arm.setManual(true)
                arm.setMovement(stick:GetY())
            else
                arm.setManual(false)
                arm.setMovement(0)
            end
            -- Wrist Control
            if stick:GetRawButton(6) then
                arm.setWristMotor(-stick:GetY())
            else
                arm.setWristMotor(0)
            end
        end,
    },
    -- Joystick 4 (eStop Module)
    {
        [2] = {down=function() arm.setPreset("slot") end},
        [3] = {down=function() arm.setPreset("stow") end},
        [4] = {down=function() arm.setPreset("pickup") end},
        [5] = {down=function() arm.setPreset("low") end},
        [6] = {down=function() arm.setPreset("middle") end},
        [7] = {down=function() arm.setPreset("high") end},
        [8] = {down=function() arm.setPreset("midLow") end},
        [9] = {down=function() arm.setPreset("midMiddle") end},
        [10] = {down=function() arm.setPreset("midHigh") end},
        update = function(stick)
            arm.setForward(not stick:GetRawButton(12))
        end,
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
