#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

class Drive;
class Locator;
class XYManager;
class AutoManager;

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

    Talon *strafeDrive;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *gyro;
    Encoder *elevatorEncoder;

    Joystick *testStick;

    Locator *locator;

    XYManager *xyManager;

    Drive *drive;

    AutoManager *autoManager;
};

}

#endif
