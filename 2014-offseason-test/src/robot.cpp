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

    stick = new Joystick(1);
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

    if (stick->GetRawButton(2))
        arm->setBehavior("intake");

    if (stick->GetRawButton(5))
        arm->setBehavior("stow");

    intake->setSpeed(stick->GetY());

    if (stick->GetRawButton(7))
        intake->setFangs(true);

    if (stick->GetRawButton(8))
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
