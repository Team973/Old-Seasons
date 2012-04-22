-- intake.lua

--cheaterroller vertical conveyer up
--driver basic intake
--codriver intake up down
--fire cheaterroller + verticalintake up at slow speed

local ipairs = ipairs
local type = type

local io = require("io")
local math = require("math")
local linearize = require("linearize")
local pid = require("pid")
local string = require("string")
local wpilib = require("wpilib")

module(...)

local lowered = false
local frontSpeed = 0 -- front intake roller
local sideSpeed = 0 -- side intake roller
local verticalSpeed = 0
local repack = false
local repackTimer = nil
local allowAutoRepack = false

local verticalConveyer = linearize.wrap(wpilib.Victor(2,4))
local cheaterRoller = wpilib.Victor(2,5)
local sideIntake = wpilib.Victor(2,1)
local frontIntake = wpilib.Victor(2,2)
local intakeSolenoid = wpilib.Solenoid(2)
local verticalConveyerEncoder = wpilib.Encoder(2,7,2,8)
local loadBallTimer =  wpilib.Timer()
squishMeter = wpilib.AnalogChannel(5)

local loadBallState = 0
local lastSquishVoltage = 0

local SQUISH_RISE_THRESHOLD = 2.0
local SQUISH_FALL_THRESHOLD = 2.75

verticalConveyerEncoder:Start()

function setVerticalSpeed(speed)
    verticalSpeed = speed
end

function setRepack(val)
    repack = val
end

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

loadBallPeaks = {complete=false, 0, 0}
local loadBallStateTable = {
    {1.0, true, SQUISH_RISE_THRESHOLD, nil},
    {-0.75, false, SQUISH_FALL_THRESHOLD, 1},
    {0.75, true, SQUISH_RISE_THRESHOLD, nil},
    {0.75, false, function() return loadBallPeaks[1] * 0.9 end, 2},
}

local fireCount = 0
local ballLog = io.open("ball-log.csv", "a")
-- Mode,Time,FireCount,Peak1,Peak2...
ballLog:write("on,0,0")
for i = 1, #loadBallPeaks do
    ballLog:write(",0")
end
ballLog:write("\r\n")
ballLog:flush()
local ballTimer = wpilib.Timer()

local function runLoadBallState(speed, rising, threshold, peak)
    local voltage = squishMeter:GetVoltage()
    local lastThresh = lastSquishVoltage > threshold
    local thresh = voltage > threshold
    verticalConveyer:Set(speed)
    if peak then
        peak = math.max(peak, voltage)
    end
    local shouldAdvance = (rising and not lastThresh and thresh) or (not rising and lastThresh and not thresh)
    return shouldAdvance, peak
end

function setAllowAutoRepack(allow)
    allowAutoRepack = allow
end

function update(turretReady)
    local squishVoltage = squishMeter:GetVoltage()
    ballTimer:Start()

    local dashboard = wpilib.SmartDashboard_GetInstance()
    local autoRepack = verticalSpeed ~= 0 and frontSpeed ~= 0 and loadBallState == 0 and ((squishVoltage > SQUISH_RISE_THRESHOLD and not repackTimer) or (repackTimer and repackTimer:Get() < .5)) and allowAutoRepack

    sideIntake:Set(sideSpeed)
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)

    if repack or autoRepack then
        verticalSpeed = -1
        cheaterRoller:Set(1)

        if autoRepack and not repackTimer then
            repackTimer = wpilib.Timer()
            repackTimer:Start()
        end
    elseif math.abs(frontSpeed) > math.abs(verticalSpeed) then
        cheaterRoller:Set(frontSpeed)
    else
        cheaterRoller:Set(verticalSpeed)
    end
    if not autoRepack then
        repackTimer = nil
    end

    if loadBallState > 0 then
        if loadBallTimer:Get() > 3 then
            -- Cutoff
            loadBallState = 0
            loadBallTimer:Stop()
        else
            -- Normal state
            local state = loadBallStateTable[loadBallState]
            local nextState
            local threshold = state[3]
            if type(threshold) == "function" then
                threshold = threshold()
            end
            if state[4] then
                nextState, loadBallPeaks[state[4]] = runLoadBallState(state[1], state[2], threshold, loadBallPeaks[state[4]])
            else
                nextState = runLoadBallState(state[1], state[2], threshold)
            end
            if nextState then
                loadBallState = loadBallState + 1
                if loadBallState > #loadBallStateTable then
                    loadBallState = 0
                    loadBallTimer:Stop()
                    fireCount = fireCount + 1
                    loadBallPeaks.complete = true

                    if wpilib.IsAutonomous() then
                        ballLog:write("auto,")
                    elseif wpilib.IsOperatorControl() then
                        ballLog:write("teleop,")
                    else
                        ballLog:write(",")
                    end
                    ballLog:write(string.format("%.1f,%d", ballTimer:Get(), fireCount))
                    for _, peak in ipairs(loadBallPeaks) do
                        ballLog:write(string.format(",%.5f", peak))
                    end
                    ballLog:write("\r\n")
                    ballLog:flush()
                end
            end
        end
    else
        verticalConveyer:Set(verticalSpeed)
    end

    dashboard:PutDouble("Vertical Speed", verticalSpeed)
    dashboard:PutDouble("Cheater Speed", cheaterRoller:Get())
    dashboard:PutDouble("Squish Meter", squishVoltage)
    dashboard:PutInt("Load Ball State", loadBallState)

    lastSquishVoltage = squishVoltage
end

function loadBall()
    if loadBallState <= 0 then
        loadBallTimer:Start()
        loadBallTimer:Reset()
        loadBallState = 1
        loadBallPeaks.complete = false
        for i = 1, #loadBallPeaks do
            loadBallPeaks[i] = 0
        end
    end
end

function setIntake(speed)
    frontSpeed = speed
    sideSpeed = speed
end

function intakeStop()
    frontSpeed = 0
    sideSpeed = 0
    verticalSpeed = 0
    cheaterSpeed = 0
end

function fullStop()
    verticalConveyer:Set(0.0)
    cheaterRoller:Set(0.0)
    sideIntake:Set(0.0)
    frontIntake:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
