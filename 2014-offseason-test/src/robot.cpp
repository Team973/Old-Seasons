#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "arm.hpp"
#include "intake.hpp"
#include "shooter.hpp"
#include <math.h>
#include "utility.hpp"
#include "constants.hpp"
#include "lib/subsystemLister.hpp"
#include "lib/subsystemBase.hpp"

namespace frc973
{

Robot::Robot()
{
    this->SetPeriod(0);

    Constants::Extend();

    subsystemLister = SubsystemLister::getInstance();

    leftFrontDriveMotors = new Talon(1);
    rightFrontDriveMotors = new Talon(2);
    leftBackDriveMotors = new Talon(5);
    rightBackDriveMotors = new Talon(6);
    strafeDriveMotors = new Talon(4);
    armMotor = new Talon(8);
    intakeMotor = new Victor(7);
    winchMotor = new Victor(3);

    shiftingSolenoid = new Solenoid(1);
    backShiftingSolenoid = new Solenoid(2);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(5);

    armEncoder = new Encoder(4,5);
    armEncoder->Start();

    winchFullCockSensor = new DigitalInput(2);

    armPot = new AnalogChannel(1);

    compressor = new Compressor(1,1);
    compressor->Start();

    drive = new Drive(leftFrontDriveMotors, rightFrontDriveMotors, leftBackDriveMotors, rightBackDriveMotors, strafeDriveMotors, shiftingSolenoid, backShiftingSolenoid);
    arm = dynamic_cast<Arm*>(subsystemLister->addReturnSubsystem("arm",
            new Arm(armMotor, armEncoder, armPot)));
    intake = dynamic_cast<Intake*>(subsystemLister->addReturnSubsystem("intake",
            new Intake(intakeMotor, clawSolenoid)));
    shooter = dynamic_cast<Shooter*>(subsystemLister->addReturnSubsystem("shooter",
            new Shooter(winchMotor, winchReleaseSolenoid, winchFullCockSensor)));

    driver = new Joystick(1);
    coDriver = new Joystick(2);

    autoThrottle = new KinectStick(1);
    autoTimer = new Timer();

    dsLCD = DriverStationLCD::GetInstance();

    autoMode = BLOCK;
}

void Robot::RobotInit() {
}

std::string Robot::boolToString(bool b)
{
    if (b)
        return "true";
    else
        return "false";
}

void Robot::DisabledInit()
{
    autoTimer->Stop();
    autoTimer->Reset();
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "Mode: %s", "block dat stuff");
    dsLCD->UpdateLCD();
}

void Robot::DisabledPeriodic() {
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line6, "Arm Angle: %f", arm->getAngle());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line5, "Full Cock: %s", boolToString(winchFullCockSensor->Get()).c_str());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "Arm Pot: %f", armPot->GetVoltage());
    dsLCD->UpdateLCD();

    if (coDriver->GetRawButton(1))
    {
        autoMode = BLOCK;
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "Mode: %s", "block dat stuff");
    }
    else if (coDriver->GetRawButton(3))
    {
        autoMode = JUST_DRIVE;
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "Mode: %s", "driving places");
    }

}

void Robot::AutonomousInit() {
    autoTimer->Start();
}

void Robot::AutonomousPeriodic() {
    switch (autoMode)
    {
        case JUST_DRIVE:

            if (autoTimer->Get() <= .5)
                drive->fromControls(1, 0, false, false);
            else
                drive->fromControls(0, 0, false, false);

            break;

        case BLOCK:
            drive->fromControls(-deadband(autoThrottle->GetY(), 0.2), 0, false, false);
            break;
    }
    drive->update();
}

void Robot::TeleopInit() {
}

void Robot::TeleopPeriodic() {

    // Driver
    drive->fromControls(-deadband(driver->GetY(), 0.1), deadband(driver->GetRawAxis(3), 0.1), driver->GetRawButton(6), driver->GetRawButton(5));
    drive->strafe(deadband(driver->GetX(), 0.1));


    if (driver->GetRawButton(8) && !shooter->isFiring())
    {
        if (arm->getCurrPreset() == "trussShot")
            intake->setFangs(false);

        shooter->wantFire();
    }

    // coDriver
    intake->setSpeed(coDriver->GetY());

    if (coDriver->GetRawButton(1))
    {
        intake->setFangs(true);
        arm->setPreset("pseudoIntake");
    }

    if (coDriver->GetRawButton(2))
    {
        intake->setFangs(true);
        arm->setPreset("intake");
    }

    if (coDriver->GetRawButton(4))
    {
        if (arm->isShotSafe())
        {
            shooter->setCock("fullCock");
        }
    }

    if (coDriver->GetRawButton(5))
    {
        intake->setFangs(true);
        arm->setPreset("stow");
    }

    if (coDriver->GetRawButton(6))
    {
        intake->setFangs(true);
        arm->setPreset("trussShot");
    }

    if (coDriver->GetRawButton(7))
    {
        intake->setFangs(true);
    }

    if (coDriver->GetRawButton(8))
    {
        intake->setFangs(false);
    }

    drive->update();
    arm->update();
    intake->update();
    shooter->update();

    dsLCD->PrintfLine(DriverStationLCD::kUser_Line5, "Full Cock: %s", boolToString(winchFullCockSensor->Get()).c_str());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line6, "Arm Angle: %f", arm->getAngle());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "Arm Pot: %f", armPot->GetVoltage());
    dsLCD->UpdateLCD();
}

void Robot::TestInit() {
}

void Robot::TestPeriodic() {
}

}

START_ROBOT_CLASS(frc973::Robot);
