#include "WPILib.h"
#include "robot.hpp"
#include "constants.hpp"
#include "controlMap.hpp"
#include "autoManager.hpp"
#include "stateManager.hpp"
#include "controlManager.hpp"
#include "lib/utility.hpp"
#include "lib/logger.hpp"
#include "lib/txtFileIO.hpp"
#include "lib/gyro.hpp"
#include "lib/gyro_sender.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"
#include "subsystems/intake.hpp"
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

    //pthread_t gyroThread;
    //pthread_create(&gyroThread, NULL, GyroSender::processor_executor, new GyroSender());

    Logger::Log(MESSAGE, "Boot complete, initializing objects...\n");

    driver = new Joystick(0);
    coDriver = new Joystick(1);

    leftDriveMotors = new VictorSP(0);
    rightDriveMotors = new VictorSP(1);

    elevatorMotor = new VictorSP(6);
    armMotor = new VictorSP(7);

    leftIntakeMotor = new VictorSP(8);
    rightIntakeMotor = new VictorSP(9);
    humanFeederIntakeMotor = new VictorSP(10);

    whipMotor = new VictorSP(3);

    humanFeederSolenoid = new Solenoid(0);
    floorSolenoid = new Solenoid(1);

    leftDriveEncoder = new Encoder(0,1);
    rightDriveEncoder = new Encoder(2,3);

    elevatorEncoder = new Encoder(5,6);
    armEncoder = new Encoder(7,8);

    gyro = new SPIGyro();

    whipPot = new AnalogInput(2);

    toteSensor = new DigitalInput(2);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftDriveMotors, rightDriveMotors);
    sauropod = new Sauropod(elevatorMotor, armMotor, elevatorEncoder,  armEncoder);
    intake = new Intake(leftIntakeMotor, rightIntakeMotor, humanFeederIntakeMotor, whipMotor, floorSolenoid, humanFeederSolenoid, whipPot, toteSensor);

    controls = new ControlMap(driver, coDriver);

    stateManager = new StateManager(drive, sauropod, intake);

    controlManager = new ControlManager(controls, stateManager);

    autoManager = new AutoManager(stateManager);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

}

void Robot::dashboardUpdate()
{
    SmartDashboard::PutString("DB/String 0", asString(leftDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 1", asString(rightDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 2", autoManager->getCurrentName());
    SmartDashboard::PutString("DB/String 3", asString(sauropod->getElevatorHeight()));
    SmartDashboard::PutString("DB/String 4", asString(gyro->GetDegrees()));
    SmartDashboard::PutNumber("raw arm encoder: ", armEncoder->Get());
    SmartDashboard::PutBoolean("is sauropod done: ", sauropod->atTarget());
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
    stateManager->update();

    dashboardUpdate();
}

void Robot::TeleopInit()
{
}

void Robot::TeleopPeriodic()
{
    controlManager->update();
    stateManager->update();
    dashboardUpdate();
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
