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
local valueNames = {
    "armPositionForward", 
    "armPositionReverse",
}

local function uberTostring(val, indent)
    local t = type(val)
    if not indent then
        indent = ""
    end
    local nextIndent = indent .. "\t"
    if t == "number" or t == "boolean" then
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
    end
end

controlMap = {
    -- Joystick 1
    {},
    -- Joystick 2
    {},
    -- Joystick 3
    {
        ["y"] = function(axis) arm.setMovement(axis) end,
        [11] = {down=function() newValues.armPositionForward = config.armPot:GetVoltage() end},
        [10] = {down=function() newValues.armPositionReverse = config.armPot:GetVoltage() end},
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
