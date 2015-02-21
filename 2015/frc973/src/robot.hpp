#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Intake;
class Locator;
class XYManager;
class ControlMap;
class AutoManager;
class StateManager;
class ControlManager;
class SPIGyro;

class Robot : public IterativeRobot
{
public:
    Robot();
    void RobotInit();
    void DisabledInit();
    void DisabledPeriodic();
    void AutonomousInit();
    void AutonomousPeriodic();
    void TeleopInit();
    void TeleopPeriodic();
    void TestPeriodic();
private:
    static void* runLogger(void*);
    static void* runTxtIO(void*);
    static void* runServer(void*);

    void dashboardUpdate();

    VictorSP *leftDriveMotors;
    VictorSP *rightDriveMotors;
    VictorSP *elevatorMotor;
    VictorSP *armMotor;
    VictorSP *leftIntakeMotor;
    VictorSP *rightIntakeMotor;
    VictorSP *humanFeederIntakeMotor;
    VictorSP *whipMotor;
    VictorSP *containerGrabberMotor;

    Solenoid *humanFeederSolenoid;
    Solenoid *floorSolenoid;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *elevatorEncoder;
    Encoder *armEncoder;

    SPIGyro *gyro;

    AnalogInput *whipPot;
    AnalogInput *armPot;
    AnalogInput *elevatorPot;
    AnalogInput *containerGrabberPot;

    DigitalInput *toteSensor;
    DigitalInput *elevatorZeroPulse;
    DigitalInput *armZeroPulse;
    DigitalInput *armUpPulse;
    DigitalInput *airPressureSwitch;
    DigitalInput *containerGrabberUP;
    DigitalInput *containerGrabberDOWN;

    Relay *compressor;

    Joystick *driver;
    Joystick *coDriver;

    Locator *locator;

    XYManager *xyManager;

    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;

    ControlMap *controls;

    StateManager *stateManager;

    ControlManager *controlManager;

    AutoManager *autoManager;
};

}

#endif
