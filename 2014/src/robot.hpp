
#ifndef ROBOT_H
#define ROBOT_H

class Shooter;
class Intake;
class Drive;
class Arm;
class KinectHandler;
class HellaBlocker;
class DataLog;

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
    Victor *trussWinchMotor;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;

    Encoder *armSensorA;
    Encoder *armSensorB;

    DigitalInput *intakeBallSensor;

    Encoder *winchEncoder;
    DigitalInput *winchZeroSensor;
    DigitalInput *winchFullCockSensor;
    AnalogChannel *trussWinchPot;

    Encoder *colinGyro;
    Gyro *testGyro;

    Solenoid *blockerSolenoid;
    Solenoid *shiftingSolenoid;
    Solenoid *kickUpSolenoid;
    Solenoid *clawSolenoid;
    Solenoid *winchReleaseSolenoid;
    Solenoid *autoCorralSolenoid;

    Compressor *compressor;

    HellaBlocker *blocker;

    Drive *drive;
    Arm *arm;
    Shooter *shooter;
    Intake *intake;

    AutoManager *autoMode;
    bool autoComplete;
    Timer* autoSafetyTimer;

    Joystick *stick1;
    Joystick *stick2;

    KinectStick *leftAutoControl;
    KinectStick *rightAutoControl;
    KinectHandler *kinect;

    Timer *autoTimer;
    Timer *controlTimer;

    DriverStationLCD *dsLCD;
    DriverStation *ds;

    DataLog *robotLog;
    DataLog *voltageLog;

    float DriveY;
    float DriveX;
    bool quickTurn;
    bool lowGear;
    bool kickUp;
    float prevCoDriverDPad;
    int autoSelectMode;
    float autoDistance;
    bool areWeHot;
    int directionFlag;

    bool deBugMode;

};

#endif
