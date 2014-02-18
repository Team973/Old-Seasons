#include "WPILib.h"
#include "drive.hpp"
#include "autoManager.hpp"
#include "auto/sequentialCommand.hpp"
#include "auto/autoCommand.hpp"
#include <vector>

#ifndef ROBOT_H
#define ROBOT_H

class Shooter;
class Intake;
class Drive;
class Arm;
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
    void joystick1();
    void joystick2();

    Talon *leftDriveMotors;
    Talon *rightDriveMotors;
    Talon *armMotor;
    Victor *winchMotor;
    Victor *linearIntakeMotor;
    Victor *crossIntakeMotor;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;

    Encoder *armSensorA;
    Encoder *armSensorB;

    DigitalInput *intakeBallSensor;

    Encoder *winchEncoder;
    DigitalInput *winchZeroSensor;
    DigitalInput *winchFullCockSensor;

    Encoder *colinGyro;

    Solenoid *shiftingSolenoid;
    Solenoid *kickUpSolenoid;
    Solenoid *clawSolenoid;
    Solenoid *winchReleaseSolenoid;

    Compressor *compressor;

    Drive *drive;
    Arm *arm;
    Shooter *shooter;
    Intake *intake;

    AutoManager *autoMode;
    bool autoComplete;

    Joystick *stick1;
    Joystick *stick2;

    Timer *autoTimer;

    float DriveY;
    float DriveX;
    bool quickTurn;
    bool lowGear;
    bool kickUp;

};

#endif
