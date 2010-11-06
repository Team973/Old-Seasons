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
leftMotor2 = wpilib.Victor(3)
rightMotor1 = wpilib.Victor(2)
rightMotor2 = wpilib.Victor(4)

gearSwitch = wpilib.Solenoid(1)

intakeMotor = wpilib.Victor(6, 3)

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)


-- Kicker(To do)
gateLatch = wpilib.Solenoid(3)
kickerCylinder = wpilib.Solenoid(4)
thirdZoneCylinder = wpilib.Solenoid(5)
kickerReloadTime = 1
kickerFireTime = 1
kickerReturnTime = 1
kickerCatchTime = 1

-- Arm
PTOSwitch = wpilib.Solenoid(2)
