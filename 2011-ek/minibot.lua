-- minibot.lua

local wpilib = require("wpilib")

module(...)

local isReady = false
local deploying = false
local deploymentTimer
local deploymentTimerTime = 110

function startGameTimer()
    deploymentTimer = wpilib.Timer()
    deploymentTimer:Start()
end

function deploymentTimerFinished()
    return deploymentTimer:Get() > deploymentTimerTime
end

function ready()
    isReady = true
end

function unready()
    isReady = false
    deploying = false
end

function toggleReady()
    if isReady then
        unready()
    else
        ready()
    end
end

function getReady()
    return isReady
end

function getFired()
    return deploying
end
    
local holdTimer = nil

function deploy()
    if not isReady then return end
    if not deploying then
        holdTimer = wpilib.Timer()
        holdTimer:Start()
    end
    deploying = true
end

function update(readyMinibotSolenoid, fireMinibotSolenoid)
    if isReady then
        readyMinibotSolenoid:Set(true)
    else
        readyMinibotSolenoid:Set(false)
    end
    fireMinibotSolenoid:Set(deploying)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
