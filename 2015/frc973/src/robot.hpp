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

    const static int CARBON_FIBER = 1;
    const static int ALUMINUM = 2;

    int grabberType;

    const static int FAST = 1;
    const static int SLOW = 2;

    int grabberSpeed;

    int autoType;

    bool autoRan;

    VictorSP *leftDriveMotors;
    VictorSP *rightDriveMotors;
    VictorSP *elevatorMotor;
    VictorSP *leftIntakeMotor;
    VictorSP *rightIntakeMotor;
    VictorSP *containerGrabberMotor;
    Talon *leftArmMotors;
    Talon *rightArmMotors;
    
    CANTalon *leftGrabberMotorA;
    CANTalon *leftGrabberMotorB;
    CANTalon *rightGrabberMotorA;
    CANTalon *rightGrabberMotorB;

    Solenoid *intakeSolenoid;
    Solenoid *humanLoadFunnelSolenoid;
    Solenoid *footSolenoid;
    Solenoid *clawClampSolenoid;
    Solenoid *clawBrakeSolenoid;
    Solenoid *grabberSolenoid;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *elevatorEncoder;
    Encoder *gyro;
    Encoder *leftArmEncoder;
    Encoder *rightArmEncoder;

    SPIGyro *spiGyro;

    AnalogInput *elevatorPot;

    DigitalInput *toteSensor;
    DigitalInput *airPressureSwitch;
    DigitalInput *autoSwitchA;
    DigitalInput *autoSwitchB;

    Relay *compressor;
    Relay *statusLEDA;
    Relay *statusLEDB;
    Relay *statusLEDC;
    Relay *statusLEDD;

    Joystick *driver;
    Joystick *coDriver;
    Joystick *armController;

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
