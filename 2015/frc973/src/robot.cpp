#include "WPILib.h"
#include "robot.hpp"
#include "constants.hpp"
#include "controlMap.hpp"
#include "autoManager.hpp"
#include "stateManager.hpp"
#include "controlManager.hpp"
#include "grabManager.hpp"
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
#include "subsystems/containerGrabber.hpp"
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
	//SocketClient::Start();
    //SocketClient::Run(NULL);
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
    armController = new Joystick(2);

    leftDriveMotors = new VictorSP(0);
    rightDriveMotors = new VictorSP(1);

    leftArmMotors = new Talon(2);
    rightArmMotors = new Talon(3);

    elevatorMotor = new VictorSP(6);

    leftIntakeMotor = new VictorSP(8);
    rightIntakeMotor = new VictorSP(9);

    containerGrabberMotor = new VictorSP(4);

    humanLoadFunnelSolenoid = new Solenoid(0);
    intakeSolenoid = new Solenoid(1);
    footSolenoid = new Solenoid(4);

    clawClampSolenoid = new Solenoid(2);
    clawBrakeSolenoid = new Solenoid(5);

    grabberSolenoid = new Solenoid(3);

    airPressureSwitch = new DigitalInput(9);
    compressor = new Relay(0, Relay::kForwardOnly);

    statusLEDA = new Relay(1, Relay::kBothDirections);
    statusLEDB = new Relay(2, Relay::kBothDirections);
    statusLEDC = new Relay(3, Relay::kBothDirections);
    statusLEDD = new Relay(4, Relay::kBothDirections);

    leftDriveEncoder = new Encoder(4,5, false, CounterBase::k2X);
    rightDriveEncoder = new Encoder(2,3, false, CounterBase::k2X);

    leftArmEncoder = new Encoder(10, 11);
    rightArmEncoder = new Encoder(12, 13);

    elevatorEncoder = new Encoder(6,7);
    elevatorPot = new AnalogInput(1);

    gyro = new Encoder(0,1,false,CounterBase::k2X);

    spiGyro = new SPIGyro();

    toteSensor = new DigitalInput(8);

    autoSwitchA = new DigitalInput(0);
    autoSwitchB = new DigitalInput(1);
    autoSwitchC = new DigitalInput(25);

    leftGrabberMotorA = new CANTalon(0);
    leftGrabberMotorA->SetControlMode(CANSpeedController::kPercentVbus);
    rightGrabberMotorA = new CANTalon(2);
    rightGrabberMotorA->SetControlMode(CANSpeedController::kPercentVbus);
    leftGrabberMotorB = new CANTalon(1);
    leftGrabberMotorB->SetControlMode(CANSpeedController::kPercentVbus);
    rightGrabberMotorB = new CANTalon(3);
    rightGrabberMotorB->SetControlMode(CANSpeedController::kPercentVbus);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, spiGyro, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftDriveMotors, rightDriveMotors);
    sauropod = new Sauropod(elevatorMotor, elevatorEncoder, clawClampSolenoid, clawBrakeSolenoid);
    intake = new Intake(leftIntakeMotor, rightIntakeMotor, intakeSolenoid, humanLoadFunnelSolenoid, footSolenoid, toteSensor);
    grabber = new ContainerGrabber(leftGrabberMotorA, leftGrabberMotorB, rightGrabberMotorA, rightGrabberMotorB, leftArmEncoder, rightArmEncoder);

    grabManager = new GrabManager(grabberSolenoid);

    controls = new ControlMap(driver, coDriver);

    stateManager = new StateManager(drive, sauropod, intake, grabManager);

    controlManager = new ControlManager(controls, stateManager);

    autoManager = new AutoManager(stateManager, grabManager);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

    grabberType = CARBON_FIBER;
    grabberSpeed = FAST;

    autoRan = false;
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
    SmartDashboard::PutNumber("Left Grabber Encoder: ", leftArmEncoder->Get());
    SmartDashboard::PutNumber("Right Grabber Encoder: ", rightArmEncoder->Get());
    SmartDashboard::PutNumber("LeftA Grabber Velocity: ", leftGrabberMotorA->GetEncVel());
    SmartDashboard::PutNumber("RightA Grabber Velocity: ", rightGrabberMotorA->GetEncVel());
    SmartDashboard::PutNumber("drive distance: ", locator->getMovedDistance());
    SmartDashboard::PutNumber("left drive distance: ", locator->getDistance(leftDriveEncoder));
    SmartDashboard::PutNumber("raw elevator encoder: ", elevatorEncoder->Get());
    SmartDashboard::PutNumber("raw left encoder: ", leftDriveEncoder->Get());
    SmartDashboard::PutNumber("raw right encoder: ", rightDriveEncoder->Get());
    SmartDashboard::PutBoolean("is sauropod done: ", sauropod->motionDone());
    SmartDashboard::PutBoolean("tote sensor: ", toteSensor->Get());
    SmartDashboard::PutNumber("anaglog tote sensor: ", elevatorPot->GetVoltage());
    SmartDashboard::PutBoolean("pressure: ", airPressureSwitch->Get());
    SmartDashboard::PutNumber("gyro angle: ", locator->getAngle());
    SmartDashboard::PutNumber("spigyro angle: ", spiGyro->GetDegrees());
    SmartDashboard::PutNumber("Elevator Height:", sauropod->getElevatorHeight());

    SmartDashboard::PutBoolean("AutoSwitch A: ", autoSwitchA->Get());
    SmartDashboard::PutBoolean("AutoSwitch B: ", autoSwitchB->Get());
    SmartDashboard::PutBoolean("AutoSwitch C: ", autoSwitchC->Get());
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

    if (autoSwitchA->Get() && autoSwitchB->Get()) {
        autoManager->setMode("Drive");
    } else if (autoSwitchA->Get()) {
        autoManager->setMode("BasicThreeTote");
    } else if (autoSwitchB->Get()) {
        autoManager->setMode("Grab");
    } else {
        autoManager->setMode("None");
    }

    if (!autoSwitchC->Get()) {
        autoManager->setMode("UberAuto");
    }

    dashboardUpdate();
}

void Robot::AutonomousInit()
{
    spiGyro->ZeroAngle();
    locator->resetGyro();
    locator->resetAll();
    autoManager->getCurrentMode()->init();
}

void Robot::AutonomousPeriodic()
{
    stateManager->actuateClaw(true);
    autoRan = true;
    autoManager->getCurrentMode()->run();

    xyManager->update();
    stateManager->update();
    drive->update();

    runCompressor();

    statusLEDA->Set(Relay::kOn);


    dashboardUpdate();
}

void Robot::TeleopInit()
{
    if (!autoRan) {
    }
    stateManager->unBrakeClaw();
}

void Robot::TeleopPeriodic()
{
    controlManager->update();
    stateManager->update();

    if (armController->GetRawButton(1)) {
        leftArmMotors->Set(-0.2);
    } else if (armController->GetRawButton(4)) {
        leftArmMotors->Set(0.35);
    } else {
        leftArmMotors->Set(0.0);
    }

    if (armController->GetRawButton(2)) {
        rightArmMotors->Set(-0.2);
    } else if (armController->GetRawButton(3)) {
        rightArmMotors->Set(0.35);
    } else {
        rightArmMotors->Set(0.0);
    }

    runCompressor();

    dashboardUpdate();
}

void Robot::TestPeriodic()
{
}

}

START_ROBOT_CLASS(frc973::Robot);
