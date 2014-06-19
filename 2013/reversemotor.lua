-- reversemotor.lua

local setmetatable = setmetatable

module(...)

local SpeedController = {}

function SpeedController:New(controller)
    local newController = {controller=controller, value=0.0}
    setmetatable(newController, {__index=self})
    return newController
end

function SpeedController:Get()
    return self.value
end

function SpeedController:Set(x)
    self.value = x
    self.controller:Set(-x)
end

function wrap(motor)
    return SpeedController:New(motor)
end
