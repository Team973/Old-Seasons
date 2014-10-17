#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "arm.hpp"
#include "intake.hpp"

Robot::Robot()
{
    this->SetPeriod(0);

    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);
    armMotor = new Talon(3);
    intakeMotor = new Victor(6);

    shiftingSolenoid = new Solenoid(5);
    clawSolenoid = new Solenoid(3);

    armEncoder = new Encoder(6,7);
    armEncoder->Start();

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid);
    arm = new Arm(armMotor, armEncoder);
    intake = new Intake(intakeMotor, clawSolenoid);

    driver = new Joystick(1);
    coDriver = new Joystick(2);
}

void Robot::RobotInit() {
}

void Robot::DisabledInit() {
}

void Robot::DisabledPeriodic() {
}

void Robot::AutonomousInit() {
}

void Robot::AutonomousPeriodic() {
}

void Robot::TeleopInit() {
}

void Robot::TeleopPeriodic() {

    // Driver
    drive->setBehavior(-driver->GetY(), -driver->GetRawAxis(3), false, false);

    // coDriver
    if (coDriver->GetRawButton(2))
        arm->setBehavior("intake");

    if (coDriver->GetRawButton(5))
        arm->setBehavior("stow");

    intake->setSpeed(coDriver->GetY());

    if (coDriver->GetRawButton(7))
        intake->setFangs(true);

    if (coDriver->GetRawButton(8))
        intake->setFangs(false);

    drive->update();
    arm->update();
    intake->update();
}

void Robot::TestInit() {
}

void Robot::TestPeriodic() {
}

START_ROBOT_CLASS(Robot);
