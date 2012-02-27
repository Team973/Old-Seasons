-- intake.lua

--cheaterroller vertical conveyer up
--driver basic intake
--codriver intake up down
--fire cheaterroller + verticalintake up at slow speed

local wpilib = require("wpilib")

module(...)

local towerState = "stopped"
local lowered = false
local vertSpeed = 0 -- this is vertical converer
local cheatSpeed = 0 -- this is the cheater roller wheels
local frontSpeed = 0 -- this is front intake roller
local sideSpeed = 0 -- this is side intake roller
local isBallBlockingTurret = false
local bigButton = false
local fireButton = false
local intakeButton = false
local motor.verticalConveyer = wpilib.victor(2,4)
local motor.cheaterRoller = wpilib.victor(2,5)
local motor.sideIntake = wpilib.victor(2,1)
local motor.frontIntake = wpilib.victor(2,2)
local pn.Intake = wpilib.solenoid(2)

function towerFire()
	towerState = "fired"
end

function towerRepack()
	towerState = "repacked"
end

function towerUp()
	towerState = "up"
end

function towerDown()
	towerState = "down"

function towerStop()
	towerState = "stopped"

function toggleRaise()
	if bigButton == false then
		pn.Intake:set(false)
	end
    lowered = not lowered
end

function update(turretReady)
    motor.sideIntake:set(sideSpeed)
    motor.frontIntake:set(frontSpeed)
    
    if towerState = "fire" then
	motor.verticalConveyer:set(.3)
	motor.cheaterRoller:set(.3)
    else if towerState = "repack" then
	motor.verticalConveyer:set(-.5)
	motor.cheaterRoller:set(.5)
    else if towerState = "up" then
	motor.verticalConveyer:set(1)
	motor.cheaterRoller:set(1)
    else if towerState = "down" then
	motor.verticalConveyer:set(-1)
	motor.cheaterRoller:set(-1)
    else if towerState = "stop" then
	motor.verticalConveyer:set(0)
	motor.cheaterRoller:set(0)
    else then 
	motor.verticalConveyer:set(0)
	motor.cheaterRoller:set(0)
    end
end

	
end

function setIntake(speed)
	frontSpeed = speed
	sideSpeed = speed
	end
end

fuction intakeStop()
	frontSpeed = 0
	sideSpeed = 0
	towerState = "stopped"
end

-- vim: ft=lua et ts=4 sts=4 sw=4
