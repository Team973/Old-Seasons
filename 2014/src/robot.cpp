#include "WPILib.h"
#include "pid.hpp"
#include "drive.hpp"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"
#include "autoManager.hpp"
#include "robot.hpp"
#include <vector>
#include "kinectHandler.hpp"
#include "hellaBlocker.hpp"

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

    testGyro = new Gyro(1,1);

    compressor = new Compressor(1,1);
    compressor->Start();

    blocker = new HellaBlocker(blockerSolenoid, autoCorralSolenoid);

    drive = new Drive(leftDriveMotors, rightDriveMotors, shiftingSolenoid, kickUpSolenoid, leftDriveEncoder, rightDriveEncoder, colinGyro, testGyro);
    arm = new Arm(armMotor, armSensorA);
    intake = new Intake(linearIntakeMotor, crossIntakeMotor, clawSolenoid, autoCorralSolenoid, intakeBallSensor);
    shooter = new Shooter(winchMotor, winchReleaseSolenoid, winchZeroSensor, winchFullCockSensor, winchEncoder);

    shooter->initialize(arm, intake);
    intake->initialize(arm, shooter);
    arm->initialize(intake);

    stick1 = new Joystick(1);
    stick2 = new Joystick(2);

    leftAutoControl = new KinectStick(1);
    rightAutoControl = new KinectStick(2);
    kinect = new KinectHandler(leftAutoControl, rightAutoControl);

    autoMode = new AutoManager(drive, shooter, intake, arm, kinect, blocker);
    autoSelectMode = ONE_BALL_SIMPLE;
    controlTimer= new Timer();
    initialAutoDistance = 4;
    finalAutoDistance = -4;
    autoDriveTime = 4.5;
    autoSide = "left";
    autoLane = "mid";

    autoComplete = false;

    deBugMode = false;

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
    SmartDashboard::PutNumber(" Test Gyro: ", testGyro->GetAngle());
    SmartDashboard::PutNumber("Drive Distance: ", drive->getWheelDistance());

    SmartDashboard::PutBoolean("Left Hand: ", kinect->getLeftHand());
    SmartDashboard::PutBoolean("Right Hand: ", kinect->getRightHand());

    if (deBugMode)
    {
        SmartDashboard::PutNumber("Right Drive: ", rightDriveMotors->Get());
        SmartDashboard::PutNumber("Left Drive: ", leftDriveMotors->Get());
        SmartDashboard::PutNumber("Left Distance: ", drive->getLeftDistance());
        SmartDashboard::PutNumber("Right Distance: ", drive->getRightDistance());
        SmartDashboard::PutNumber("X: ", drive->getX());
        SmartDashboard::PutNumber("Y: ", drive->getY());
    }
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
    if (stick2->GetRawButton(9) && stick2->GetRawButton(10))
    {
        arm->reset();
    }
    else if (stick2->GetRawButton(9) && stick2->GetRawButton(10) && stick2->GetRawButton(7))
    {
        arm->zeroEncoder();
    }

    // [10]
    //shooter->killShooter(stick2->GetRawButton(10));

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

    if (stick2->GetRawButton(4) && controlTimer->Get() >= .15)
    {
        autoSelectMode += 1;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(2) && controlTimer->Get() >= .15)
    {
        autoSelectMode -= 1;
        controlTimer->Reset();
    }

    if (autoSelectMode > DRIVE_ONLY)
        autoSelectMode = TEST;
    else if (autoSelectMode < TEST)
        autoSelectMode = DRIVE_ONLY;

    if (stick2->GetRawButton(1) && controlTimer->Get() >= .15)
    {
        initialAutoDistance += .5;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(3) && controlTimer->Get() >= .15)
    {
        initialAutoDistance -= .5;
        controlTimer->Reset();
    }

    if (stick2->GetRawButton(5) && controlTimer->Get() >= .15)
    {
        finalAutoDistance += .5;
        controlTimer->Reset();
    }
    else if (stick2->GetRawButton(6) && controlTimer->Get() >= .15)
    {
        finalAutoDistance -= .5;
        controlTimer->Reset();
    }

    if (initialAutoDistance >= 8)
    {
        initialAutoDistance = 7.5;
        autoSelectMode = BLOCK_LOW_GOAL;
    }
    else if (initialAutoDistance <= -8)
    {
        initialAutoDistance = -7.5;
        autoSelectMode = BLOCK_LOW_GOAL;
    }

    if (stick2->GetRawButton(9))
    {
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line5,"Side: %s", "left");
        autoSide = "left";
    }
    else if (stick2->GetRawButton(10))
    {
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line5,"Side: %s", "right");
        autoSide = "right";
    }

    if (stick1->GetRawButton(1))
    {
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Lane: %f", "close");
        autoLane = "close";
    }
    else if (stick1->GetRawButton(2))
    {
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Lane: %f", "mid");
        autoLane = "mid";
    }
    else if (stick1->GetRawButton(3))
    {
        dsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Lane: %f", "far");
        autoLane = "far";
    }

    switch(autoSelectMode)
    {
        case TEST:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "test");
            break;
        case ONE_BALL_SIMPLE:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "1 ball");
            break;
        case DRIVE_ONLY:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "move only");
            break;
        case NO_AUTO:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "no auto");
            break;
        case BLOCK_SIMPLE:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "b simple");
            break;
        case BLOCK_HOT:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "b hot");
            break;
        case BLOCK_DOUBLE:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "b double");
            break;
        case BLOCK_DOUBLE_HOT:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "b hot double");
            break;
        case BLOCK_LOW_GOAL:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "b low goal");
            break;
        case HOT_CENTER_TWO_BALL:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "h 2 ball c");
            break;
        case HOT_CENTER_ONE_BALL:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "h 1 ball c");
            break;
        case HOT_SIDE_ONE_BALL:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "h 1 ball s");
            break;
        case TWO_BALL:
            dsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Mode: %s", "2 ball");
    }

    dsLCD->PrintfLine(DriverStationLCD::kUser_Line2,"Initial Dist: %f", initialAutoDistance);
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line3,"Final Dist: %f", finalAutoDistance);
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line4,"Switch Time: %f", autoDriveTime);

    if (stick1->GetRawButton(9) && stick1->GetRawButton(10))
    {
        deBugMode = true;
    }

    dashboardUpdate();
    dsLCD->UpdateLCD();
}

