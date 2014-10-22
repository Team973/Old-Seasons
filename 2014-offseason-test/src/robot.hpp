#ifndef ROBOT_H
#define ROBOT_H

class Drive;
class Arm;
class Intake;
class Shooter;

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

        std::string boolToString(bool b);

        Talon *leftFrontDriveMotors;
        Talon *leftBackDriveMotors;
        Talon *rightFrontDriveMotors;
        Talon *rightBackDriveMotors;
        Talon *strafeDriveMotors;
        Talon *armMotor;
        Victor *intakeMotor;
        Victor *winchMotor;

        Solenoid *shiftingSolenoid;
        Solenoid *backShiftingSolenoid;
        Solenoid *clawSolenoid;
        Solenoid *winchReleaseSolenoid;

        Encoder *armEncoder;

        DigitalInput *winchFullCockSensor;

        Compressor *compressor;

        Drive *drive;
        Arm *arm;
        Intake *intake;
        Shooter *shooter;

        Joystick *driver;
        Joystick *coDriver;

        DriverStationLCD *dsLCD;
};

#endif
