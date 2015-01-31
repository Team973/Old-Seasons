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

    elevatorMotor = new VictorSP(6);

    strafeDrive = new Talon(4);

    leftDriveEncoder = new Encoder(0,1);
    rightDriveEncoder = new Encoder(2,3);
    gyro = new Encoder(14,15);

    elevatorEncoder = new Encoder(5,6);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftFrontDrive, rightFrontDrive, leftBackDrive, rightBackDrive, strafeDrive);

    autoManager = new AutoManager(drive);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

}

void Robot::dashboardUpdate()
{
    SmartDashboard::PutString("DB/String 0", asString(leftDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 1", asString(rightDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 2", autoManager->getCurrentName());
    SmartDashboard::PutString("DB/String 3", asString(elevatorEncoder->Get()));
}

void Robot::RobotInit()
{
}

void Robot::DisabledInit()
{
    autoManager->resetModes();
}

void Robot::DisabledPeriodic()
{
    if (testStick->GetRawButton(1))
    {
        autoManager->nextMode();
    }

    dashboardUpdate();
}

void Robot::AutonomousInit()
{
    locator->resetEncoders();
    autoManager->getCurrentMode()->init();
}

void Robot::AutonomousPeriodic()
{
    autoManager->getCurrentMode()->run();

    xyManager->update();
    drive->update();

    dashboardUpdate();
}

void Robot::TeleopInit()
{
}

void Robot::TeleopPeriodic()
{
    drive->CheesyDrive(deadband(testStick->GetY(), 0.1), -deadband(testStick->GetRawAxis(2), 0.1), false, testStick->GetRawButton(5));
    drive->strafe(-deadband(testStick->GetX(), 0.3));
    elevatorMotor->Set(deadband(testStick->GetRawAxis(3), 0.1));
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
