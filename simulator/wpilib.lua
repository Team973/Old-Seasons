-- mock WPILib

require("love.joystick")

local coroutine = coroutine
local love = love
local os = os
local pairs = pairs
local print = print
local string = string

module(...)

DATA = {}

function Wait(time)
    print("WAITING")
    coroutine.yield()
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

DATA.lcd = {current = blankLCD(), new = blankLCD()}

function DriverStationLCD_GetInstance()
    return {
        UpdateLCD = function(self)
            -- TODO: Swap tables instead of using more memory
            DATA.lcd.current, DATA.lcd.new = DATA.lcd.new, {}
            for line, data in pairs(DATA.lcd.current) do
                DATA.lcd.new[line] = data
            end
        end,
        Clear = function(self)
            DriverStationLCD_NewData = blankLCD()
        end,
        Print = function(self, line, col, s)
            local curr = DATA.lcd.new[line]
            DATA.lcd.new[line] = string.sub(curr, 1, col - 1) ..
                    string.sub(s, 1, DriverStationLCD_kLineLength - (col - 1)) ..
                    string.sub(curr, col + #s)
        end,
        PrintLine = function(self, line, s)
            DATA.lcd.new[line] = string.sub(s, 1, DriverStationLCD_kLineLength) ..
                    string.rep(" ", DriverStationLCD_kLineLength - #s)
        end,
    }
end

DATA.relays = {}

Relay_kOff = 0
Relay_kOn = 1
Relay_kForward = 2
Relay_kReverse = 3

Relay_kBothDirections = 0
Relay_kForwardOnly = 1
Relay_kReverseOnly = 2

function Relay(slot, channel, direction)
    if not DATA.relays[slot] then DATA.relays[slot] = {} end
    DATA.relays[slot][channel] = Relay_kOff
    return {
        Set=function(self, value)
            DATA.relays[slot][channel] = value
        end,
        SetDirection=function(self, direction)
        end,
    }
end

DATA.pwms = {}
function SpeedController(slot, channel)
    if not channel then
        slot, channel = 4, channel
    end
    if not DATA.pwms[slot] then DATA.pwms[slot] = {} end
    DATA.pwms[slot][channel] = 0.0
    return {
        Get=function(self)
            return DATA.pwms[slot][channel]
        end,
        Set=function(self, value)
            DATA.pwms[slot][channel] = value
        end,
    }
end

Jaguar = SpeedController
Victor = SpeedController

DATA.din = {}
function DigitalInput(slot, channel)
    if not channel then
        slot, channel = 4, channel
    end
    if not DATA.din[slot] then DATA.din[slot] = {} end
    DATA.din[slot][channel] = false
    return {
        Get=function(self)
            return DATA.din[slot][channel]
        end,
        GetInt=function(self)
            if DATA.din[slot][channel] then
                return 1
            else
                return 0
            end
        end,
    }
end

DATA.encoders = {}
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

DATA.solenoids = {}
function Solenoid(slot, channel)
    if not channel then
        slot, channel = 8, channel
    end
    if not DATA.solenoids[slot] then DATA.solenoids[slot] = {} end
    DATA.solenoids[slot][channel] = false
    return {
        Get=function(self)
            return DATA.solenoids[slot][channel]
        end,
        Set=function(self, on)
            DATA.solenoids[slot][channel] = on
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

DATA.enabled = false
DATA.teleoperated = true

function IsEnabled()
    return DATA.enabled
end

function IsDisabled()
    return not DATA.enabled
end

function IsAutonomous()
    return not DATA.teleoperated
end

function IsOperatorControl()
    return DATA.teleoperated
end
