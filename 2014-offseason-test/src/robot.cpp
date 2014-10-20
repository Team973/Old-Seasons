#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "arm.hpp"
#include "intake.hpp"
#include <math.h>
#include "utility.hpp"

Robot::Robot()
{
    this->SetPeriod(0);

    leftFrontDriveMotors = new Talon(1);
    rightFrontDriveMotors = new Talon(2);
    leftBackDriveMotors = new Talon(5);
    rightBackDriveMotors = new Talon(6);
    strafeDriveMotors = new Talon(4);
    armMotor = new Talon(3);
    intakeMotor = new Victor(9);

    shiftingSolenoid = new Solenoid(5);
    clawSolenoid = new Solenoid(3);

    armEncoder = new Encoder(6,7);
    armEncoder->Start();

    drive = new Drive(leftFrontDriveMotors, rightFrontDriveMotors, leftBackDriveMotors, rightBackDriveMotors, strafeDriveMotors, shiftingSolenoid);
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
    drive->setBehavior(-deadband(driver->GetY(), 0.1), deadband(driver->GetRawAxis(3), 0.1), false, false);
    drive->strafe(deadband(driver->GetX(), 0.1));

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
