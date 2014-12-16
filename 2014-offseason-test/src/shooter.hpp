#ifndef SHOOTER_HPP
#define SHOOTER_HPP

#include <string>
#include "WPILib.h"
#include "lib/subsystemBase.hpp"

namespace frc973 {

class PID;

class Shooter : public SubsystemBase
{
public:
    Shooter(Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *fullCockPoint_);
    void setBehavior(std::string preset);
    void cock(std::string preset);
    void wantFire();
    bool isFiring();
    void update();
private:
    void setTarget(float target);

    Victor *winchMotor;
    Solenoid *winchRelease;
    DigitalInput *fullCockPoint;

    PID *winchPID;

    Timer *cockTimer;
    Timer *fireTimer;

    std::string currPreset;
    bool firing;

    map<std::string, float> presets;
};

}

#endif
