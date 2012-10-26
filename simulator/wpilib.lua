-- mock WPILib

require "love.joystick"
require "love.thread"
require "love.timer"
require "TSerial"

local coroutine = coroutine
local love = love
local os = os
local pairs = pairs
local print = print
local string = string
local TSerial = TSerial

module(...)

local IN, OUT = {}, {}

function Wait(seconds)
    -- Send heartbeat
    local t = love.thread.getThread()
    t:send("heartbeat", TSerial.pack({data=OUT}))

    -- Receive input (if available)
    local input = t:receive("input")
    if input then
        IN = TSerial.unpack(input)
    end

    -- Do sleep
    love.timer.sleep(seconds * 1000)
end

local function realJoystick(port)
    port = port - 1 -- LOVE is 0-based
    love.joystick.open(port)
    return {
        GetX=function(self) return love.joystick.getAxis(port, 0) end,
        GetY=function(self) return love.joystick.getAxis(port, 1) end,
        GetZ=function(self) return love.joystick.getAxis(port, 2) end,
        GetTwist=function(self) return 0 end,
        GetThrottle=function(self) return 0 end,
        GetRawAxis=function(self, axis) return love.joystick.getAxis(port, axis - 1) end,
        GetTrigger=function(self) return love.joystick.isDown(port, 0) end,
        GetTop=function(self) return love.joystick.isDown(port, 1) end,
        GetBumper=function(self) return false end,
        GetButton=function(self) return false end,
        GetRawButton=function(self, button) return love.joystick.isDown(port, button - 1) end,
        GetMagnitude=function(self) return 0.0 end,
        GetDirectionRadians=function(self) return 0.0 end,
        GetDirectionDegrees=function(self) return 0.0 end,
    }
end

local function mockJoystick(port)
    return {
        GetX=function(self) return 0 end,
        GetY=function(self) return 0 end,
        GetZ=function(self) return 0 end,
        GetTwist=function(self) return 0 end,
        GetThrottle=function(self) return 0 end,
        GetRawAxis=function(self, axis) return 0 end,
        GetTrigger=function(self) return false end,
        GetTop=function(self) return false end,
        GetBumper=function(self) return false end,
        GetButton=function(self) return false end,
        GetRawButton=function(self, button) return false end,
        GetMagnitude=function(self) return 0.0 end,
        GetDirectionRadians=function(self) return 0.0 end,
        GetDirectionDegrees=function(self) return 0.0 end,
    }
end

function Joystick(port)
    if port <= love.joystick.getNumJoysticks() then
        return realJoystick(port)
    else
        return mockJoystick(port)
    end
end

DriverStationLCD_kLineLength = 21
DriverStationLCD_kNumLines = 6
DriverStationLCD_kMain_Line6 = 0
DriverStationLCD_kUser_Line1 = 0
DriverStationLCD_kUser_Line2 = 1
DriverStationLCD_kUser_Line3 = 2
DriverStationLCD_kUser_Line4 = 3
DriverStationLCD_kUser_Line5 = 4
DriverStationLCD_kUser_Line6 = 5

local blankLine = string.rep(" ", DriverStationLCD_kLineLength)
local function blankLCD()
    return {
        [DriverStationLCD_kUser_Line1]=blankLine,
        [DriverStationLCD_kUser_Line2]=blankLine,
        [DriverStationLCD_kUser_Line3]=blankLine,
        [DriverStationLCD_kUser_Line4]=blankLine,
        [DriverStationLCD_kUser_Line5]=blankLine,
        [DriverStationLCD_kUser_Line6]=blankLine,
    }
end

OUT.lcd = {current = blankLCD(), new = blankLCD()}

