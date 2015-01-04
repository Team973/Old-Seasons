#include "WPILib.h"
#include "robot.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

Robot::Robot()
{
    leftFrontDrive = new Talon(0);
    rightFrontDrive = new Talon(1);
    leftBackDrive = new Talon(2);
    rightBackDrive = new Talon(3);

    drive = new Drive(leftFrontDrive, rightFrontDrive, leftBackDrive, rightBackDrive);
}

void Robot::RobotInit()
{
}

void Robot::AutonomousInit()
{
}

void Robot::AutonomousPeriodic()
{

}

void Robot::TeleopInit()
{

}

void Robot::TeleopPeriodic()
{
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
