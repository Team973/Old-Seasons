#include "WPILib.h"
#include "robot.hpp"
#include "lib/utility.hpp"
#include "lib/logger.hpp"
#include "subsystems/drive.hpp"
#include <pthread.h>
#include <unistd.h>

namespace frc973 {

void* Robot::runLogger(void*)
{
    Logger::Run(NULL);
    return NULL;
}

void* Robot::runThread(void*)
{
    while (true)
    {
        printf("hello from a second thread\n");
        usleep(10000);
    }
}

Robot::Robot()
{
    pthread_t loggingThread;
    pthread_create(&loggingThread, NULL, runLogger, NULL);

    pthread_t secondThread;
    pthread_create(&secondThread, NULL, runThread, NULL);

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
    Logger::Log("From outside a thread!!");
    drive->CheesyDrive(deadband(testStick->GetY(), 0.1), deadband(testStick->GetRawAxis(2), 0.1), false, testStick->GetRawButton(6));
    drive->strafe(-deadband(testStick->GetX(), 0.3));
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
