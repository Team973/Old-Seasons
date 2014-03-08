#include "WPILib.h"
#include "auto/autoCommand.hpp"
#include "auto/sequentialCommand.hpp"
#include <vector>
#include "drive.hpp"
#include "shooter.hpp"
#include "intake.hpp"
#include "arm.hpp"

#ifndef AUTO_MANAGER_H
#define AUTO_MANAGER_H

#define TEST 1
#define ONE_BALL_SIMPLE 2
#define ONE_BALL_IN_MOVEMENT 3
#define NO_AUTO 4

class AutoManager : public AutoCommand
{
public:
    AutoManager(Drive *drive_, Shooter* shooter_, Intake* intake_, Arm *arm_);
    virtual void Init();
    virtual bool Run();
    void autoSelect(int autoMode);
    void reset();
private:
    Drive *drive;
    Shooter *shooter;
    Intake *intake;
    Arm *arm;
    std::vector<AutoCommand*> commandSequence;
    SequentialCommand *AUTO_SEQUENCE;
};

#endif
