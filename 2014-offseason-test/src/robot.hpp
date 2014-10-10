#ifndef ROBOT_H
#define ROBOT_H

class Drive;

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
        void TestInit();
        void TestPeriodic();
private:
        Talon *leftDriveMotors;
        Talon *rightDriveMotors;
        Solenoid *shiftingSolenoid;

        Drive *drive;
};

#endif
