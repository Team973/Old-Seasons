#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

class Drive;

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
    Talon *leftFrontDrive;
    Talon *rightFrontDrive;
    Talon *leftBackDrive;
    Talon *rightBackDrive;

    Talon *strafeDrive;

    Joystick *testStick;

    Drive *drive;
};

}

#endif
