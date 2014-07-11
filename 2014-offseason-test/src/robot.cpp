#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "greyJoy.hpp"
#include "NetworkTables/NetworkTable.h"

TestRobot::TestRobot()
{
    this->SetPeriod(0);

    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);
    leftDriveMotors->Set(.5);
    rightDriveMotors->Set(.5);

    shiftingSolenoid = new Solenoid(5);

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid);

    driver = new GreyJoy(1);
    coDriver = new GreyJoy(2);

    SmartDashboard::init();
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
    /*
    drive->CheesyDrive(driver->getAxis("y"), driver->getAxis("rx"), driver->getButton("rbumper"), driver->getButton("lbumper"));
    drive->update();
    driver->update();
    coDriver->update();
    */
    SmartDashboard::PutBoolean("test button: ", driver->getButton("a"));
    driver->update();
}

void TestRobot::TestInit() {
}

void TestRobot::TestPeriodic() {
}

START_ROBOT_CLASS(TestRobot);
