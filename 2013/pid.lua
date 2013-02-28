--[[
pid.lua - PID loops!

Useful indexes of a PID table:
    p, i, and d: The constants of the loop
    min and max: Limit the output (nil represents no limit)
    icap: Limit the contribution of K_i * (integral of error)
          (must be positive, nil represents no limit)
    target: The desired value of the dependent variable
    errFunc: Function used to calculate the error. Takes two arguments: actual
             and target. Defaults to the difference of target from actual.
    output (read-only): The calculated output of the manipulated variable
    timerNew: A function to create a timer.

There are other indexes, but you shouldn't touch them.
--]]

local setmetatable = setmetatable

module(...)

PID = {timerNew=function() return nil end}

local function defaultError(actual, target)
    return target - actual
end

local function defaultDerivative(err, prevErr, time)
    return (err - prevErr) / time
end

function PID:new(p, i, d, errFunc, derivFunc)
    local pidObj = {p=p or 0, i=i or 0, d=d or 0,
                    errFunc=errFunc or defaultError,
                    derivFunc=derivFunc or defaultDerivative}
    setmetatable(pidObj, {__index=self})
    pidObj:reset()
    return pidObj
end

function new(...)
    return PID:new(...)
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
    self.timer = self.timerNew()
end

-- Start the PID loop's internal timer
function PID:start()
    if self.timer then
        self.timer:Start()
        self.timer:Reset()
    end
end

-- Stop the PID loop's internal timer
function PID:stop()
    if self.timer then
        self.timer:Stop()
    end
end

--[[
Update the PID controller.

The computation involves finding the error between the input and the target,
the error from last time, and the integral of the error.

If a minimum or maximum was set on the PID controller, then any output
calculated will be clipped into the range given.

If you omit the time parameter, then the internal timer will be used.  If you
do this, then you must call start() first.

The function returns the output value.
--]]
function PID:update(actual, time)
    if not time then
        if self.timer then
            time = self.timer:Get()
            self.timer:Reset()
        else
            error("no timer provided")
        end
    end

    if time <= 0 then time = 0.001 end

    -- Calculate error
    local e = self.errFunc(actual, self.target)

    -- Calculate integral
    self.integral = self.integral + e * time
    local iterm = self.i * self.integral
    if self.icap then
        if iterm > self.icap then
            iterm = self.icap
        elseif iterm < -self.icap then
            iterm = -self.icap
        end
    end

    -- Calculate derivative
    local derivative = self.derivFunc(e, self.previousError, time)

    -- Compute output
    self.output = (self.p * e) + iterm + (self.d * derivative)
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
