#include "WPILib.h"
#include "robot.hpp"

TestRobot::TestRobot()
{
    this->SetPeriod(0);
}

void TestRobot::RobotInit() {
}

void TestRobot::DisabledInit() {
}

void TestRobot::DisabledPeriodic() {
}

void TestRobot::AutonomousInit() {
}

void TestRobot::AutonomousPeriodic() {
}

void TestRobot::TeleopInit() {
}

void TestRobot::TeleopPeriodic() {
}

void TestRobot::TestInit() {
}

void TestRobot::TestPeriodic() {
}

START_ROBOT_CLASS(TestRobot);
