#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"

Robot::Robot()
{
    this->SetPeriod(0);

    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);

    shiftingSolenoid = new Solenoid(5);

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid);
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
    drive->update();
}

void Robot::TestInit() {
}

void Robot::TestPeriodic() {
}

START_ROBOT_CLASS(Robot);
