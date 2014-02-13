#include "WPILib.h"
#include "drive.hpp"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"
#include "autoManager.hpp"
#include "robot.hpp"

#include "NetworkTables/NetworkTable.h"

Robot::Robot()
{
    this->SetPeriod(0);

    leftDriveMotors = new Talon(1);
    leftDriveMotors->Set(0);
    rightDriveMotors = new Talon(2);
    rightDriveMotors->Set(0);
    armMotor = new Talon(3);
    winchMotor = new Victor(4);
    linearIntakeMotor = new Victor(6);
    crossIntakeMotor = new Victor(7);

    shiftingSolenoid = new Solenoid(5);
    kickUpSolenoid = new Solenoid(4);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(2);


    leftDriveEncoder = new Encoder(2, 3);
    leftDriveEncoder->Start();
    rightDriveEncoder = new Encoder(4, 5);
    rightDriveEncoder->Start();

    armSensorA = new Encoder(6, 7);
    armSensorA->Start();

    intakeBallSensor = new DigitalInput(8);

    winchEncoder = new Encoder(9, 10);
    winchEncoder->Start();
    winchZeroSensor = new DigitalInput(11);
    winchFullCockSensor = new DigitalInput(12);

    compressor = new Compressor(1,8);
    compressor->Start();

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid, kickUpSolenoid, leftDriveEncoder, rightDriveEncoder);
    arm = new Arm(armMotor, armSensorA);
    intake = new Intake(arm, linearIntakeMotor, crossIntakeMotor, clawSolenoid, intakeBallSensor);
    shooter = new Shooter(arm, intake, winchMotor, winchReleaseSolenoid, winchZeroSensor, winchFullCockSensor, winchEncoder);

    autoMode = new AutoManager(drive, shooter, intake, arm);

    stick1 = new Joystick(1);
    stick2 = new Joystick(2);

    autoComplete = false;

    SmartDashboard::init();
}

void Robot::dashboardUpdate()
{
    drive->dashboardUpdate();
    arm->dashboardUpdate();
    shooter->dashboardUpdate();
}

float Robot::deadband(float axis, float threshold)
{
    if ((axis < threshold) && (axis > -threshold))
        return 0.0;
    else
        return axis;
}

void Robot::joystick1() // Driver
{
    // [y]
    DriveY = deadband(stick1->GetY(), 0.1);

    // [x]
    //stick1->GetX();

    // [rx]
    DriveX = deadband(stick1->GetRawAxis(3), 0.1);

    // [ry]
    //stick1->GetRawAxis(4);

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
    kickUp = stick1->GetRawButton(8);

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

void Robot::joystick2() // Co-Driver
{
    // [y]
    //intake->manual(deadband(stick2->GetY(), 0.1));

    // [x]
    //stick2->GetX();

    // [ry]
    //winchMotor->Set(deadband(stick2->GetRawAxis(3), 0.1));

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
    dashboardUpdate();
}

void Robot::AutonomousInit()
{
    autoTimer = new Timer();
    autoTimer->Start();

    drive->resetDrive();
    autoMode->autoSelect(TEST);
    autoMode->Init();
}

void Robot::AutonomousPeriodic()
{
    float AUTO_WAIT_TIME = 1;
    if (autoTimer->Get() >= AUTO_WAIT_TIME)
    {
       if (autoMode->Run())
           autoComplete = true;
    }

    SmartDashboard::PutBoolean("Auto Mode Complete: ", autoComplete);

    dashboardUpdate();
}

void Robot::TeleopInit()
{
}

void Robot::TeleopPeriodic()
{
    joystick1();
    joystick2();

    drive->update(DriveX, DriveY, lowGear, kickUp, quickTurn);
    arm->update();
    shooter->update();
    intake->update();

    dashboardUpdate();

}

void Robot::TestInit()
{
}

void Robot::TestPeriodic()
{
}

START_ROBOT_CLASS(Robot);