function DriverStationLCD_GetInstance()
    return {
        UpdateLCD = function(self)
            -- TODO: Swap tables instead of using more memory
            OUT.lcd.current, OUT.lcd.new = OUT.lcd.new, {}
            for line, data in pairs(OUT.lcd.current) do
                OUT.lcd.new[line] = data
            end
        end,
        Clear = function(self)
            DriverStationLCD_NewData = blankLCD()
        end,
        Print = function(self, line, col, s)
            local curr = OUT.lcd.new[line]
            OUT.lcd.new[line] = string.sub(curr, 1, col - 1) ..
                    string.sub(s, 1, DriverStationLCD_kLineLength - (col - 1)) ..
                    string.sub(curr, col + #s)
        end,
        PrintLine = function(self, line, s)
            OUT.lcd.new[line] = string.sub(s, 1, DriverStationLCD_kLineLength) ..
                    string.rep(" ", DriverStationLCD_kLineLength - #s)
        end,
    }
end

OUT.relays = {}

Relay_kOff = 0
Relay_kOn = 1
Relay_kForward = 2
Relay_kReverse = 3

Relay_kBothDirections = 0
Relay_kForwardOnly = 1
Relay_kReverseOnly = 2

function Relay(slot, channel, direction)
    if not OUT.relays[slot] then OUT.relays[slot] = {} end
    OUT.relays[slot][channel] = Relay_kOff
    return {
        Set=function(self, value)
            OUT.relays[slot][channel] = value
        end,
        SetDirection=function(self, direction)
        end,
    }
end

OUT.pwms = {}
function SpeedController(slot, channel)
    if not channel then
        slot, channel = 4, channel
    end
    if not OUT.pwms[slot] then OUT.pwms[slot] = {} end
    OUT.pwms[slot][channel] = 0.0
    return {
        Get=function(self)
            return OUT.pwms[slot][channel]
        end,
        Set=function(self, value)
            if value > 1.0 then
                value = 1.0
            elseif value < -1.0 then
                value = -1.0
            end
            OUT.pwms[slot][channel] = value
        end,
    }
end

Jaguar = SpeedController
Victor = SpeedController

OUT.din = {}
function DigitalInput(slot, channel)
    if not channel then
        slot, channel = 4, channel
    end
    if not OUT.din[slot] then OUT.din[slot] = {} end
    OUT.din[slot][channel] = false
    return {
        Get=function(self)
            return OUT.din[slot][channel]
        end,
        GetInt=function(self)
            if OUT.din[slot][channel] then
                return 1
            else
                return 0
            end
        end,
    }
end

OUT.encoders = {}
function Encoder(...)
    -- TODO
    return {
        Start=function(self) end,
        Get=function(self) return 0 end,
        GetRaw=function(self) return 0 end,
        Reset=function(self) end,
        Stop=function(self) end,
        GetPeriod=function(self) return 0.0 end,
        SetMaxPeriod=function(self, maxPeriod) end,
        GetStopped=function(self) return false end,
        GetDirection=function(self) return false end,
        GetDistance=function(self) return 0.0 end,
        GetRate=function(self) return 0.0 end,
        SetMinRate=function(self, minRate) end,
        SetDistancePerPulse=function(self, distancePerPulse) end,
        SetReverseDirection=function(self, reverseDirection) end,
    }
end

OUT.solenoids = {}
function Solenoid(slot, channel)
    if not channel then
        slot, channel = 8, channel
    end
    if not OUT.solenoids[slot] then OUT.solenoids[slot] = {} end
    OUT.solenoids[slot][channel] = false
    return {
        Get=function(self)
            return OUT.solenoids[slot][channel]
        end,
        Set=function(self, on)
            OUT.solenoids[slot][channel] = on
        end,
    }
end

function Timer()
    local t = os.time()
    return {
        Get=function(self) return os.time() - t end,
        Reset=function(self) t = os.time() end,
        Start=function(self) end,
        Stop=function(self) end,
        HasPeriodPassed=function(self, period)
            local elapsed = os.time() - t
            if period < elapsed then
                -- TODO: Probably need to reset or something.
                return true
            else
                return false
            end
        end,
    }
end

function GetWatchdog()
    return {
        -- TODO: Figure out what Feed *should* return
        Feed=function(self) return false end,

        Kill=function(self) end,
        GetTimer=function(self) return 0.0 end,
        GetExpiration=function(self) return 0.0 end,
        SetExpiration=function(self, value) end,
        GetEnabled=function(self) return false end,
        SetEnabled=function(self) end,
        IsAlive=function(self) return true end,
        IsSystemActive=function(self) return true end,
    }
end

IN.enabled = false
IN.teleoperated = true

function IsEnabled()
    return IN.enabled
end

function IsDisabled()
    return not IN.enabled
end

function IsAutonomous()
    return not IN.teleoperated
end

function IsOperatorControl()
    return IN.teleoperated
end
