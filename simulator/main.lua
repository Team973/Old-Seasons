require "TSerial"
require "wpilib"

local simThread
local lastData
local input = {
    enabled = false,
    teleoperated = true,
}

-- Constants
local screenW, screenH
local lcdX, lcdY = 550, 450
local lcdFontSize = 12
local lcdFont

function love.load()
    simThread = love.thread.newThread("simulator", "simulator.lua")
    simThread:start()

    screenW = love.graphics.getWidth()
    screenH = love.graphics.getHeight()
    lcdFont = love.graphics.newFont(lcdFontSize)
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
    -- Field state
    if input.enabled then
        love.graphics.setColor(0, 255, 0, 255)
    else
        love.graphics.setColor(255, 0, 0, 255)
    end
    if input.teleoperated then
        love.graphics.print("Teleoperated", 0, 0)
    else
        love.graphics.print("Autonomous", 0, 0)
    end

    -- LCD
    love.graphics.setColor(255, 255, 255, 255)
    love.graphics.rectangle("fill", lcdX, lcdY, screenW - lcdX, screenH - lcdY)

    local lcdMessage = ""
    for line = wpilib.DriverStationLCD_kUser_Line1, wpilib.DriverStationLCD_kUser_Line6 do
        if line > wpilib.DriverStationLCD_kUser_Line1 then
            lcdMessage = lcdMessage .. "\n"
        end
        lcdMessage = lcdMessage .. lastData.lcd.current[line]
    end
    love.graphics.setColor(0, 0, 0, 255)
    love.graphics.setFont(lcdFont)
    love.graphics.printf(lcdMessage, lcdX, lcdY, screenW - lcdX)

    -- Motors
    love.graphics.setColor(255, 255, 255, 255)
    love.graphics.print(string.format("%.1f %.1f", lastData.pwms[4][7], lastData.pwms[4][8]), 25, 100)
    love.graphics.print(string.format("%.1f %.1f", lastData.pwms[4][1], lastData.pwms[4][2]), 125, 100)
    love.graphics.print(string.format("%.1f %.1f", lastData.pwms[4][6], lastData.pwms[4][5]), 25, 200)
    love.graphics.print(string.format("%.1f %.1f", lastData.pwms[4][3], lastData.pwms[4][4]), 125, 200)
end
