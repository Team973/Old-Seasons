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
    Sauropod();
    void setPreset(std::string preset);
    void setTarget(Preset target);
    void update();

private:
    void addPreset(std::string name, float horiz, float height);

    PID *armPID;
    PID *elevatorPID;

    std::string currPreset;

    std::map<std::string, Preset> presets;
};

}

#endif
