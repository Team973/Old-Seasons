-- intake.lua

local ipairs = ipairs
local type = type
-- most comments are for hodge-podge look carefully!
local io = require("io")
local math = require("math")
local linearize = require("linearize")
local pid = require("pid")
local string = require("string")
local wpilib = require("wpilib")

module(...)

--local lowered = false
local frontSpeed = 0 -- front intake roller
local verticalSpeed = 0

local verticalConveyer = linearize.wrap(wpilib.Victor(2,4))
--local cheaterRoller = wpilib.Victor(2,5)
local frontIntake = wpilib.Victor(2,2)
--local intakeSolenoid = wpilib.Solenoid(2)

function setVerticalSpeed(speed)
    verticalSpeed = speed
end
--[[
function toggleRaise()
    lowered = not lowered
end

function setLowered(val)
    lowered = val
end
--]]
function update(turretReady)
    local dashboard = wpilib.SmartDashboard_GetInstance()

    frontIntake:Set(frontSpeed)
--[[
    intakeSolenoid:Set(lowered)
    if math.abs(frontSpeed) > math.abs(verticalSpeed) then
        cheaterRoller:Set(frontSpeed)
    else
        cheaterRoller:Set(verticalSpeed)
    end
--]]
    verticalConveyer:Set(verticalSpeed)
end

function setIntake(speed)
    frontSpeed = speed
end

function intakeStop()
    frontSpeed = 0
    verticalSpeed = 0
    --cheaterSpeed = 0
end

function fullStop()
    verticalConveyer:Set(0.0)
   -- cheaterRoller:Set(0.0)
    frontIntake:Set(0.0)
end

-- vim: ft=lua et ts=4 sts=4 sw=4
