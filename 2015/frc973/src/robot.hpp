#ifndef ROBOT_HPP
#define ROBOT_HPP

namespace frc973 {

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
};

}

#endif
