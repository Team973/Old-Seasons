-- fakesolenoid.lua
-- Use relays as solenoids

local wpilib = require "wpilib"
local setmetatable = setmetatable

module(...)

Solenoid = {}

function Solenoid:New(relay)
    s = {relay=relay}
    setmetatable(s, {__index=self})
    return s
end

function Solenoid:Set(b)
    if b then
        self.relay:Set(wpilib.Relay_kForward)
    else
        self.relay:Set(wpilib.Relay_kReverse)
    end
end

function new(slot, channel)
    return Solenoid:New(wpilib.Relay(slot, channel, wpilib.Relay_kBothDirections))
end
