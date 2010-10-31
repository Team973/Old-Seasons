-- kicker.lua

require "config"
require "wpilib"

module(..., package.seeall)

local gateLatch = config.gateLatch
local kickerCylinder = config.kickerCylinder
local thirdZoneCylinder = config.thirdZoneCylinder

local state = "cocked"
local timer = wpilib.Timer()

function fire()
    if not isReady() then return end
    state = "firing"
    timer:Reset()
end

-- Check whether the kicker is ready to fire.
function isReady()
    return state == "cocked" and timer:Get() >= config.kickerReloadTime
end

function update()
    local function changeAfter(newState, time)
        if timer:Get() > time then
            state = newState
            timer:Reset()
        end
    end
    
    if state == "cocked" then
        gateLatch:Set(false)
        kickerCylinder:Set(true)
    elseif state == "firing" then
        gateLatch:Set(true)
        kickerCylinder:Set(true)
        changeAfter("return", config.kickerFireTime)
    elseif state == "return" then
        gateLatch:Set(true)
        kickerCylinder:Set(false)
        changeAfter("catch", config.kickerReturnTime)
    elseif state == "catch" then
        gateLatch:Set(false)
        kickerCylinder:Set(false)
        changeAfter("cocked", config.kickerCatchTime)
    else
        -- Failsafe for kicker, just in case the kicker state is set wrong.
        gateLatch:Set(false)
        kickerCylinder:Set(false)
    end
end

-- Change whether third zone cylinder is engaged.
function setThirdZoneEnabled(flag)
    if state ~= "cocked" then return end
    thirdZoneCylinder:Set(flag)
end
