--[[
pid.lua - PID loops!

Useful indexes of a PID table:
    p, i, and d: The constants of the loop
    min and max: Limit the output (nil represents no limit)
    target: The desired value of the dependent variable
    output (read-only): The calculated output of the manipulated variable

There are other indexes, but you shouldn't touch them.
--]]

local wpilib = require "wpilib"

module(...)

PID = {}

function PID:new(p, i, d)
    local pidObj = {p=p or 0, i=i or 0, d=d or 0}
    setmetatable(pidObj, {__index=self})
    return pidObj
end

--[[
Reset the PID controller.

This will wipe most all values from the PID controller, so use with
extreme caution.  The values affected are:
    - the previous error sample
    - the integral accumulator
    - the target
    - the output
    - the timer

All of the values mentioned above except the timer will be set to zero.
The timer will be reset and stopped.

It is important to note that the PID constants will not be altered by
this method.
--]]
function PID:reset()
    self.previousError = 0
    self.integral = 0
    self.target = 0
    self.output = 0
    self.timer = wpilib.Timer()
end

-- Start the PID loop's internal timer
function PID:start()
    self.timer:Start()
    self.timer:Reset()
end

-- Stop the PID loop's internal timer
function PID:stop()
    self.timer:Stop()
end

--[[
Update the PID controller.

The computation involves finding the difference between the input and the
target, the difference from last time, and the integral of the difference.

If a minimum or maximum was set on the PID controller, then any output
calculated will be clipped into the range given.

If you omit the time parameter, then the internal timer will be used.  If you
do this, then you must call start() first.

The function returns the output value.
--]]
function PID:update(actual, time)
    if not time then
        time = self.timer:Get()
        self.timer:Reset()
    end
    
    if time <= 0 then time = 0.001 end
    
    -- Calculate error
    local e = self.target - actual
    
    -- Calculate integral
    self.integral = self.integral + e * time
    
    -- Calculate derivative
    local derivative = (e - self.previousError) / time
    
    -- Compute output
    self.output = (self.p * e) + (self.i * self.integral) + (self.d * derivative)
    self.previousError = e
    
    if self.max and self.output > self.max then
        self.output = self.max
    end
    if self.min and self.output < self.min then
        self.output = self.min
    end
    
    return self.output
end

-- vim: ft=lua et ts=4 sts=4 sw=4
