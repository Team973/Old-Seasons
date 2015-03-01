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
#include "lib/socketClient.hpp"
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

void* Robot::runServer(void*)
{
    SocketClient::Run(NULL);
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

    pthread_t serverThread;
    pthread_create(&serverThread, NULL, runServer, NULL);

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
    humanFeederIntakeMotor = new VictorSP(5);

    whipMotor = new VictorSP(3);
    containerGrabberMotor = new VictorSP(4);

    humanFeederSolenoid = new Solenoid(1);
    floorSolenoid = new Solenoid(0);

    containerFinger = new Solenoid(2);

    airPressureSwitch = new DigitalInput(9);
    compressor = new Relay(0, Relay::kForwardOnly);

    leftDriveEncoder = new Encoder(4,5);
    rightDriveEncoder = new Encoder(2,3);

    elevatorEncoder = new Encoder(6,7);
    elevatorPot = new AnalogInput(1);

    armEncoder = new Encoder(0,1);
    armPot = new AnalogInput(0);

    gyro = new Encoder(24,25,false,CounterBase::k2X);

    //gyro = new SPIGyro();

    whipPot = new AnalogInput(2);

    toteSensor = new DigitalInput(8);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftDriveMotors, rightDriveMotors);
    sauropod = new Sauropod(elevatorMotor, armMotor, elevatorEncoder,  armEncoder, containerFinger);
    intake = new Intake(leftIntakeMotor, rightIntakeMotor, humanFeederIntakeMotor, whipMotor, floorSolenoid, humanFeederSolenoid, whipPot, toteSensor);

    controls = new ControlMap(driver, coDriver);

    stateManager = new StateManager(drive, sauropod, intake);

    controlManager = new ControlManager(controls, stateManager);

    autoManager = new AutoManager(stateManager);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

}

void Robot::runCompressor() {
    if (!airPressureSwitch->Get()) {
    	compressor->Set(Relay::kOn);
    } else {
        compressor->Set(Relay::kOff);
    }
}

void Robot::dashboardUpdate()
{
    SmartDashboard::PutString("DB/String 0", asString(leftDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 1", asString(rightDriveEncoder->Get()));
    SmartDashboard::PutString("DB/String 2", autoManager->getCurrentName());
    SmartDashboard::PutString("DB/String 3", asString(sauropod->getElevatorHeight()));
    SmartDashboard::PutNumber("drive distance: ", locator->getMovedDistance());
    SmartDashboard::PutNumber("left drive distance: ", locator->getDistance(leftDriveEncoder));
    SmartDashboard::PutNumber("raw arm encoder: ", armEncoder->Get());
    SmartDashboard::PutNumber("raw elevator encoder: ", elevatorEncoder->Get());
    SmartDashboard::PutNumber("raw left encoder: ", leftDriveEncoder->Get());
    SmartDashboard::PutNumber("raw right encoder: ", rightDriveEncoder->Get());
    SmartDashboard::PutBoolean("is sauropod done: ", sauropod->atTarget());
    SmartDashboard::PutNumber("Whip Angle: ", intake->getWhipAngle());
    SmartDashboard::PutNumber("Whip Voltage: ", whipPot->GetVoltage());
    SmartDashboard::PutBoolean("tote sensor: ", toteSensor->Get());
    SmartDashboard::PutNumber("anaglog tote sensor: ", elevatorPot->GetVoltage());
    SmartDashboard::PutBoolean("pressure: ", airPressureSwitch->Get());
    SmartDashboard::PutNumber("gyro angle: ", locator->getAngle());
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
    autoManager->setMode("Drive");
    locator->resetAll();
    //stateManager->disableAutoStacking();
    autoManager->getCurrentMode()->init();
}

void Robot::AutonomousPeriodic()
{
    autoManager->getCurrentMode()->run();

    xyManager->update();
    stateManager->update();
    drive->update();

    runCompressor();

    dashboardUpdate();
}

void Robot::TeleopInit()
{
    stateManager->enableAutoStacking();
    intake->stowWhip();
}

void Robot::TeleopPeriodic()
{
    controlManager->update();
    stateManager->update();

    runCompressor();

    dashboardUpdate();
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
