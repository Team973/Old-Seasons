#ifndef ROBOT_H
#define ROBOT_H

class Drive;
class Arm;
class Intake;

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

        float deadband(float x, float threshold);

        Talon *leftFrontDriveMotors;
        Talon *leftBackDriveMotors;
        Talon *rightFrontDriveMotors;
        Talon *rightBackDriveMotors;
        Talon *strafeDriveMotors;
        Talon *armMotor;
        Victor *intakeMotor;

        Solenoid *shiftingSolenoid;
        Solenoid *clawSolenoid;

        Encoder *armEncoder;

        Drive *drive;
        Arm *arm;
        Intake *intake;

        Joystick *driver;
        Joystick *coDriver;
};

#endif
