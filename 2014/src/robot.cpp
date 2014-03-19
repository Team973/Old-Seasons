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

    winchEncoder = new Encoder(9, 10, false, CounterBase::k1X);
    winchEncoder->Start();
    winchZeroSensor = new DigitalInput(11);
    winchFullCockSensor = new DigitalInput(12);

    colinGyro = new Encoder(13, 14);
    colinGyro->Start();

    compressor = new Compressor(1,1);
    compressor->Start();

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid, kickUpSolenoid, leftDriveEncoder, rightDriveEncoder, colinGyro);
    arm = new Arm(armMotor, armSensorA);
    intake = new Intake(linearIntakeMotor, crossIntakeMotor, clawSolenoid, autoCorralSolenoid, intakeBallSensor);
    shooter = new Shooter(winchMotor, winchReleaseSolenoid, winchZeroSensor, winchFullCockSensor, winchEncoder);

    shooter->initialize(arm, intake);
    intake->initialize(arm, shooter);
    arm->initialize(intake);

    autoMode = new AutoManager(drive, shooter, intake, arm, blockerSolenoid);
    autoSelectMode = NO_AUTO;
    controlTimer= new Timer();
    hellaDistance = 4;

    stick1 = new Joystick(1);
    stick2 = new Joystick(2);

    autoControl = new Joystick(3);

    autoComplete = false;

    prevCoDriverDPad = 0;

    GetWatchdog().SetExpiration(1000);
    GetWatchdog().SetEnabled(true);

    dsLCD = DriverStationLCD::GetInstance();
    SmartDashboard::init();
}

void Robot::dashboardUpdate()
{
    drive->dashboardUpdate();
    arm->dashboardUpdate();
    shooter->dashboardUpdate();
    intake->dashboardUpdate();
    SmartDashboard::PutNumber("Arm Target: ", arm->getTarget());
    SmartDashboard::PutNumber("Gyro: ", drive->getGyroAngle());
    SmartDashboard::PutNumber("Drive Distance: ", drive->getWheelDistance());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line3,"Arm Angle: %f", arm->getRawAngle());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line4,"Left Dist: %f", drive->getLeftDistance());
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line5,"Right Dist: %f", drive->getRightDistance());
    dsLCD->UpdateLCD();
}

float Robot::deadband(float axis, float threshold)
{
    if ((axis < threshold) && (axis > -threshold))
        return 0.0;
    else
        return axis;
}

float Robot::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
}

void Robot::joystick1() // Driver
{
    // [y]
    DriveY = -deadband(stick1->GetY(), 0.1);

    // [x]
    //stick1->GetX();

    // [rx]
    DriveX = deadband(stick1->GetRawAxis(3), 0.1);

    // [ry]
    //stick1->GetRawAxis(4);

    // [1]
    //stick1->GetRawButton(1)

    // [2]
    //stick1->GetRawButton(2)

    // [3]
    //stick1->GetRawButton(3)

    // [4]
    //stick1->GetRawButton(4)

    // [5]
    quickTurn = stick1->GetRawButton(5);

    // [6]
    lowGear = stick1->GetRawButton(6);

    // [7]
    kickUp = stick1->GetRawButton(7);

    // [8]
    if (stick1->GetRawButton(8))
    {
        shooter->fire(true);
    }

    // [9]
    //stick1->GetRawButton(9)

    // [10]
    //stick1->GetRawButton(10)

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

    // [rx]
    //stick2->GetRawAxis(3)

    // [ry]
    arm->ballTrapper(deadband(limit(stick2->GetRawAxis(4)*2), 0.1));

    // [1]
    if (stick2->GetRawButton(1))
    {
        arm->setPreset(PSEUDO_INTAKE);
        intake->setFangs(false, false);
    }

    // [2]
    if (stick2->GetRawButton(2))
    {
        arm->setPreset(INTAKE);
        intake->setFangs(false, false);
    }

    // [3]
    if (stick2->GetRawButton(3))
    {
        arm->setPreset(CLOSE_SHOT);
    }

    // [4]
    if (stick2->GetRawButton(4))
    {
        shooter->cock(FULL_COCK);
    }

    // [5]

    if (stick2->GetRawButton(5))
    {
        arm->setPreset(STOW);
    }

    // [6]
    if (stick2->GetRawButton(6))
    {
        arm->setPreset(SHOOTING);
    }

    // [7]
    if (stick2->GetRawButton(7))
    {
        intake->setFangs(false, true);
    }

    // [8]
    if (stick2->GetRawButton(8))
    {
        intake->setFangs(true, false);
    }

    // [9]
    //stick2->GetRawButton(9)

    // [10]
    shooter->killShooter(stick2->GetRawButton(10));

    // [11]
    //stick2->GetRawButton(11);

    // [12]
    //stick2->GetRawButton(12);

    // [hatx]
    //stick2->GetRawAxis(5);

    // [haty]
    float increment = 0.5;
    double axis = stick2->GetRawAxis(6);

    if (axis > 0.5 && prevCoDriverDPad <= 0.5)
    {
        arm->setTarget(arm->getTarget() - increment);
    }
    if (axis < -0.5 && prevCoDriverDPad >= -0.5)
    {
        arm->setTarget(arm->getTarget() + increment);
    }
    prevCoDriverDPad = axis;

}

