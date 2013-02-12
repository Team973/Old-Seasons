-- arm.lua

local wpilib = require("wpilib")

module(...)

encoder = wpilib.Encoder(1, 2, true)
motor = wpilib.Talon(8)

encoder:Start()
