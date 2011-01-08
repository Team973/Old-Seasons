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

-- Pneumatics
compressor = wpilib.Relay(4, 1, wpilib.Relay_kForwardOnly)
pressureSwitch = wpilib.DigitalInput(1)
