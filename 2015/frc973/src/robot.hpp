#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

class Drive;
class Locator;
class XYManager;

class Robot : public IterativeRobot
{
public:
    Robot();
    void RobotInit();
    void AutonomousInit();
    void AutonomousPeriodic();
    void TeleopInit();
    void TeleopPeriodic();
    void TestPeriodic();
private:
    static void* runLogger(void*);
    static void* runTxtIO(void*);

    Talon *leftFrontDrive;
    Talon *rightFrontDrive;
    Talon *leftBackDrive;
    Talon *rightBackDrive;

    Talon *strafeDrive;

    Encoder *leftDriveEncoder;
    Encoder *rightDriveEncoder;
    Encoder *gyro;

    Joystick *testStick;

    Locator *locator;

    XYManager *xyManager;

    Drive *drive;
};

}

#endif
