--grabber.lua


--Requires ect. --
local wpilib = require("wpilib")
local config = require("config")

module(...)
------------------


--Varraibles
local movement = 0 
local movementWrist = 0 


--Functions
function setGripMotor(delta)
     movement = delta
end

function setWristMotor(delta)
     movementWrist = (delta * 2)
end



function update() --Function robot runs in teleop/autonomous
	config.grabberWristMotor:Set(movementWrist)
	config.grabberPickUpMotor:Set(movement) 
end







--Vim Stuff
-- vim: ft=lua et ts=4 sts=4 sw=4
