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

class AutoManager : public AutoCommand
{
public:
    AutoManager(Drive *drive_, Shooter* shooter_, Intake* intake_, Arm *arm_);
    virtual void Init();
    virtual bool Run();
    void autoSelect(int autoMode);
private:
    Drive *drive;
    Shooter *shooter;
    Intake *intake;
    Arm *arm;
    std::vector<AutoCommand*> commandSequence;
    SequentialCommand *AUTO_SEQUENCE;
};

#endif
