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
    oliverStick = new Joystick(2);

    leftDriveMotors = new VictorSP(0);
    rightDriveMotors = new VictorSP(1);

    elevatorMotor = new VictorSP(6);

    leftIntakeMotor = new VictorSP(8);
    rightIntakeMotor = new VictorSP(9);

    containerGrabberMotor = new VictorSP(4);

    humanLoadFunnelSolenoid = new Solenoid(0);
    intakeSolenoid = new Solenoid(1);
    footSolenoid = new Solenoid(7);

    clawClampSolenoid = new Solenoid(2);
    clawBrakeSolenoid = new Solenoid(3);

    airPressureSwitch = new DigitalInput(9);
    compressor = new Relay(0, Relay::kForwardOnly);

    statusLEDA = new Relay(1, Relay::kBothDirections);
    statusLEDB = new Relay(2, Relay::kBothDirections);
    statusLEDC = new Relay(3, Relay::kBothDirections);
    statusLEDD = new Relay(4, Relay::kBothDirections);

    leftDriveEncoder = new Encoder(4,5, false, CounterBase::k2X);
    rightDriveEncoder = new Encoder(2,3, false, CounterBase::k2X);

    elevatorEncoder = new Encoder(6,7);
    elevatorPot = new AnalogInput(1);

    gyro = new Encoder(0,1,false,CounterBase::k2X);

    spiGyro = new SPIGyro();

    toteSensor = new DigitalInput(8);

    leftGrabberMotorA = new CANTalon(0);
    leftGrabberMotorA->SetControlMode(CANSpeedController::kPosition);
    leftGrabberMotorA->SetFeedbackDevice(CANTalon::QuadEncoder);
    leftGrabberMotorA->SelectProfileSlot(0);
    rightGrabberMotorA = new CANTalon(2);
    rightGrabberMotorA->SetControlMode(CANSpeedController::kPosition);
    rightGrabberMotorA->SetFeedbackDevice(CANTalon::QuadEncoder);
    rightGrabberMotorA->SelectProfileSlot(0);
    leftGrabberMotorB = new CANTalon(1);
    leftGrabberMotorB->SetControlMode(CANSpeedController::kPosition);
    leftGrabberMotorB->SetFeedbackDevice(CANTalon::QuadEncoder);
    leftGrabberMotorB->SelectProfileSlot(0);
    rightGrabberMotorB = new CANTalon(3);
    rightGrabberMotorB->SetControlMode(CANSpeedController::kPosition);
    rightGrabberMotorB->SetFeedbackDevice(CANTalon::QuadEncoder);
    rightGrabberMotorB->SelectProfileSlot(0);

    locator = new Locator(leftDriveEncoder, rightDriveEncoder, spiGyro, gyro);

    xyManager = XYManager::getInstance();
    xyManager->injectLocator(locator);

    drive = new Drive(leftDriveMotors, rightDriveMotors);
    sauropod = new Sauropod(elevatorMotor, elevatorEncoder, clawClampSolenoid, clawBrakeSolenoid);
    intake = new Intake(leftIntakeMotor, rightIntakeMotor, intakeSolenoid, humanLoadFunnelSolenoid, footSolenoid, toteSensor);
    grabber = new ContainerGrabber(leftGrabberMotorA, leftGrabberMotorB, rightGrabberMotorA, rightGrabberMotorB);

    grabManager = new GrabManager(drive, grabber);

    controls = new ControlMap(driver, coDriver);

    stateManager = new StateManager(drive, sauropod, intake);

    controlManager = new ControlManager(controls, stateManager);

    autoManager = new AutoManager(stateManager);

    Logger::Log(MESSAGE, "objects initialized\n");

    Logger::Log(MESSAGE, "starting smart dashboard\n");

    autoType = NORMAL;
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
    SmartDashboard::PutNumber("LeftA Grabber Encoder: ", leftGrabberMotorA->GetEncPosition());
    SmartDashboard::PutNumber("LeftB Grabber Encoder: ", leftGrabberMotorB->GetEncPosition());
    SmartDashboard::PutNumber("RightA Grabber Encoder: ", rightGrabberMotorA->GetEncPosition());
    SmartDashboard::PutNumber("RightB Grabber Encoder: ", rightGrabberMotorB->GetEncPosition());
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
    if (!autoRan) {
        leftGrabberMotorA->SetPosition(0);
        leftGrabberMotorB->SetPosition(0);
        rightGrabberMotorA->SetPosition(0);
        rightGrabberMotorB->SetPosition(0);
    }

    switch (autoType) {
        case CANBURGLE:
            grabManager->initSequence();

            switch (grabberType) {
                case CARBON_FIBER:
                    switch (grabberSpeed) {
                        case FAST:
                            grabManager->startSequence(1.0, false);
                            break;
                        case SLOW:
                            grabManager->startSequence(0.4, false);
                            break;
                    }
                    break;
                case ALUMINUM:
                    switch (grabberSpeed) {
                        case FAST:
                            grabManager->startSequence(0.6, false);
                            break;
                        case SLOW:
                            grabManager->startSequence(0.4, false);
                            break;
                    }
                    break;
            }
            break;
        case NORMAL:
            grabManager->cancelSequence();
            break;
    }

    dashboardUpdate();
}

void Robot::AutonomousInit()
{
    autoManager->setMode("TurnThreeTote");
    spiGyro->ZeroAngle();
    locator->resetGyro();
    locator->resetAll();
    autoManager->getCurrentMode()->init();
}

void Robot::AutonomousPeriodic()
{
    autoRan = true;
    switch (autoType) {
        case NORMAL:
            autoManager->getCurrentMode()->run();

            xyManager->update();
            stateManager->update();
            drive->update();

            runCompressor();
            break;
    }

    statusLEDA->Set(Relay::kOn);

    if (grabManager->isDriving()) {
        statusLEDB->Set(Relay::kOn);
    }
    if (grabber->isSettled()) {
        statusLEDC->Set(Relay::kOn);
    }

    grabManager->update();

    dashboardUpdate();
}

void Robot::TeleopInit()
{
    //grabManager->cancelSequence();
    //grabManager->startSequence(1.0, false);
    //grabber->retract();
    grabManager->cancelSequence();
    stateManager->unBrakeClaw();
}

void Robot::TeleopPeriodic()
{
    grabber->update();
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