void Robot::RobotInit()
{
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
    GetWatchdog().Feed();
    controlTimer->Start();

    if (stick2->GetRawButton(4) && controlTimer->Get() >= .5)
    {
        autoSelectMode += 1;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(2) && controlTimer->Get() >= .5)
    {
        autoSelectMode -= 1;
        controlTimer->Reset();
    }

    if (autoSelectMode > DRIVE_ONLY)
        autoSelectMode = TEST;
    else if (autoSelectMode < TEST)
        autoSelectMode = DRIVE_ONLY;

    if (stick2->GetRawButton(1) && controlTimer->Get() >= .5)
    {
        hellaDistance += .5;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(3) && controlTimer->Get() >= .5)
    {
        hellaDistance -= .5;
        controlTimer->Reset();
    }

    if (stick2->GetRawButton(5) && controlTimer->Get() >= .5)
    {
        hellaDistance += 2;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(6) && controlTimer->Get() >= .5)
    {
        hellaDistance -= 2;
        controlTimer->Reset();
    }

    if (stick2->GetRawButton(7) && controlTimer->Get() >= .5)
    {
        hellaDistance += 5;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(8) && controlTimer->Get() >= .5)
    {
        hellaDistance -= 5;
        controlTimer->Reset();
    }

    if (hellaDistance > 27)
        hellaDistance = 0;
    else if (hellaDistance < 0)
        hellaDistance = 27;

    switch(autoSelectMode)
    {
    case TEST:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "Test");
        break;
    case ONE_BALL_SIMPLE:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "one ball ss");
        break;
    case ONE_BALL_IN_MOVEMENT:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "one ball ms");
        break;
    case DRIVE_ONLY:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "move only");
        break;
    case NO_AUTO:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "no auto");
        break;
    case HELLAVATOR_FOREWARD:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "h fore");
        break;
    case HELLAVATOR_BACKWARD:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "h back");
        break;
    default:
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Auto Mode: %s", "no auto");
        break;
    }

    dsLCD->PrintfLine(DriverStationLCD::kUser_Line2,"Auto Dist: %f", hellaDistance);

    dashboardUpdate();
    dsLCD->UpdateLCD();
    autoMode->setHellaDistance(hellaDistance);
}

void Robot::AutonomousInit()
{
    controlTimer->Stop();

    autoTimer = new Timer();
    autoTimer->Start();

    autoSafetyTimer = new Timer();
    autoSafetyTimer->Start();

    if (autoSafetyTimer->Get() > 10)
    {
        autoMode->reset();
        autoTimer->Reset();
        autoSafetyTimer->Reset();
    }

    drive->resetDrive();
    autoMode->autoSelect(autoSelectMode);
    autoMode->Init();
}

void Robot::AutonomousPeriodic()
{
    GetWatchdog().Feed();
   if (autoMode->Run())
       autoComplete = true;


    arm->update();
    shooter->update();
    intake->update();
    drive->positionUpdate();
    dashboardUpdate();

}

void Robot::TeleopInit()
{
    shooter->cock(NO_COCK);
    drive->holdPosition(false);
    drive->update(0, 0, false, false, false, false);
    intake->stop();
    blockerSolenoid->Set(false);
}

void Robot::TeleopPeriodic()
{
    GetWatchdog().Feed();
    joystick1();
    joystick2();

    drive->update(DriveX, DriveY, lowGear, kickUp, quickTurn, false);
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
