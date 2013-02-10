-- arm.lua

local wpilib = require("wpilib")

module(...)

local encoder = wpilib.Encoder(1, 2)
local motor = wpilib.Talon(8)
