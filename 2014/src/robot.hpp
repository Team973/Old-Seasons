#include "WPILib.h"
#include "drive.hpp"
#include "autoManager.hpp"
//#include "gyro/GyroManager.h"

#ifndef ROBOT_H
#define ROBOT_H

//XXX: rename class when we decide on a robot name
class Robot : public IterativeRobot
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

private:
    void dashboardUpdate();
    float deadband(float axis, float threshold);
    void JoyStick1();
    void JoyStick2();

    Victor *leftDriveMotors;
    Victor *rightDriveMotors;
    Talon *armMotor;
    Talon *winchMotor;
    Victor *intakeMotor;

    //GyroManager *gyro;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;

    Encoder *armSensorA;
    Encoder *armSensorB;
    Encoder *armSensorC;

    Solenoid *shiftingSolenoid;
    Solenoid *kickUpSolenoid;
    Solenoid *clawSolenoid;
    Solenoid *winchReleaseSolenoid;

    Compressor *compressor;

    Gyro *gyro;

    Drive *drive;
    Arm *arm;
    Shooter *shooter;
    Intake *intake;

    AutoManager *autoMode;

    Joystick *stick1;
    Joystick *stick2;

    Timer *autoTimer;

    float DriveY;
    float DriveX;
    bool quickTurn;
    bool lowGear;

};

#endif
