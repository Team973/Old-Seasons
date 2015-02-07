#ifndef SAUROPOD_HPP
#define SAUROPOD_HPP

#include <string>
#include <map>

namespace frc973 {

class PID;
class TrapProfile;

class Sauropod {

    struct Preset {
        float horizProjection;
        float height;
    };

    struct Gains {
        float up[3];
        float down[3];
    };

public:
    Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_);
    void setPreset(std::string preset);
    void setGain(std::string name);
    void setTarget(Preset target);
    bool atTarget();
    float getElevatorHeight();
    float getArmAngle();
    void update();

private:
    void addPreset(std::string name, float horiz, float height);
    void addGain(std::string name, Gains gain);

    bool isPackSafe();
    bool isDropSafe();

    PowerDistributionPanel *pdp;

    VictorSP *elevatorMotor;
    VictorSP *armMotor;
    Encoder *elevatorEncoder;
    Encoder *armEncoder;

    PID *armPID;
    PID *elevatorPID;

    std::string currPreset;
    std::string currGains;

    std::map<std::string, Preset> presets;
    std::map<std::string, Gains> gainSchedule;

    TrapProfile *profile;

    Timer *loopTimer;
};

}

#endif
