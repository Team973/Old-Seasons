#include "WPILib.h"
#include "robot.hpp"
#include "constants.hpp"
#include "autoManager.hpp"
#include "lib/utility.hpp"
#include "lib/logger.hpp"
#include "lib/txtFileIO.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/locator.hpp"
#include "subsystems/xyManager.hpp"
#include <pthread.h>
#include <unistd.h>
#include <time.h>

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
    Constants::Extend();

    Logger::Initialize();

    pthread_t loggingThread;
    pthread_create(&loggingThread, NULL, runLogger, NULL);

    pthread_t parsingThread;
    pthread_create(&parsingThread, NULL, runTxtIO, NULL);

    Logger::Log(MESSAGE, "Boot complete, initializing objects...\n");

    testStick = new Joystick(0);

    leftFrontDrive = new Talon(0);
    rightFrontDrive = new Talon(1);
    leftBackDrive = new Talon(2);
    rightBackDrive = new Talon(3);

    strafeDrive = new Talon(4);

    leftDriveEncoder = new Encoder(0,1);
    rightDriveEncoder = new Encoder(2,3);
    gyro = new Encoder(4,5);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftFrontDrive, rightFrontDrive, leftBackDrive, rightBackDrive, strafeDrive);

    autoManager = new AutoManager(drive);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

    SmartDashboard::init();

}

void Robot::dashboardUpdate()
{
    SmartDashboard::PutNumber("Left Encoder: ", leftDriveEncoder->Get());
    SmartDashboard::PutNumber("Right Encoder: ", rightDriveEncoder->Get());
}

void Robot::RobotInit()
{
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
    dashboardUpdate();
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
