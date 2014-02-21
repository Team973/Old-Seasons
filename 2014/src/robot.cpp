#include "WPILib.h"
#include "pid.hpp"
#include "drive.hpp"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"
#include "autoManager.hpp"
#include "robot.hpp"
#include <vector>

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

    blockerSolenoid = new Solenoid(6);
    shiftingSolenoid = new Solenoid(5);
    kickUpSolenoid = new Solenoid(4);
    clawSolenoid = new Solenoid(3);
    winchReleaseSolenoid = new Solenoid(2);
    autoCorralSolenoid = new Solenoid(1);


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

    colinGyro = new Encoder(13, 14);
    colinGyro->Start();

    compressor = new Compressor(1,1);
    compressor->Start();

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid, kickUpSolenoid, leftDriveEncoder, rightDriveEncoder);
    arm = new Arm(armMotor, armSensorA);
    intake = new Intake(arm, shooter, linearIntakeMotor, crossIntakeMotor, clawSolenoid, autoCorralSolenoid, intakeBallSensor);
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
    intake->dashboardUpdate();
    SmartDashboard::PutNumber("Gyro: ", colinGyro->Get());
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
    DriveX = -deadband(stick1->GetRawAxis(3), 0.1);

    // [ry]
    //stick1->GetRawAxis(4);

    // [1]
    if (stick1->GetRawButton(1))
    {
        leftDriveMotors->Set(1);
        rightDriveMotors->Set(1);
    }

    // [2]
    if (stick1->GetRawButton(2))
    {
        leftDriveMotors->Set(-1);
        rightDriveMotors->Set(-1);
    }

    // [3]
    if (stick1->GetRawButton(3))
    {
        leftDriveMotors->Set(0);
        rightDriveMotors->Set(0);
    }

    // [4]
    //stick1->GetRawButton(4)

    // [5]
    quickTurn = stick1->GetRawButton(5);

    // [6]
    lowGear = stick1->GetRawButton(6);

    // [7]
    //stick1->GetRawButton(7)

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
    intake->manual(deadband(stick2->GetY(), 0.1));

    // [x]
    //stick2->GetX();

    // [ry]
    //stick2->GetRawAxis(3)

    // [rx]
    //stick2->GetRawAxis(4);

    // [1]
    if (stick2->GetRawButton(1))
    {
        arm->setPreset(INTAKE);
    }

    // [2]
    if (stick2->GetRawButton(2))
    {
        arm->setPreset(SHOOTING);
    }

    // [3]
    if (stick2->GetRawButton(3))
    {
        shooter->cock(HALF_COCK);
    }

    // [4]
    if (stick2->GetRawButton(4))
    {
        shooter->cock(FULL_COCK);
    }

    // [5]
    if (stick2->GetRawButton(5))
    {
        winchReleaseSolenoid->Set(true);
    }
    

    // [6]
    //stick2->GetRawButton(6)

    // [7]
    if (stick2->GetRawButton(7))
    {
        intake->setFangs(true);
    }

    // [8]
    if (stick2->GetRawButton(8))
    {
        intake->setFangs(false);
    }

    // [9]
    autoCorralSolenoid->Set(stick2->GetRawButton(9));

    // [10]
    blockerSolenoid->Set(stick2->GetRawButton(10));

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
