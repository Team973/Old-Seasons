#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Locator;
class XYManager;
class ControlMap;
class AutoManager;
class StateManager;
class ControlManager;

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

    void dashboardUpdate();

    Talon *leftFrontDrive;
    Talon *rightFrontDrive;
    Talon *leftBackDrive;
    Talon *rightBackDrive;
    VictorSP *elevatorMotor;
    VictorSP *armMotor;

    Talon *strafeDrive;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *gyro;
    Encoder *elevatorEncoder;
    Encoder *armEncoder;

    Joystick *driver;
    Joystick *coDriver;

    Locator *locator;

    XYManager *xyManager;

    Drive *drive;
    Sauropod *sauropod;

    ControlMap *controls;

    StateManager *stateManager;

    ControlManager *controlManager;

    AutoManager *autoManager;
};

}

#endif
