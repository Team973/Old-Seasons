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

local verticalConveyer = wpilib.Victor(2,4)
local cheaterRoller = wpilib.Victor(2,5)
local sideIntake = wpilib.Victor(2,1)
local frontIntake = wpilib.Victor(2,2)
local intakeSolenoid = wpilib.Solenoid(2)
local verticalConveyerEncoder = wpilib.Encoder(2,7,2,8)
local conveyerPID = pid.new(0, 0, 0) 
local loadBallTimer =  wpilib.Timer()
squishMeter = wpilib.AnalogChannel(5)

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

local isLoadingBall = false

function update(turretReady)
    local dashboard = wpilib.SmartDashboard_GetInstance()

    sideIntake:Set(sideSpeed)
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)
    if repack then
        verticalSpeed = -1
        cheaterRoller:Set(1)
    elseif math.abs(frontSpeed) > math.abs(verticalSpeed) then
        cheaterRoller:Set(frontSpeed)
    else
        cheaterRoller:Set(verticalSpeed)
    end
    
    if isLoadingBall then 
        verticalConveyer:Set(1) 
        loadBallTimer:Start()
        if loadBallTimer:Get() > 3 or squishMeterOutput() > squishMeterOutput() * .9 then
            isLoadingBall = false
        end
    else 
        verticalConveyer:Set(verticalSpeed)
    end

    verticalConveyer:Set(verticalSpeed)
    dashboard:PutDouble("Vertical Speed", verticalSpeed)
    dashboard:PutDouble("Cheater Speed", cheaterRoller:Get())
    dashboard:PutDouble("Squish Meter", squishMeter:GetVoltage())
    --conveyerPID:update(verticalConveyerEncoder:Get())
    --verticalConveyer:Set(conveyerPID.output)
end

function loadBall()
    if not isLoadingBall then 
        loadBallTimer:Reset()
        isLoadingBall = true
    end
end

local lastPeak = 0 
function squishMeterOutput() 
    local voltage = squishMeter:GetVoltage() 
    if voltage > lastPeak then
        lastPeak = voltage
    elseif voltage < lastPeak * .25 then
        lastPeak = voltage
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
