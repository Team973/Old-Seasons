
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

    float limit(float x);

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

    Solenoid *blockerSolenoid;
    Solenoid *shiftingSolenoid;
    Solenoid *kickUpSolenoid;
    Solenoid *clawSolenoid;
    Solenoid *winchReleaseSolenoid;
    Solenoid *autoCorralSolenoid;

    Compressor *compressor;

    Drive *drive;
    Arm *arm;
    Shooter *shooter;
    Intake *intake;

    AutoManager *autoMode;
    bool autoComplete;
    Timer* autoSafetyTimer;

    Joystick *stick1;
    Joystick *stick2;

    Timer *autoTimer;

    DriverStationLCD *dsLCD;

    float DriveY;
    float DriveX;
    bool quickTurn;
    bool lowGear;
    bool kickUp;
    float prevCoDriverDPad;
    int autoSelectMode;

};

#endif
