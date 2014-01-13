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
    TELEOP_LOOP_LAG = 0.005;
    AUTO_LOOP_LAG = 0.005 * 1.50;

    leftDriveMotors = new Victor(1);
    rightDriveMotors = new Victor(2);
    armMotor = new Talon(5);
    winchMotor = new Talon(4);
    intakeMotor = new Victor(3);

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

    //compressor = new Compressor(14,8);
    //compressor->Start();

    drive = new Drive(leftDriveMotors, rightDriveMotors, leftDriveEncoder, rightDriveEncoder);
    arm = new Arm(armMotor, armSensorA, armSensorB, armSensorC);
    shooter = new Shooter(winchMotor, winchReleaseSolenoid);
    intake = new Intake(intakeMotor, clawSolenoid);

    crapStick = new Joystick(1);

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

float Robot::deadband(float axis, float threshold)
{
    if ((axis < threshold) && (axis > -threshold))
        return 0.0;
    else
        return axis;
}

void Robot::OperatorControl()
{
    while (IsOperatorControl())
    {
        //crap controls
        intake->manualIN(crapStick->GetRawButton(7));
        intake->manualOUT(crapStick->GetRawButton(8));

        // Updates
        drive->update(deadband(-crapStick->GetRawAxis(3), 0.1), deadband(crapStick->GetY(), 0.1), crapStick->GetRawButton(6), crapStick->GetRawButton(5));
        arm->update();
        shooter->update();
        intake->update();

        // wait for the ds
        //Wait(TELEOP_LOOP_LAG);
    }
}

void Robot::Test()
{
}

START_ROBOT_CLASS(Robot);
