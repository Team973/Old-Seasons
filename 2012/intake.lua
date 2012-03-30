-- intake.lua

--cheaterroller vertical conveyer up
--driver basic intake
--codriver intake up down
--fire cheaterroller + verticalintake up at slow speed

local wpilib = require("wpilib")

module(...)

local lowered = false
local frontSpeed = 0 -- front intake roller
local sideSpeed = 0 -- side intake roller
local verticalSpeed = 0
local cheaterSpeed = 0

local verticalConveyer = wpilib.Victor(2,4)
local cheaterRoller = wpilib.Victor(2,5)
local sideIntake = wpilib.Victor(2,1)
local frontIntake = wpilib.Victor(2,2)
local intakeSolenoid = wpilib.Solenoid(2)
local verticalConveyerEncoder = wpilib.Encoder(2,7,2,8)
local conveyorPID = pid.new(0, 0, 0) 

verticalConveyerEncoder:Start()

function setVerticalSpeed(speed)
    verticalSpeed = speed
end

function setCheaterSpeed(speed)
    cheaterSpeed = speed
end

function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end

function update(turretReady)
    local dashboard = wpilib.SmartDashboard_GetInstance()

    sideIntake:Set(sideSpeed)
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)
    
    verticalConveyer:Set(verticalSpeed)
    cheaterRoller:Set(cheaterSpeed)
    dashboard:PutDouble("Vertical Speed", verticalSpeed)
    dashboard:PutDouble("Cheater Speed", cheaterSpeed)
    conveyerPID:update(verticalConveyerEncoder:Get())
    verticalConveyer:Set(conveyerPID.output)
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
