-- controls.lua

local arm = require("arm")
local drive = require("drive")
local wpilib = require("wpilib")

local ipairs = ipairs
local restartRobot = restartRobot

module(...)

local sticks = {}
local cypress = wpilib.DriverStation_GetInstance():GetEnhancedIO()

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
        ["y"] = function(axis) if arm.getManual() then arm.setMovement(axis) else arm.setWristMotor(axis) end,
        [1] = {down=function() arm.openClaw() end},
        [6] = {
            down=function() arm.setManual(true) end,
            up=function() arm.setManual(false) end,
        },
        [9] = {down=restartRobot},

        update = function(stick)
            if stick:GetRawButton(2) or stick:GetRawButton(3) or stick:GetRawButton(4) or stick:GetRawButton(5) then
                arm.closeClaw()
                arm.setGripMotor(1)
            else
                arm.setGripMotor(0)
            end
        end,
    },
    -- Cypress Module
    cypress={
        [13] = {down=function() arm.setPreset("pickup") end},
        [9] = {down=function() arm.setPreset("stow") end},
        [5] = {down=function() arm.setPreset("slot") end},
    },
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
        for button = 1, 11 do
            previousState[i][button] = stick:GetRawButton(button)
        end
    end
    if cypress then
        for button = 1, 16 do
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
        -- Call update
        if stickMap.update then stickMap.update(stick) end
    end
    -- Cypress
    if cypress then
        for button = 1, 16 do
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
