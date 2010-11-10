-- kicker.lua

require "config"
require "wpilib"

module(..., package.seeall)

local gateLatch = config.gateLatch
local kickerCylinder = config.kickerCylinder

local state = "cocked"
local timer = wpilib.Timer()
timer:Start()

-- Check whether the kicker is ready to fire.
function isReady()
    return state == "cocked" and timer:Get() >= config.kickerReloadTime
end

function fire()
    if not isReady() then return end
    state = "firing"
    timer:Reset()
    timer:Start()
end

function update()
    local function changeAfter(newState, time)
        if timer:Get() > time then
            state = newState
            timer:Reset()
            timer:Start()
        end
    end
    
    local lcd = wpilib.DriverStationLCD_GetInstance()
    lcd:PrintLine(wpilib.DriverStationLCD_kUser_Line3, state)
    lcd:UpdateLCD()
    
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
