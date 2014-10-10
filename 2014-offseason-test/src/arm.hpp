#ifndef ARM_HPP
#define ARM_HPP

class PID;

class Arm
{
public:
    Arm(Talon *motor_, Encoder *sensor_);
    float getAngle();
    void setBehavior(std::string preset);
    void update();
private:
    std::map<std::string, float> presets;
    std::string lastPreset;

    Talon *motor;
    Encoder *sensor;

    PID *armPID;
};
#endif
