#include "WPILib.h"
#include "drive.hpp"
//#include "gyro/GyroManager.h"
#include "robot.hpp"

#include "NetworkTables/NetworkTable.h"

Robot::Robot()
{
    TELEOP_LOOP_LAG = 0.005;
    AUTO_LOOP_LAG = 0.005 * 1.50;

    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);
    armMotor = new Talon(3);
    winchMotor = new Talon(4);
    intakeMotor = new Talon(5);

    shiftingSolenoid = new Solenoid(1);
    kickUpSolenoid = new Solenoid(2);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(4);


    leftDriveEncoder = new Encoder(1, 2);
    leftDriveEncoder->Start();
    rightDriveEncoder = new Encoder(3, 4);
    rightDriveEncoder->Start();

    armSensorA = new Encoder(5, 6);
    armSensorA->Start();
    armSensorB = new Encoder(7, 8);
    armSensorB->Start();
    armSensorC = new Encoder(9, 10);
    armSensorC->Start();

    compressor = new Compressor(14,8);

    drive = new Drive(leftDriveMotors, rightDriveMotors, leftDriveEncoder, rightDriveEncoder);

    SmartDashboard::init();
}

void Robot::dashboardUpdate()
{
}

void Robot::Autonomous()
{
    while (IsAutonomous() && IsEnabled())
    {
        Wait(AUTO_LOOP_LAG);
    }
}

void Robot::OperatorControl()
{
    while (IsOperatorControl() && IsEnabled())
    {
        Wait(TELEOP_LOOP_LAG);
    }
}

void Robot::Test()
{
}

START_ROBOT_CLASS(Robot);
