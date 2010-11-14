-- config.lua

module(..., package.seeall)

watchdogEnabled = false

features =
{
    compressor = true,
    gearSwitch = true,
    lcd = true,
    softClutch = false,
}

-- Drive
leftMotor1 = wpilib.Victor(1)
leftMotor2 = wpilib.Victor(5)
rightMotor1 = wpilib.Victor(2)
rightMotor2 = wpilib.Victor(4)

gearSwitch = wpilib.Solenoid(1)

intakeMotor = wpilib.Victor(6, 3)
intakeEncoder = wpilib.Encoder(6, 1, 6, 2)
intakeEncoder:SetDistancePerPulse(360.0 / 300.0) -- the second number is the
                                                 -- pulses per rotation

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)

-- Kicker(To do)
gateLatch = wpilib.Solenoid(3)
kickerCylinder = wpilib.Solenoid(2)
kickerReloadTime = 1
kickerFireTime = .3
kickerReturnTime = 1
kickerCatchTime = .5
kickerStopIntakeTime = .2

-- Arm
PTOSwitch = wpilib.Solenoid(2)

-- Autonomous
defaultInitialDelay = 2
longInitialDelay = 7
autonomousSwitch = wpilib.DigitalInput(6,3)
