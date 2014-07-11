#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "greyJoy.hpp"

TestRobot::TestRobot()
{
    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);

    shiftingSolenoid = new Solenoid(5);

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid);

    driver = new GreyJoy(1);
    coDriver = new GreyJoy(2);

    this->SetPeriod(0);
}

void TestRobot::RobotInit() {
}

void TestRobot::DisabledInit() {
}

void TestRobot::DisabledPeriodic() {
}

void TestRobot::AutonomousInit() {
}

void TestRobot::AutonomousPeriodic() {
}

void TestRobot::TeleopInit() {
}

void TestRobot::TeleopPeriodic() {
    drive->CheesyDrive(driver->getAxis("y"), driver->getAxis("rx"), driver->getButton("rbumper"), driver->getButton("lbumper"));
}

void TestRobot::TestInit() {
}

void TestRobot::TestPeriodic() {
}

START_ROBOT_CLASS(TestRobot);
