#ifndef SAUROPOD_HPP
#define SAUROPOD_HPP

#include <string>
#include <map>

namespace frc973 {

class PID;

class Sauropod {

    struct Preset {
        float horizProjection;
        float height;
    };

public:
    Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_);
    void setPreset(std::string preset);
    void setTarget(Preset target);
    float getElevatorHeight();
    float getArmAngle();
    void update();

private:
    void addPreset(std::string name, float horiz, float height);
    bool isPackSafe();
    bool isDropSafe();

    VictorSP *elevatorMotor;
    VictorSP *armMotor;
    Encoder *elevatorEncoder;
    Encoder *armEncoder;

    PID *armPID;
    PID *elevatorPID;

    std::string currPreset;

    bool inCradle;

    std::map<std::string, Preset> presets;
};

}

#endif
