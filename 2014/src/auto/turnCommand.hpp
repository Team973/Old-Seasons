#include "autoCommand.hpp"
#include "../pid.hpp"

#ifndef TURN_COMMAND_H
#define TURN_COMMAND_H

class TurnCommand : public AutoCommand
{
public:
    TurnCommand(Drive* drive_, float targetAngle_, float timeout_, float turnPrecision_=5, float turnCap_=0.7);
    virtual void Init();
    virtual bool Run();
private:
    PID *rotatePID;

    Drive *drive;
    float targetAngle;
    float turnPrecision;
    float turnCap;
    float turnInput;
};

#endif
