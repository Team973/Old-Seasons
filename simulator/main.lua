require "TSerial"
require "wpilib"

local simThread
local input = {}, lastData

function love.load()
    simThread = love.thread.newThread("simulator", "simulator.lua")
    simThread:start()
    love.graphics.setCaption("Robot Simulator")
end

function love.update()
    if not simThread then
        return
    end

    simThread:send("input", TSerial.pack(input))

    local message = simThread:demand("heartbeat")
    local heartbeat = TSerial.unpack(message)
    if heartbeat.data then
        lastData = heartbeat.data
    end
end

function love.keypressed(key)
    if key == " " then
        input.enabled = false
    elseif key == "t" then
        input.enabled = true
        input.teleoperated = true
    elseif key == "a" then
        input.enabled = true
        input.teleoperated = false
    end
end

function love.draw()
    for i = 1, 6 do
        local line = wpilib.DriverStationLCD_kUser_Line1 + (i - 1)
        love.graphics.print(lastData.lcd.current[line], 400, 300 + (i - 1) * 25)
    end
end
