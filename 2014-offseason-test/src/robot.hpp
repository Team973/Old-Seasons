#ifndef ROBOT_H
#define ROBOT_H

class Drive;
class GreyJoy;

class Robot : public IterativeRobot
{
public:
        TestRobot();
        void RobotInit();
        void DisabledInit();
        void DisabledPeriodic();
        void AutonomousInit();
        void AutonomousPeriodic();
        void TeleopInit();
        void TeleopPeriodic();
        void TestInit();
        void TestPeriodic();
private:
        Talon *leftDriveMotors;
        Talon *rightDriveMotors;
        Solenoid *shiftingSolenoid;

        Drive *drive;

        GreyJoy *driver;
        GreyJoy *coDriver;
};

#endif
