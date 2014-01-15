#include "WPILib.h"
#include "drive.hpp"
//#include "gyro/GyroManager.h"

#ifndef ROBOT_H
#define ROBOT_H

//XXX: rename class when we decide on a robot name
class Robot : public SimpleRobot
{
public:
    Robot();
    virtual void Autonomous();
    virtual void OperatorControl();
    virtual void Test();

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

    Joystick *stick1;
    Joystick *stick2;

    float DriveY;
    float DriveX;
    bool quickTurn;
    bool lowGear;

    float TELEOP_LOOP_LAG;
    float AUTO_LOOP_LAG;
};

#endif
