-- robot.lua

local pid = require("pid")
local wpilib = require("wpilib")
-- Inject WPILib timer object into PID
pid.PID.timerNew = wpilib.Timer
wpilib.SmartDashboard_init()

local arm = require("arm")
local controls = require("controls")
local drive = require("drive")
local intake = require("intake")
local math = require("math")
local shooter = require("shooter")

local pairs = pairs
local tostring = tostring

module(..., package.seeall)

local TELEOP_LOOP_LAG = 0.005
local AUTO_LOOP_LAG = 0.005 * 1.50

-- Declarations
local watchdogEnabled = false
local feedWatchdog, enableWatchdog, disableWatchdog

local disabledIdle, autonomous, teleop, updateCompressor
local controlMap
local deployStinger
local compressor, pressureSwitch, pressureTransducer, autoDriveSwitch, stinger
local hangingPin, hangDeployOn, hangDeployOff
local driveX, driveY, quickTurn = 0, 0, false
local prepareHang, hanging = false, false
local deployIntake = false
local currTheta, prevTheta = 0, 0
local gyroCurr
local prevGyro = 0
local leftPrev = 0
local rightPrev = 0
local currX, currY = 0, 0
local driveError, angleError = 0, 0
local drivePercision = 6
local turnPercision = 5
local velocityPercision = 0
local leftCurr, rightCurr = 0, 0
local deltaX, deltaY = 0, 0
local magnitude = 0
local targetX, targetY = 0, 0

-- STATES
local state = nil
local FIRE = "fire"
local HUMAN_LOAD = "human_load"
local STOW = "stow"
local INTAKE_LOAD = "intake_load"

-- End Declarations

-- Auto PID
local drivePID = pid.new(.3, 0, 0)
drivePID.min, drivePID.max = -.5, .5
local anglePID = pid.new(0.1, 0, 0)
anglePID.min, anglePID.max = -.3, .3
local rotatePID = pid.new(0.1, 0, 0)
rotatePID.min, rotatePID.max = -.5, .5
drivePID:start()
anglePID:start()
rotatePID:start()

function getDriveTarget()
    return drivePID.target
end

function setDriveTarget(target)
    drivePID.target = target
end

function getAngleTarget()
    return anglePID.target
end

function setAngleTarget(target)
    anglePID.target = target
end

