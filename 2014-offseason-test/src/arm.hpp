#ifndef ARM_HPP
#define ARM_HPP

#include "WPILib.h"
#include <string>
#include "lib/subsystemBase.hpp"

class PID;

class Arm : public SubsystemBase
{
public:
    Arm(Talon *motor_, Encoder *sensor_, AnalogChannel *pot_);
    float getAngle();
    void setBehavior(std::string preset);
    std::string getBehavior();
    bool isShotSafe();
    void setPreset(std::string state);
    void update();
private:
    std::map<std::string, float> presets;
    std::string currPreset;

    Talon *motor;
    Encoder *sensor;
    AnalogChannel *pot;

    PID *armPID;
};
#endif
