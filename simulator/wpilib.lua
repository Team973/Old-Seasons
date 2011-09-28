-- mock WPILib

local coroutine = require("coroutine")

module(...)

function Wait(time)
    print("WAITING")
    coroutine.yield()
end
