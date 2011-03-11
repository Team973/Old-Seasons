-- arm.lua

local config = require("config")
local math = require("math")
local ipairs = ipairs
local string = require("string")
local tostring = tostring
local wpilib = require("wpilib")

module(...)

local motor = config.armMotor
local PID = config.armPID
local wristPID = config.wristPID
local movement = 0
local manual = false
local clawState = -1 -- 0 for closed, 1 for open, -1 for neutral
local isForward = true
local presetName = nil
local possessionTimer = nil
local safety = true
local hasTube = false
local wristSpeed = 0
local gripSpeed = 0

local function voltageToDegrees(voltage, forwardVoltage, reverseVoltage)
    local scale = 180 / (reverseVoltage - forwardVoltage)
    local offset = 90 - scale * forwardVoltage
    return voltage * scale + offset
end

local function degreesToVoltage(degrees, forwardVoltage, reverseVoltage)
    local scale = (reverseVoltage - forwardVoltage) / 180
    local offset = forwardVoltage - scale * 90
    return degrees * scale + offset
end

function getArmVoltage()
    return config.armPot:GetVoltage()
end

local function getArmAngle()
    return voltageToDegrees(getArmVoltage(), config.armPositionForward, config.armPositionReverse)
end

function getWristVoltage()
    local voltage = config.wristPot:GetVoltage()
    if config.flipWristPot then
        voltage = 5 - voltage
    end
    return voltage
end

local function getWristAngle()
    return voltageToDegrees(getWristVoltage(), config.wristPositionForward, config.wristPositionReverse)
end

function init()
    PID:reset()
    PID:start()
    wristPID:reset()
    wristPID:start()

    presetName = nil
    PID.target = getArmVoltage()
    wristPID.target = getWristVoltage()
end

function getManual()
    return manual
end

function setManual(on)
    if manual and not on then
        PID.target = getArmVoltage()
        setPreset(nil)
    end
    manual = on
end

function setMovement(delta)
    movement = delta
end

function setSafety(on)
    safety = on
end

local function updateTarget()
    -- If we don't have a specific preset, don't change the target.
    if not presetName then return end

    -- Look up preset value in config and change PID targets
    if isForward then
        local preset = config.armPresets.forward[presetName]
        if preset.claw ~= nil then
           clawState = preset.claw 
        end
        PID.target = preset.arm + config.armPositionForward
        wristPID.target = preset.wrist + config.wristPositionForward
    else
        local preset = config.armPresets.reverse[presetName]
        if preset.claw ~= nil then
           clawState = preset.claw 
        end
        PID.target = preset.arm + config.armPositionReverse
        wristPID.target = preset.wrist + config.wristPositionReverse
    end
end

function setForward(f)
    isForward = f
end

function getForward()
    return isForward 
end

function getPreset()
    return presetName
end

function setPreset(preset)
    presetName = preset
    updateTarget()
end

local function calculateFeedForward()
    return config.armDriveBackAmplitude * math.sin(getArmAngle())
end

function runArmBack()
    setPreset(nil)
    local armAngle = getArmAngle()
    local armSin = math.sin(math.rad(armAngle - 90))
    local targetAngle = math.deg(math.asin(armSin - config.armRunDelta / config.armLength))
    if armAngle <= 180 then
        targetAngle = 90 + targetAngle
    else
        targetAngle = 270 - targetAngle
    end
    -- If this is physically possible, then update the target.
    -- (NaN ~= NaN in Lua)
    if targetAngle == targetAngle then
        PID.target = degreesToVoltage(targetAngle, config.armPositionForward, config.armPositionReverse)
    end
end

function runWristHorizontal()
    setPreset(nil)
    if isForward then
        wristPID.target = config.wristPositionForward
    else
        wristPID.target = config.wristPositionReverse
    end
end

function setGripMotor(speed)
    gripSpeed = speed
end

function setWristMotor(speed)
    if wristSpeed ~= 0 and speed == 0 then
        wristPID.target = getWristVoltage()
    end
    wristSpeed = speed
end

function openClaw() clawState = 1 end
function closeClaw() clawState = 0 end
function releaseClaw() clawState = -1 end

function getGripperState()
    return {clawNumb=clawState,gripNumb=gripSpeed}
end

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

local function updateArmP()
    local armIsForward = (getArmAngle() < 180)
    local positiveError = (getArmVoltage() < PID.target)
    if armIsForward == positiveError then
        PID.p = config.armUpwardP
    else
        PID.p = config.armDownwardP
    end
end

local function isAnalogConnected(channel)
    return channel:GetVoltage() >= 0.1
end

function update()
    local motorOutput
    
    updateArmP()

    -- If we don't have a tube, we're running the intake, and we're in one of the approved presets...
    if not hasTube and gripSpeed > 0 and clawState == -1 and (presetName == "pickup" or presetName == "slot") then
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
                 closeClaw()
            end
        else
            -- Limit switch is off. Reset the timer.
            if possessionTimer then
                possessionTimer:Stop()
                possessionTimer = nil
            end
        end
    elseif hasTube and clawState == 1 then
        -- The operator pulled the trigger. Let it go. JUST LET IT GO.
        hasTube = false
    end
    -- Primary Joint
    if manual then
        motorOutput = movement
    elseif isAnalogConnected(config.armPot) then
        motorOutput = -PID:update(getArmVoltage())
    else 
        motorOutput = 0
        safety = false
    end
    -- Arm Safety
    local armSafetyVoltageForward = config.armPositionForward + config.armPresets.forward.stow.arm - 0.02
    if safety and getArmVoltage() < armSafetyVoltageForward and motorOutput >= 0 then
        -- Only allow the operator to go CCW (negative)
        motorOutput = 0
    end
    local armSafetyVoltageReverse = config.armPositionReverse + config.armPresets.reverse.stow.arm + 0.02
    if safety and getArmVoltage() > armSafetyVoltageReverse and motorOutput <= 0 then
        -- Only allow the operator to go CW (positive)
        motorOutput = 0
    end

    motor:Set(motorOutput)
    -- Wrist
    if wristSpeed ~= 0 then
        motorOutput = wristSpeed
    elseif isAnalogConnected(config.armPot) and isAnalogConnected(config.wristPot) then
        motorOutput = wristPID:update(getWristVoltage())
    else
        motorOutput = 0
        safety = false
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
    if safety and math.abs(wristArmAngle) < config.wristSafetyAngle and sign(motorOutput) == sign(wristArmAngle) then
       motorOutput = 0
    end

    if config.flipWrist then
        motorOutput = -motorOutput
    end
    config.wristMotor:Set(motorOutput)
    -- Grip
    config.gripMotor:Set(gripSpeed)
    if hasTube and config.wristIntakeSwitch:Get() then
        -- If we're supposedly in possession and the tube is slipping, run intake.
        config.gripMotor:Set(1)
    end
    if config.clawSolenoids then
        -- Pro setup
        if clawState == 1 then
            -- Open
            config.clawOpenPiston:Set(true)
            config.clawClosePiston:Set(false)
        elseif clawState == 0 then
            -- Closed
            config.clawOpenPiston:Set(false)
            config.clawClosePiston:Set(false)
        else
            -- Neutral
            config.clawOpenPiston:Set(false)
            config.clawClosePiston:Set(true)
        end
    else
        -- Black Knight crap setup
        if clawState == 1 then
            config.clawPiston:Set(wpilib.Relay_kForward)
        else
            config.clawPiston:Set(wpilib.Relay_kReverse)
        end
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
