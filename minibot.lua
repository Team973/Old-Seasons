-- minibot.lua

local config = require("config")
local wpilib = require("wpilib")

module(...)

local readying = false
local isReady = false
local deploying = false
local readyTimer
local readyDelay = 0.5

function ready()
    if isReady then return end
    readying = true
end

function unready()
    readying = false
    isReady = false
    deploying = false
end

function toggleReady()
    if isReady or readying then
        unready()
    else
        ready()
    end
end

function getReady()
    return isReady
end
    
function deploy()
    if not isReady then return end
    deploying = true
end

function update()
    if isReady then
        config.readyMinibotSolenoid1:Set(true)
        config.readyMinibotSolenoid2:Set(true)
    elseif readying then
        config.readyMinibotSolenoid1:Set(true)
        config.readyMinibotSolenoid2:Set(false)
        if readyTimer == nil then
            readyTimer = wpilib.Timer()
            readyTimer:Start()
        elseif readyTimer:Get() > readyDelay then
            isReady = true
        end
    else
        config.readyMinibotSolenoid1:Set(false)
        config.readyMinibotSolenoid2:Set(false)
    end
    config.fireMinibotSolenoid:Set(deploying)
end
