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

-- vim: ft=lua et ts=4 sts=4 sw=4
