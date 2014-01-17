#include "WPILib.h"
#include "drive.hpp"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"
//#include "gyro/GyroManager.h"
#include "robot.hpp"

#include "NetworkTables/NetworkTable.h"

Robot::Robot()
{
    this->SetPeriod(0);

    leftDriveMotors = new Victor(1);
    rightDriveMotors = new Victor(2);
    armMotor = new Talon(5);
    winchMotor = new Talon(4);
    intakeMotor = new Victor(3);

    shiftingSolenoid = new Solenoid(1);
    kickUpSolenoid = new Solenoid(2);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(4);


    leftDriveEncoder = new Encoder(3, 4);
    leftDriveEncoder->Start();
    rightDriveEncoder = new Encoder(5, 6);
    rightDriveEncoder->Start();

    armSensorA = new Encoder(5, 6);
    armSensorA->Start();
    armSensorB = new Encoder(7, 8);
    armSensorB->Start();
    armSensorC = new Encoder(9, 10);
    armSensorC->Start();

    compressor = new Compressor(14,8);
    compressor->Start();

    gyro = new Gyro(1, 1);
    gyro->SetSensitivity(0.00703);
    gyro->Reset();

    drive = new Drive(leftDriveMotors, rightDriveMotors, leftDriveEncoder, rightDriveEncoder, gyro);
    arm = new Arm(armMotor, armSensorA, armSensorB, armSensorC);
    shooter = new Shooter(winchMotor, winchReleaseSolenoid);
    intake = new Intake(intakeMotor, clawSolenoid);

    stick1 = new Joystick(1);
    stick2 = new Joystick(2);

    SmartDashboard::init();

}

void Robot::dashboardUpdate()
{
    drive->dashboardUpdate();
    SmartDashboard::PutNumber("Test: ", 50);
}

float Robot::deadband(float axis, float threshold)
{
    if ((axis < threshold) && (axis > -threshold))
        return 0.0;
    else
        return axis;
}

void Robot::JoyStick1() // Driver
{
    // [y]
    DriveY = deadband(stick1->GetY(), 0.1);

    // [x]
    //stick1->GetX();

    // [ry]
    //stick1->GetRawAxis(3);

    // [rx]
    DriveX = deadband(stick1->GetRawAxis(4), 0.1);

    // [1]
    //stick1->GetRawButton(1);

    // [2]
    //stick1->GetRawButton(2);

    // [3]
    //stick1->GetRawButton(3);

    // [4]
    //stick1->GetRawButton(4);

    // [5]
    quickTurn = stick1->GetRawButton(5);

    // [6]
    lowGear = stick1->GetRawButton(6);

    // [7]
    //stick1->GetRawButton(7);

    // [8]
    //stick1->GetRawButton(8);

    // [9]
    //stick1->GetRawButton(9);

    // [10]
    //stick1->GetRawButton(10);

    // [11]
    //stick1->GetRawButton(11);

    // [12]
    //stick1->GetRawButton(12);

    // [hatx]
    //stick1->GetRawAxis(5);

    // [haty]
    //stick1->GetRawAxis(6);
}

void Robot::JoyStick2() // Co-Driver
{
    // [y]
    //stick2->GetY();

    // [x]
    //stick2->GetX();

    // [ry]
    //stick2->GetRawAxis(3);

    // [rx]
    //stick2->GetRawAxis(4);

    // [1]
    //stick2->GetRawButton(1);

    // [2]
    //stick2->GetRawButton(2);

    // [3]
    //stick2->GetRawButton(3);

    // [4]
    //stick2->GetRawButton(4);

    // [5]
    //stick2->GetRawButton(5);

    // [6]
    //stick2->GetRawButton(6);

    // [7]
    //stick2->GetRawButton(7);

    // [8]
    //stick2->GetRawButton(8);

    // [9]
    //stick2->GetRawButton(9);

    // [10]
    //stick2->GetRawButton(10);

    // [11]
    //stick2->GetRawButton(11);

    // [12]
    //stick2->GetRawButton(12);

    // [hatx]
    //stick2->GetRawAxis(5);

    // [haty]
    //stick2->GetRawAxis(6);
}

void Robot::RobotInit()
{
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
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
    drive->update(DriveX, DriveY, lowGear, quickTurn);
    arm->update();
    shooter->update();
    intake->update();
    intakeMotor->Set(0);

    dashboardUpdate();

}

void Robot::TestInit()
{
}

void Robot::TestPeriodic()
{
}

START_ROBOT_CLASS(Robot);
