-- configmode.lua

local arm = require("arm")
local config = require("config")
local controls = require("controls")
local drive = require("drive")
local io = require("io")
local ipairs = ipairs
local lcd = require("lcd")
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
    "armUpwardP",
    "armDownwardP",
}

local armPID_i, armPID_d = 0, 0, 0

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

local storeHorizontal, storePreset

controlMap = {
    -- Joystick 1
    {
        [3] = {down=function() armPID_d = armPID_d + 0.1 end},
        [2] = {down=function()
            if armPID_d >= 0.1 then
                armPID_d = armPID_d - 0.1
            end
        end},
        [6] = {down=function() newValues.armUpwardP = newValues.armUpwardP + 0.1 end},
        [7] = {down=function()
            if newValues.armUpwardP >= 0.1 then
                newValues.armUpwardP = newValues.armUpwardP - 0.1
            end
        end},
        [11] = {down=function() newValues.armDownwardP = newValues.armDownwardP + 0.1 end},
        [10] = {down=function()
            if newValues.armDownwardP >= 0.1 then
                newValues.armDownwardP = newValues.armDownwardP - 0.1
            end
        end},
    },
    -- Joystick 2
    {
        ["y"] = function(axis) arm.setWristMotor(axis) end,
    },
    -- Joystick 3
    {
        ["y"] = function(axis) arm.setMovement(axis) end,
        [3] = {down=function() storePreset("stow") end},
        [6] = {down=function() storeHorizontal("wrist") end},
        [7] = {down=function() storeHorizontal("wrist") end},
        [11] = {down=function() storeHorizontal("arm") end},
        [10] = {down=function() storeHorizontal("arm") end},
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
            isForward = not stick:GetRawButton(1)
        end,
    },
}

function storeHorizontal(joint)
    local key = joint .. "Position"
    if isForward then
        key = key .. "Forward"
    else
        key = key .. "Reverse"
    end
    newValues[key] = arm.getWristVoltage()
end

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
    preset.wrist = arm.getWristVoltage() - wristRefPoint
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
    armPID_i = config.armPID.i
    armPID_d = config.armPID.d
end

function update()
    lcd.print(2, string.format("PUp=%.1f PDown=%.1f", newValues.armUpwardP, newValues.armDownwardP))
    lcd.print(3, string.format("AI=%.1f AD=%.1f", armPID_i, armPID_d))
    lcd.print(4, string.format("WP=%.1f WI=%.1f WD=%.1f", config.wristPID.p, config.wristPID.i, config.wristPID.d))
    lcd.print(5, "")
    lcd.print(6, "")
    lcd.update()
end

function finish()
    do
        local f = io.open("lua/config/override.lua", "w")
        f:write("-- config/override.lua\n")
        f:write("local pid = require(\"pid\")\n")
        f:write("module(...)\n")
        for i, name in ipairs(valueNames) do
            f:write(name .. "=" .. uberTostring(newValues[name]) .. "\n")
        end
        f:write(string.format("armPID = pid.PID:new(%s, %s, %s)\n", newValues.armUpwardP, tostring(armPID_i), tostring(armPID_d)))
        f:write("armPID.min, armPID.max = -1, 1\n")
        f:flush()
        f:close()
    end
    restartRobot()
end