function run()
    --local lw = wpilib.LiveWindow_GetInstance()
    --lw:SetEnabled(false)

    local ds = wpilib.DriverStation_GetInstance()

    -- Main loop
    while true do
        if wpilib.IsDisabled() then
            ds:InDisabled(true)
            disabledIdle()
            ds:InDisabled(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsDisabled()
            enableWatchdog()
        elseif wpilib.IsAutonomous() then
            disableWatchdog()
            ds:InAutonomous(true)
            autonomous()
            ds:InAutonomous(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsAutonomous() or not wpilib.IsEnabled()
            enableWatchdog()
        elseif wpilib.IsTest() then
            disableWatchdog()
            --lw:SetEnabled(true)
            ds:InTest(true)
            repeat ds:WaitForData() until not wpilib.IsTest() or not wpilib.IsEnabled()
            ds:InTest(false)
            --lw:SetEnabled(false)
        else
            ds:InOperatorControl(true)
            teleop()
            ds:InOperatorControl(false)
            disableWatchdog()
            repeat ds:WaitForData() until not wpilib.IsOperatorControl() or not wpilib.IsEnabled()
            enableWatchdog()
        end
    end
end

function dashboardUpdate()
    -- local pressureVoltageMin = 1.17
    wpilib.SmartDashboard_PutBoolean("pressure", pressureSwitch:Get())
    wpilib.SmartDashboard_PutNumber("Pressure Transducer", pressureTransducer:GetVoltage())
    wpilib.SmartDashboard_PutNumber("Drive PID output", drivePID.output)
end

function disabledIdle()
    while wpilib.IsDisabled() do
        feedWatchdog()

        --Load Dashboard outputs
        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        feedWatchdog()
        wpilib.Wait(AUTO_LOOP_LAG)
        feedWatchdog()
    end


end

local function performAuto()
    --shooter.fullStop()
    --arm.setPreset("sideShot")

    local shootTimer = wpilib.Timer()
    shootTimer:Start()

    while shootTimer:Get() < 4 do
        shooter.setFlywheelRunning(true)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end
--    intake.setDeploy(true)

    local t = wpilib.Timer()
    t:Start()
    while t:Get() < 1 do
        shooter.setConveyerManual(0)
        shooter.setRollerManual(1)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end

 --   intake.setDeploy(false)
    local t = wpilib.Timer()
    t:Start()
    shooter.fire()
    while t:Get() < 5 do
        shooter.setConveyerManual(0)
        shooter.setRollerManual(0)
        drive.update(0, 0, false)
        intake.setIntakeSpeed(0.0)
        coroutine.yield()
    end

  --  arm.setPreset("Intake")



    -- Clean up
    shooter.fullStop()
    intake.setDeploy(false)
    drive.update(0, 0, false)
    intake.setIntakeSpeed(0.0)
end

local function isDone()
    if math.abs(robotLinearError) < drivePercision then --and drive.getDriveVelocity() < velocityPercision then
        return true
    else
        return false
    end
end

function autoDrive(x, y, isBackward, dPer, tPer, driveCap, turnCap, arcCap)
    targetX = x
    targetY = y
    drivePercision = dPer
    turnPercision = tPer
    --velocityPercision = vPer
    drivePID.min, drivePID.max = -driveCap, driveCap
    anglePID.min, anglePID.max = -arcCap, arcCap
    rotatePID.min, rotatePID.max = -turnCap, turnCap
end

function getIsBackward()
    return isBackward
end

function autonomous()
    disableWatchdog()

    --local c = coroutine.create(performAuto)

    while wpilib.IsAutonomous() and wpilib.IsEnabled() do --and coroutine.status(c) ~= "dead" do
        --coroutine.resume(c)

        -- values for the auto drive
        gyroCurr = drive.getGyroAngle()
            if gyroCurr > 180 then
                gryoCurr = gyroCurr - 360
            elseif gyroCurr < -180 then
                gyroCurr = gyroCurr + 360
            else 
                gyroCurr = gyroCurr
            end
        currTheta = currTheta + (gyroCurr - prevGyro)
        theta = (currTheta + prevTheta) / 2
        leftCurr =  drive.getLeftDist()
        rightCurr = drive.getRightDist()
        magnitude = (leftCurr + rightCurr - leftPrev - rightPrev) / 2
        deltaX = -magnitude * math.sin(theta / 180 * math.pi)
        deltaY = magnitude * math.cos(theta / 180 * math.pi)
        prevGyro = gyroCurr
        leftPrev = leftCurr
        rightPrev = rightCurr
        prevTheta = currTheta
        currX = currX + deltaX
        currY = currY + deltaY
        driveError = math.sqrt((targetX - currX)^2 + (targetY - currY)^2)
        targetAngle = math.atan2(currX - targetX, targetY - currY) / math.pi * 180
        wpilib.SmartDashboard_PutNumber("X", currX)
        wpilib.SmartDashboard_PutNumber("Y", currY)
        wpilib.SmartDashboard_PutNumber("Drive Error", driveError)
        --wpilib.SmartDashboard_PutBoolean("Done", isDone())

        if getIsBackward() then
            targetAngle = targetAngle + 180
        end
        if targetAngle > 180 then
            targetAngle = targetAngle - 360
        end
        robotLinearError = (targetY - currY) * math.cos(drive.getGyroAngle() / 180 * math.pi) - (targetX - currX) * math.sin(drive.getGyroAngle() / 180 * math.pi)
        angleError = targetAngle - drive.getGyroAngle()
        wpilib.SmartDashboard_PutNumber("Angle Error", angleError)
        wpilib.SmartDashboard_PutNumber("Target Angle", targetAngle)
        if getIsBackward() then
            if math.abs(targetAngle - drive.getGyroAngle()) < turnPercision then
                if math.abs(robotLinearError) < drivePercision then
                    drive.update(0, 0, false)
                else
                    drive.update(drivePID:update(driveError), anglePID:update(targetAngle - drive.getGyroAngle()), false)
                end
            else
                drive.update(0, rotatePID:update(targetAngle - drive.getGyroAngle()), false)
            end
        else
            if math.abs(targetAngle - drive.getGyroAngle()) < turnPercision then
                if math.abs(robotLinearError) < drivePercision then
                    drive.update(0, 0, false)
                else
                    drive.update(-drivePID:update(driveError), anglePID:update(targetAngle - drive.getGyroAngle()), false)
                end
            else
                drive.update(0, rotatePID:update(targetAngle - drive.getGyroAngle()), false)
            end
        end

        updateCompressor()
        intake.update()
        shooter.update()
        hangingPin:Set(false)
        hangDeployOn:Set(false)
        hangDeployOff:Set(true)
        arm.update()
        -- don't update drive, should be done in coroutine

        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        wpilib.Wait(TELEOP_LOOP_LAG)
    end

    -- Clean up
    shooter.fullStop()
    drive.update(0, 0, false)
end


function teleop()
    while wpilib.IsOperatorControl() and wpilib.IsEnabled() do
        enableWatchdog()
        feedWatchdog()


        -- Read controls
        controls.update(controlMap)
        feedWatchdog()

        -- Pneumatics
        updateCompressor()

        intake.update()
        shooter.update()

        hangingPin:Set(prepareHang)
        hangDeployOn:Set(hanging)
        hangDeployOff:Set(not hanging)

        -- Drive
        drive.update(driveX, driveY, quickTurn)

        -- Arm
        arm.update()

        -- Dashboard
        dashboardUpdate()
        arm.dashboardUpdate()
        drive.dashboardUpdate()
        shooter.dashboardUpdate()

        -- Iteration cleanup
        feedWatchdog()
        wpilib.Wait(TELEOP_LOOP_LAG)
        feedWatchdog()
    end
end

function updateCompressor()
    if pressureSwitch:Get() then
        compressor:Set(wpilib.Relay_kOff)
    else
        compressor:Set(wpilib.Relay_kOn)
    end
end

-- Inputs/Outputs
-- Don't forget to add to declarations at the top!
compressor = wpilib.Relay(1, 8, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(14)
pressureTransducer = wpilib.AnalogChannel(4)
hangingPin = wpilib.Solenoid(4)
hangDeployOn = wpilib.Solenoid(1)
hangDeployOff = wpilib.Solenoid(7)
-- End Inputs/Outputs

-- Controls
local function incConstant(tbl, name, pid, delta)
    return function()
        local target = pid.target
        pid:start()
        tbl[name] = tbl[name] + delta
        pid:stop()
        pid:reset()
        pid.target = target
    end
end

local function deadband(axis, threshold)
    if axis < threshold and axis > -threshold then
        return 0
    else
        return axis
    end
end

local prevCoDriverDpad = 0.0

controlMap =
{
    -- Joystick 1 (Driver)
    {
        ["y"] = function(axis) 
            if lowGear then
                driveY = -deadband(axis, 0.1) / 3
            else
                driveY = -deadband(axis, 0.1)
            end
        end,
        ["rx"] = function(axis)
            if lowGear then
                driveX = deadband(axis, 0.1) / 3
            else
                driveX = deadband(axis, 0.1)
        end
        end,
        [7] = {tick=function(held)
            if held then
                intake.setRetract(true)
            else
                intake.setRetract(false)
            end
        end},
        [3] = {tick=function(held)
            if held then
                intake.setDeploy(true)
            else
                intake.setDeploy(false)
            end
        end},
        [5] = {tick=function(held)
            quickTurn = held
        end},

        [6] = {tick=function(held)
            lowGear = held
        end},

        [1] = function()
            prepareHang = true
            arm.setPreset("Horizontal")
        end,

        [10] = function()
            if prepareHang then
                hanging = false
            end
        end,

        ["rtrigger"] = function()
            if prepareHang then
                hanging = true
            end
        end,
    },
    -- Joystick 2 (Co-Driver)
    {
        ["y"] = function(axis)
            shooter.setRollerManual(-deadband(axis, 0.1))
            shooter.setConveyerManual(-deadband(axis, 0.1))
        end,

        ["ry"] = function(axis)
            if state == INTAKE_LOAD then
                shooter.setRollerManual(-deadband(axis, 0.1))
                shooter.setConveyerManual(-deadband(axis, 0.1))
                intake.setIntakeSpeed(-deadband(axis, 0.1))
            end
        end,

        [1] = function()
            if not prepareHang then
                arm.setPreset("Intake")
                shooter.setFlywheelRunning(false)
                shooter.setFlapActive(false)
                shooter.setHardStopActive(true)
                state = INTAKE_LOAD
            end
        end,

        [3] = function()
            if not prepareHang then
                arm.setPreset("sideShot")
                shooter.setFlapActive(false)
                shooter.setHardStopActive(false)
                state = FIRE
            end
        end,

        [4] = function()
            if not prepareHang then
                arm.setPreset("Shooting")
                shooter.setFlapActive(false)
                shooter.setHardStopActive(false)
                state = FIRE
            end
        end,

        [2] = function()
            if not prepareHang then
                arm.setPreset("Stow")
                shooter.setFlapActive(false)
                shooter.setFlywheelRunning(false)
                state = STOW
            end
        end,

        [7] = function()
            if not prepareHang then
                arm.setPreset("Loading")
                shooter.setFlywheelRunning(false)
                shooter.setFlapActive(true)
                shooter.setHardStopActive(true)
                state = HUMAN_LOAD
            end
        end,

        [5] = {tick=shooter.humanLoad},

        [6] = {down=shooter.fire, up=function() shooter.fire(false) end},

        [8] = function()
            if state == FIRE then
                shooter.setFlywheelRunning(true)
            else
                shooter.setFlywheelRunning(false)
            end
        end,

        [9] = function()
            if state == FIRE then
                shooter.setFlywheelRunning(false)
            end
        end,

        [10] = function()
            if state == HUMAN_LOAD then
                shooter.setFlapActive(false)
            elseif state == INTAKE_LOAD then
                shooter.setFlapActive(true)
            end
        end,

        ["haty"] = function(axis)
            local increment = 0.5
            if prepareHang then
                increment = 5
            end
            if axis > 0.5 and prevCoDriverDpad <= 0.5 then
                -- Dpad down
                arm.setTarget(arm.getTarget() - increment)
            end
            if axis < -0.5 and prevCoDriverDpad >= -0.5 then
                -- Dpad down
                arm.setTarget(arm.getTarget() + increment)
            end
            prevCoDriverDpad = axis
        end,
    },
    -- Joystick 3
    {
    },
    -- Joystick 4 (eStop Module)
    {
    },
    -- Cypress Module
    cypress={},
}
-- End Controls

-- Watchdog shortcuts
if watchdogEnabled then
    wpilib.GetWatchdog():SetExpiration(0.25)

    function feedWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:Feed()
    end

    function enableWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:SetEnabled(true)
    end

    function disableWatchdog()
        local dog = wpilib.GetWatchdog()
        dog:SetEnabled(false)
    end
else
    local dog = wpilib.GetWatchdog()
    dog:SetEnabled(false)

    feedWatchdog = function() end
    enableWatchdog = function() end
    disableWatchdog = function() end
end

-- Only create the gyro at the end, because it blocks the entire thread.
--TODO(rlight): Add this back in when we have gyro
--drive.initGyro()
-- vim: ft=lua et ts=4 sts=4 sw=4
