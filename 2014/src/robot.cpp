#include "WPILib.h"
#include "drive.hpp"
//#include "gyro/GyroManager.h"
#include "robot.hpp"

#include "NetworkTables/NetworkTable.h"

Robot::Robot()
{
    this->SetPeriod(0);
    motor = new Talon(8);
    /*
    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);
    armMotor = new Talon(3);
    winchMotor = new Talon(4);
    intakeMotor = new Talon(5);

    shiftingSolenoid = new Solenoid(1);
    kickUpSolenoid = new Solenoid(2);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(4);

    //gyro = new GyroManager();

    leftDriveEncoder = new Encoder(1, 2);
    leftDriveEncoder->Start();
    rightDriveEncoder = new Encoder(3, 4);
    rightDriveEncoder->Start();

    compressor = new Compressor(14,8);

    drive = new Drive(leftDriveMotors, rightDriveMotors, leftDriveEncoder, rightDriveEncoder);

    GetWatchdog().SetExpiration(100);
    GetWatchdog().SetEnabled(true);
    */
}

void Robot::RobotInit()
{
    //compressor->Start();
    //SmartDashboard::init();
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
    //GetWatchdog().Feed();
}

void Robot::AutonomousInit()
{
}

void Robot::AutonomousPeriodic()
{
    //GetWatchdog().Feed();
}

void Robot::TeleopInit()
{
}

void Robot::TeleopPeriodic()
{
    //GetWatchdog().Feed();
    float counter = 0;
    SmartDashboard::PutNumber("Test call: ", counter++);
    motor->Set(.3);
}

void Robot::TestInit()
{
}

void Robot::TestPeriodic()
{
    //GetWatchdog().Feed();
}

/*
void Robot::OperatorControl()
{
    float counter = 0;
    while (IsOperatorControl() && IsEnabled())
    {

        SmartDashboard::PutNumber("Test call: ", counter++);
        Wait(0.10);
    }
}
*/

START_ROBOT_CLASS(Robot);
