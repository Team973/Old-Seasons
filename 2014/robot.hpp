#include "WPILib"

#ifndef ROBOT_H
#define ROBOT_H

//XXX: rename class when we decide on a robot name
class Robot : public SimpleRobot
{
public:
    Robot();
    void Autonomous();
    void OperatorControl();
    void Test();
};

#endif
