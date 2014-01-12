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

    Talon *leftDriveMotors;
    Talon *rightDriveMotors;
    Talon *armMotor;
    Talon *winchMotor;
    Talon *intakeMotor;

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

    Drive *drive;
    Arm *arm;
    Shooter *shooter;
    Intake *intake;

    Joystick *crapStick;

    float TELEOP_LOOP_LAG;
    float AUTO_LOOP_LAG;
};

#endif