void Robot::AutonomousInit()
{
    controlTimer->Stop();

    autoTimer = new Timer();
    autoTimer->Start();
    autoTimer->Reset();
    autoMode->inject(autoTimer);

    autoSafetyTimer = new Timer();
    autoSafetyTimer->Start();
    autoSafetyTimer->Reset();

    blocker->reset();

    drive->resetDrive();
    autoMode->reset();
    autoMode->setDriveTime(autoDriveTime);
    autoMode->setAutoSide(autoSide);
    autoMode->setAutoLane(autoLane);
    autoMode->setInitialDistance(initialAutoDistance);
    autoMode->setFinalDistance(finalAutoDistance);
    autoMode->autoSelect(autoSelectMode);
    autoMode->Init();
}

void Robot::AutonomousPeriodic()
{
    GetWatchdog().Feed();
   if (autoMode->Run())
       autoComplete = true;

    GetWatchdog().Feed();

    arm->update();
    shooter->update();
    intake->update();
    kinect->update();
    drive->PIDupdate();
    dashboardUpdate();
    GetWatchdog().Feed();
}

void Robot::TeleopInit()
{
    shooter->cock(NO_COCK);
    drive->update(0, 0, true, false, false, false);
    intake->stop();
    blockerSolenoid->Set(false);
    autoCorralSolenoid->Set(false);
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
    dsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Arm Angle: %f", arm->getRawAngle());
    dsLCD->UpdateLCD();

}

void Robot::TestInit()
{
}

void Robot::TestPeriodic()
{
}

START_ROBOT_CLASS(Robot);
