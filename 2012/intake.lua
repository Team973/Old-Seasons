-- intake.lua

--cheaterroller vertical conveyer up
--driver basic intake
--codriver intake up down
--fire cheaterroller + verticalintake up at slow speed

local wpilib = require("wpilib")

module(...)

local towerState = "stop"
local lowered = false
local frontSpeed = 0 -- front intake roller
local sideSpeed = 0 -- side intake roller

local verticalConveyer = wpilib.Victor(2,4)
local cheaterRoller = wpilib.Victor(2,5)
local sideIntake = wpilib.Victor(2,1)
local frontIntake = wpilib.Victor(2,2)
local intakeSolenoid = wpilib.Solenoid(2)

function towerFire()
	towerState = "fire"
end

function towerRepack()
	towerState = "repack"
end

function towerUp()
	towerState = "up"
end

function towerDown()
	towerState = "down"
end

function towerStop()
	towerState = "stop"
end

function toggleRaise()
    lowered = not lowered
end

function update(turretReady)
    sideIntake:Set(sideSpeed)
    frontIntake:Set(frontSpeed)

    intakeSolenoid:Set(lowered)
    
    if towerState == "fire" then
        verticalConveyer:Set(.3)
        cheaterRoller:Set(.3)
    elseif towerState == "repack" then
        verticalConveyer:Set(-.5)
        cheaterRoller:Set(.5)
    elseif towerState == "up" then
        verticalConveyer:Set(1)
        cheaterRoller:Set(1)
    elseif towerState == "down" then
        verticalConveyer:Set(-1)
        cheaterRoller:Set(-1)
    elseif towerState == "stop" then
        verticalConveyer:Set(0)
        cheaterRoller:Set(0)
    else
        verticalConveyer:Set(0)
        cheaterRoller:Set(0)
    end
end

function setIntake(speed)
	frontSpeed = speed
	sideSpeed = speed
end

function intakeStop()
	frontSpeed = 0
	sideSpeed = 0
	towerState = "stop"
end

-- vim: ft=lua et ts=4 sts=4 sw=4
