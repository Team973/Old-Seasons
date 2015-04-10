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
class ContainerGrabber;
class GrabManager;

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

    void runCompressor();

    void dashboardUpdate();

    bool autoRan;

    VictorSP *leftDriveMotors;
    VictorSP *rightDriveMotors;
    VictorSP *elevatorMotor;
    VictorSP *leftIntakeMotor;
    VictorSP *rightIntakeMotor;
    VictorSP *containerGrabberMotor;
    
    CANTalon *leftGrabberMotorA;
    CANTalon *leftGrabberMotorB;
    CANTalon *rightGrabberMotorA;
    CANTalon *rightGrabberMotorB;

    Solenoid *intakeSolenoid;
    Solenoid *humanLoadFunnelSolenoid;
    Solenoid *footSolenoid;
    Solenoid *clawClampSolenoid;
    Solenoid *clawBrakeSolenoid;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *elevatorEncoder;
    Encoder *gyro;

    //SPIGyro *spiGyro;

    AnalogInput *elevatorPot;

    DigitalInput *toteSensor;
    DigitalInput *airPressureSwitch;

    Relay *compressor;
    Relay *statusLED;

    Joystick *driver;
    Joystick *coDriver;
    Joystick *oliverStick;

    Locator *locator;

    XYManager *xyManager;

    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;
    ContainerGrabber *grabber;

    GrabManager *grabManager;

    ControlMap *controls;

    StateManager *stateManager;

    ControlManager *controlManager;

    AutoManager *autoManager;
};

}

#endif
