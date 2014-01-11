#include "WPILib.h"
#include "drive.hpp"
//#include "gyro/GyroManager.h"

#ifndef ROBOT_H
#define ROBOT_H

//XXX: rename class when we decide on a robot name
class Robot : /*public SimpleRobot*/ public IterativeRobot
{
public:
    Robot();
    virtual void RobotInit();
    virtual void DisabledInit();
    virtual void DisabledPeriodic();
    virtual void AutonomousInit();
    virtual void AutonomousPeriodic();
    virtual void TeleopInit();
    virtual void TeleopPeriodic();
    virtual void TestInit();
    virtual void TestPeriodic();

    //virtual void OperatorControl();

private:
    Talon *motor;
    /*
    Talon *leftDriveMotors;
    Talon *rightDriveMotors;
    Talon *armMotor;
    Talon *winchMotor;
    Talon *intakeMotor;

    //GyroManager *gyro;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;

    Solenoid *shiftingSolenoid;
    Solenoid *kickUpSolenoid;
    Solenoid *clawSolenoid;
    Solenoid *winchReleaseSolenoid;

    Compressor *compressor;

    Drive *drive;
    */
};

#endif
