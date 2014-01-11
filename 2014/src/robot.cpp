#include "WPILib.h"
#include "drive.hpp"
#include "gyro/GyroManager.h"
#include "robot.hpp"

#include "NetworkTables/NetworkTable.h"

Robot::Robot()
{
    leftDriveMotors = new Talon(1);
    rightDriveMotors = new Talon(2);
    armMotor = new Talon(3);
    winchMotor = new Talon(4);
    intakeMotor = new Talon(5);

    shiftingSolenoid = new Solenoid(1);
    kickUpSolenoid = new Solenoid(2);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(4);

    gyro = new GyroManager();

    leftDriveEncoder = new Encoder(1, 2);
    leftDriveEncoder->Start();
    rightDriveEncoder = new Encoder(3, 4);
    rightDriveEncoder->Start();

    compressor = new Compressor(14,8);

    drive = new Drive(leftDriveMotors, rightDriveMotors, leftDriveEncoder, rightDriveEncoder);

    SmartDashboard::init();
}

void Robot::RobotInit()
{
    compressor->Start();
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
    gyro->update();

    SmartDashboard::PutNumber("Gyro RAW angle: ", gyro->getAngle());
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
}

void Robot::TestInit()
{
}

void Robot::TestPeriodic()
{
}

START_ROBOT_CLASS(Robot);
