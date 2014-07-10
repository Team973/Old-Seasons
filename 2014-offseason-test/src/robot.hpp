#ifndef TEST_ROBOT_H
#define TEST_ROBOT_H

class TestRobot : public IterativeRobot
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
};

#endif
