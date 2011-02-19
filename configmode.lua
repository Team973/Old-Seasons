-- configmode.lua

local arm = require("arm")
local config = require("config")
local controls = require("controls")
local drive = require("drive")
local io = require("io")
local restartRobot = restartRobot
local tostring = tostring

module(...)

local armPositionForward, armPositionReverse

controlMap = {
    -- Joystick 1
    {},
    -- Joystick 2
    {},
    -- Joystick 3
    {
        ["y"] = function(axis) arm.setMovement(axis) end,
        [11] = {down=function() armPositionForward = config.armPot:GetVoltage() end},
        [10] = {down=function() armPositionReverse = config.armPot:GetVoltage() end},
    },
    -- Joystick 4 (eStop)
    {},
}

function start()
    drive.arcade(0, 0)
    arm.setManual(true)
    arm.setMovement(0)
    arm.setWristMotor(0)
    arm.setGripMotor(0)

    armPositionForward = config.armPositionForward
    armPositionReverse = config.armPositionReverse
end

function finish()
    do
        local f = io.open("lua/config/override.lua", "w")
        f:write("-- config/override.lua\n")
        f:write("module(...)\n")
        f:write("armPositionForward = " .. tostring(armPositionForward) .. "\n")
        f:write("armPositionReverse = " .. tostring(armPositionReverse) .. "\n")
        f:close()
    end
    restartRobot()
end
