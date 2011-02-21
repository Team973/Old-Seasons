-- arm.lua

local config = require("config")
local math = require("math")
local wpilib = require("wpilib")

module(...)

local motor = config.armMotor
local PID = config.armPID
local wristPID = config.wristPID
local movement = 0
local manual = false
local clawOpen = false
local isForward = true
local presetName = nil
local possessionTimer = nil

local hasTube = false
local wristSpeed = 0
local gripSpeed = 0

function init()
    PID:reset()
    PID:start()
    wristPID:reset()
    wristPID:start()

    presetName = nil
    PID.target = config.armPot:GetVoltage()
    wristPID.target = config.wristPot:GetVoltage()
end

function getManual()
    return manual
end

function setManual(on)
    if manual and not on then
        PID.target = config.armPot:GetVoltage()
        setPreset(nil)
    end
    manual = on
end

function setMovement(delta)
    movement = delta
end

local function updateTarget()
    -- If we don't have a specific preset, don't change the target.
    if not presetName then return end

    -- Look up preset value in config and change PID targets
    if isForward then
        local preset = config.armPresets.forward[presetName]
        if preset.claw ~= nil then
           clawOpen = preset.claw 
        end
        PID.target = preset.arm + config.armPositionForward
        wristPID.target = preset.wrist + config.wristPositionForward
    else
        local preset = config.armPresets.reverse[presetName]
        if preset.claw ~= nil then
           clawOpen = preset.claw 
        end
        PID.target = preset.arm + config.armPositionReverse
        wristPID.target = preset.wrist + config.wristPositionReverse
    end
end

function setForward(f)
    isForward = f
end

function getPreset()
    return presetName
end

function setPreset(preset)
    presetName = preset
    updateTarget()
end

local function voltageToDegrees(voltage, forwardVoltage, reverseVoltage)
    local scale = 180 / (reverseVoltage - forwardVoltage)
    local offset = 90 - scale * forwardVoltage
    return voltage * scale + offset
end

local function getArmAngle()
    return voltageToDegrees(config.armPot:GetVoltage(), config.armPositionForward, config.armPositionReverse)
end

local function getWristAngle()
    return voltageToDegrees(config.wristPot:GetVoltage(), config.wristPositionForward, config.wristPositionReverse)
end

local function calculateFeedForward()
    return config.armDriveBackAmplitude * math.sin(getArmAngle())
end

function setGripMotor(speed)
    gripSpeed = speed
end

function setWristMotor(speed)
    if wristSpeed ~= 0 and speed == 0 then
        wristPID.target = config.wristPot:GetVoltage()
    end
    wristSpeed = speed
end

function openClaw() clawOpen = true end

function closeClaw() clawOpen = false end

function getHasTube() return hasTube end

local function sign(k)
    if k == 0 then
        return 0
    elseif k > 0 then
        return 1
    elseif k < 0 then
        return -1
    end
end
function update()
    local motorOutput

    -- If we don't have a tube, we're running the intake, and we're in one of the approved presets...
    if not hasTube and gripSpeed > 0 and not clawOpen and (presetName == "pickup" or presetName == "slot") then
         if not config.wristIntakeSwitch:Get() then
             if not possessionTimer then
                 -- The limit switch just got activated
                 possessionTimer = wpilib.Timer()
                 possessionTimer:Start()
             elseif possessionTimer:Get() > config.wristIntakeTime then
                 -- We've waited for the set time. We now have a tube.
                 possessionTimer:Stop()
                 possessionTimer = nil
                 hasTube = true
                 setPreset("carry")
            end
        else
            -- Limit switch is off. Reset the timer.
            if possessionTimer then
                possessionTimer:Stop()
                possessionTimer = nil
            end
        end
    elseif hasTube and clawOpen then
        -- The operator pulled the trigger. Let it go. JUST LET IT GO.
        hasTube = false
    end
    -- Primary Joint
    if manual then
        motorOutput = movement
    else
        motorOutput = -PID:update(config.armPot:GetVoltage())
    end
    --Arm Safety    
    local armSafetyVoltageForward = config.armPresets.forward.stow.arm - 0.02
    if config.armPot:GetVoltage() < armSafetyVoltageForward and motorOutput <= 0 then
        motorOutput = 0 
    end
    local armSafetyVoltageReverse = config.armPresets.reverse.stow.arm + 0.02
    if config.armPot:GetVoltage() > armSafetyVoltageReverse and motorOutput >= 0 then
        motorOutput = 0
    end
    motor:Set(motorOutput)
    -- Wrist
    if wristSpeed ~= 0 then
        motorOutput = wristSpeed
    else
        motorOutput = wristPID:update(config.wristPot:GetVoltage())
    end
    config.wristMotor:Set(motorOutput)
    -- Grip
    config.gripMotor:Set(gripSpeed)
    if hasTube and config.wristIntakeSwitch:Get() then
        -- If we're supposedly in possession and the tube is slipping, run intake.
        config.gripMotor:Set(1)
    end
    if clawOpen then
        config.clawPiston:Set(wpilib.Relay_kForward)
    else
        config.clawPiston:Set(wpilib.Relay_kReverse)
    end
    --Wrist Safety
    --Wrist arm angle is from the wrist to the arm. Wrist arm angle is positive for counter-clockwise
    local wristArmAngle = 180 + getArmAngle() - getWristAngle()
    if wristArmAngle >= 180 then
        wristArmAngle = wristArmAngle - 360
    end
    if wristArmAngle <= -180 then
        wristArmAngle = wristArmAngle + 360
    end
    if math.abs(wristArmAngle) < config.wristSafetyAngle and sign(motorOutput) == sign(wristArmAngle) then
       config.wristMotor:Set(0)
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
