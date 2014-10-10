#include "WPILib.h"
#include "robot.hpp"
#include "drive.hpp"
#include "greyJoy.hpp"
#include "NetworkTables/NetworkTable.h"

Robot::Robot()
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
    /*
    drive->CheesyDrive(driver->getAxis("y"), driver->getAxis("rx"), driver->getButton("rbumper"), driver->getButton("lbumper"));
    drive->update();
    driver->update();
    coDriver->update();
    */
    SmartDashboard::PutBoolean("test button: ", driver->getButton("a"));
    driver->update();
}

void Robot::TestInit() {
}

void Robot::TestPeriodic() {
}

START_ROBOT_CLASS(Robot);
