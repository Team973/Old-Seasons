#include "WPILib.h"
#include "robot.hpp"
#include "lib/utility.hpp"
#include "lib/logger.hpp"
#include "lib/txtFileIO.hpp"
#include "subsystems/drive.hpp"
#include <pthread.h>
#include <unistd.h>

namespace frc973 {

void* Robot::runLogger(void*)
{
    Logger::Run(NULL);
    return NULL;
}

void* Robot::runTxtIO(void*)
{
    TxtWriter::Run();
    return NULL;
}

Robot::Robot()
{
    pthread_t loggingThread;
    pthread_create(&loggingThread, NULL, runLogger, NULL);

    pthread_t parsingThread;
    pthread_create(&parsingThread, NULL, runTxtIO, NULL);

    Logger::Log("testing, testing, 123\n");

    leftFrontDrive = new Talon(0);
    rightFrontDrive = new Talon(1);
    leftBackDrive = new Talon(2);
    rightBackDrive = new Talon(3);

    strafeDrive = new Talon(4);

    testStick = new Joystick(0);

    drive = new Drive(leftFrontDrive, rightFrontDrive, leftBackDrive, rightBackDrive, strafeDrive);

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
    drive->CheesyDrive(deadband(testStick->GetY(), 0.1), deadband(testStick->GetRawAxis(2), 0.1), false, testStick->GetRawButton(6));
    drive->strafe(-deadband(testStick->GetX(), 0.3));
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
