-- linearize.lua

local setmetatable = setmetatable 

module(...)

-- Convert a logical motor speed to a Victor input.
-- Thanks to Austin Schuh on Team 971 for the polynomial approximation.
function linearize(goalSpeed)
    local deadbandValue = 0.082
    if goalSpeed > deadbandValue then
        goalSpeed = goalSpeed - deadbandValue
    elseif goalSpeed < -deadbandValue then
        goalSpeed = goalSpeed + deadbandValue
    else
        goalSpeed = 0
    end
    goalSpeed = goalSpeed / (1 - deadbandValue)

    local goalSpeed2 = goalSpeed * goalSpeed
    local goalSpeed3 = goalSpeed2 * goalSpeed
    local goalSpeed4 = goalSpeed3 * goalSpeed
    local goalSpeed5 = goalSpeed4 * goalSpeed
    local goalSpeed6 = goalSpeed5 * goalSpeed
    local goalSpeed7 = goalSpeed6 * goalSpeed

    -- Constants for the 5th order polynomial
    local victorFitE1 = 0.437239
    local victorFitC1 = -1.56847
    local victorFitA1 = -(125.0 * victorFitE1 + 125.0 * victorFitC1 - 116.0) / 125.0
    
    local answer5thOrder = (victorFitA1 * goalSpeed5 + victorFitC1 * goalSpeed3 + victorFitE1 * goalSpeed)

    -- Constants for the 7th order polynomial
	local victorFitC2 = -5.46889
	local victorFitE2 = 2.24214
	local victorFitG2 = -0.042375
	local victorFitA2 = -(125.0 * (victorFitC2 + victorFitE2 + victorFitG2) - 116.0) / 125.0
	local answer7thOrder = victorFitA2 * goalSpeed7 + victorFitC2 * goalSpeed5 + victorFitE2 * goalSpeed3 + victorFitG2 * goalSpeed

    -- Average the 5th and 7th order polynomials
    local answer = 0.85 * 0.5 * (answer7thOrder + answer5thOrder) + .15 * goalSpeed * (1.0 - deadbandValue)

    if answer > 0.001 then
        answer = answer + deadbandValue
    elseif answer < -0.001 then
        answer = answer - deadbandValue
    end
    
    return answer
end

SpeedController = {}

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
    self.controller:Set(linearize(x))
end

function wrap(victor)
    return SpeedController:New(victor)
end
