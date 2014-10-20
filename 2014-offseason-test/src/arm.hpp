#ifndef ARM_HPP
#define ARM_HPP

#include "lib/subsystemBase.hpp"

class PID;

class Arm : public SubsystemBase
{
public:
    Arm(Talon *motor_, Encoder *sensor_);
    float getAngle();
    void setBehavior(std::string preset);
    std::string getBehavior();
    void setPreset(std::string state);
    void update();
private:
    std::map<std::string, float> presets;
    std::string currPreset;

    Talon *motor;
    Encoder *sensor;

    PID *armPID;
};
#endif
