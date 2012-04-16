-- intake.lua

--cheaterroller vertical conveyer up
--driver basic intake
--codriver intake up down
--fire cheaterroller + verticalintake up at slow speed

local math = require("math")
local pid = require("pid")
local wpilib = require("wpilib")

module(...)

local lowered = false
local frontSpeed = 0 -- front intake roller
local sideSpeed = 0 -- side intake roller
local verticalSpeed = 0
local repack = false
local repackTimer = nil

local verticalConveyer = wpilib.Victor(2,4)
local cheaterRoller = wpilib.Victor(2,5)
local sideIntake = wpilib.Victor(2,1)
local frontIntake = wpilib.Victor(2,2)
local intakeSolenoid = wpilib.Solenoid(2)
local verticalConveyerEncoder = wpilib.Encoder(2,7,2,8)
local conveyerPID = pid.new(0, 0, 0) 
local loadBallTimer =  wpilib.Timer()
squishMeter = wpilib.AnalogChannel(5)

local loadBallState = 0
local peak1, peak2, peak3
local lastSquishThresh

local SQUISH_THRESHOLD = 2

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

local loadBallPeaks
local loadBallStateTable = {
    {0.5, true, nil},
    {0.5, false, 1},
    {-0.5, true, nil},
    {-0.5, false, 2},
    {0.5, true, nil},
    {0.5, false, 3},
}

local function runLoadBallState(speed, rising, peak)
    local voltage = squishMeter:GetVoltage()
    local thresh = voltage > SQUISH_THRESHOLD
    verticalConveyer:Set(speed)
    if peak then
        peak = math.max(peak, voltage)
    end
    local shouldAdvance = (rising and not lastSquishThresh and thresh) or (not rising and lastSquishThresh and not thresh)
    lastSquishThresh = thresh
    return shouldAdvance, peak
end

function update(turretReady)
    local dashboard = wpilib.SmartDashboard_GetInstance()
    local autoRepack = verticalSpeed ~= 0 and frontSpeed ~= 0 and squishMeter:GetVoltage() > SQUISH_THRESHOLD and loadBallState == 0 and (not repackTimer or repackTimer:Get() < .5)

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
            if state[3] then
                nextState, loadBallPeaks[state[3]] = runLoadBallState(state[1], state[2], loadBallPeaks[state[3]])
            else
                nextState = runLoadBallState(state[1], state[2])
            end
            if nextState then
                loadBallState = loadBallState + 1
                if loadBallState > #loadBallStateTable then
                    loadBallState = 0
                    loadBallTimer:Stop()
                end
            end
        end
    else 
        verticalConveyer:Set(verticalSpeed)
    end	  

    dashboard:PutDouble("Vertical Speed", verticalSpeed)
    dashboard:PutDouble("Cheater Speed", cheaterRoller:Get())
    dashboard:PutDouble("Squish Meter", squishMeter:GetVoltage())
    dashboard:PutInt("Load Ball State", loadBallState)
    --conveyerPID:update(verticalConveyerEncoder:Get())
    --verticalConveyer:Set(conveyerPID.output)
end

function loadBall()
    if loadBallState <= 0 then 
        loadBallTimer:Start()
        loadBallTimer:Reset()
        loadBallState = 1
        loadBallPeaks = {0, 0, 0}
        lastSquishThresh = squishMeter:GetVoltage() > SQUISH_THRESHOLD
    end
end

function ConveyerUp()
    conveyerPID.target = conveyerPID.target + 12
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
