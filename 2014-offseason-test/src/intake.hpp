#ifndef INTAKE_HPP
#define INTAKE_HPP

#include "lib/subsystemBase.hpp"

namespace frc973 {

class Intake : public SubsystemBase
{
public:
    Intake(Victor *motor_, Solenoid *claw_);
    void setBehavior(float speed, bool open);
    void setFangs(bool open);
    void setSpeed(float speed);
    void update();
private:
    Victor *motor;
    Solenoid *claw;

    float intakeSpeed;
};

}

#endif
