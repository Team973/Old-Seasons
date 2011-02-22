-- configmode.lua

local arm = require("arm")
local config = require("config")
local controls = require("controls")
local drive = require("drive")
local io = require("io")
local ipairs = ipairs
local pairs = pairs
local restartRobot = restartRobot
local string = string
local tostring = tostring
local type = type

module(...)

local newValues = {}
local isForward = true
local valueNames = {
    "armPositionForward", 
    "armPositionReverse",
    "wristPositionForward", 
    "wristPositionReverse",
    "armPresets",
}

local function uberTostring(val, indent)
    local t = type(val)
    if not indent then
        indent = ""
    end
    local nextIndent = indent .. "\t"
    if t == "number" or t == "boolean" or t == "nil" then
        return tostring(val)
    elseif t == "string" then
        -- Return quoted string
        return string.format("%q", val)
    elseif t == "table" then
        local s = "{\n"
        for k, v in pairs(val) do
            s = s .. nextIndent .. string.format("[%s] = %s,\n", uberTostring(k), uberTostring(v, nextIndent))
        end
        return s .. indent .. "}"
    else
        return t
    end
end

controlMap = {
    -- Joystick 1
    {},
    -- Joystick 2
    {
        ["y"] = function(axis) arm.setWristMotor(axis) end,
    },
    -- Joystick 3
    {
        ["y"] = function(axis) arm.setMovement(axis) end,
        [3] = {down=function() storePreset("stow") end},
        [6] = {down=function() newValues.wristPositionForward = config.wristPot:GetVoltage() end},
        [7] = {down=function() newValues.wristPositionReverse = config.wristPot:GetVoltage() end},
        [11] = {down=function() newValues.armPositionForward = config.armPot:GetVoltage() end},
        [10] = {down=function() newValues.armPositionReverse = config.armPot:GetVoltage() end},
    },
    -- Joystick 4 (eStop)
    {
        [2] = {down=function() storePreset("slot") end},
        [3] = {down=function() storePreset("carry") end},
        [4] = {down=function() storePreset("pickup") end},
        [5] = {down=function() storePreset("low") end},
        [6] = {down=function() storePreset("middle") end},
        [7] = {down=function() storePreset("high") end},
        [8] = {down=function() storePreset("midLow") end},
        [9] = {down=function() storePreset("midMiddle") end},
        [10] = {down=function() storePreset("midHigh") end},
        update = function(stick)
            isForward = not stick:GetRawButton(12)
        end,
    },
}

function storePreset(name)
    local preset, armRefPoint, wristRefPoint
    if isForward then
        preset = newValues.armPresets.forward[name]
        armRefPoint = newValues.armPositionForward
        wristRefPoint = newValues.wristPositionForward
    else
        preset = newValues.armPresets.reverse[name]
        armRefPoint = newValues.armPositionReverse
        wristRefPoint = newValues.wristPositionReverse
    end
    preset.arm = config.armPot:GetVoltage() - armRefPoint
    preset.wrist = config.wristPot:GetVoltage() - wristRefPoint
end

function start()
    drive.arcade(0, 0)
    arm.setManual(true)
    arm.setMovement(0)
    arm.setWristMotor(0)
    arm.setGripMotor(0)
    arm.setSafety(false)

    for i, name in ipairs(valueNames) do
        newValues[name] = config[name]
    end
end

function finish()
    do
        local f = io.open("lua/config/override.lua", "w")
        f:write("-- config/override.lua\n")
        f:write("module(...)\n")
        for i, name in ipairs(valueNames) do
            f:write(name .. "=" .. uberTostring(newValues[name]) .. "\n")
        end
        f:close()
    end
    restartRobot()
end
