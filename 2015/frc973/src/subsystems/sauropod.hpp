#ifndef SAUROPOD_HPP
#define SAUROPOD_HPP

#include <string>
#include <map>
#include <vector>
#include <queue>

namespace frc973 {

class PID;
class TrapProfile;
class FlagAccumulator;
class RampedOutput;

class Sauropod {

    struct Preset {
        float height;
    };

    struct Gains {
        float up[3];
        float down[3];
    };

public:
    Sauropod(VictorSP* elevatorMotor_, Encoder* elevatorEncoder_, Solenoid *clawClamp_, Solenoid *clawBrake_);
    void setGain(std::string name);
    void setTarget(Preset target);
    void setPreset(std::string name);
    std::string getCurrPreset();
    bool isCurrPreset(std::string preset);
    void incrementElevator(float increment);
    bool motionDone();
    bool atTarget();
    void setElevatorManual(float speed);
    float getElevatorHeight();
    float getElevatorVelocity();
    float getElevatorCurrent();
    void update();
    bool inCradle();
    bool lotsoTotes();

private:

    void addPreset(std::string name, float height);
    void addGain(std::string name, Gains gain);

    PowerDistributionPanel *pdp;

    VictorSP *elevatorMotor;
    Encoder *elevatorEncoder;
    Solenoid *clawClamp;
    Solenoid *clawBrake;

    PID *elevatorPID;

    std::string currPreset;
    std::string currGains;

    std::map<std::string, Preset> presets;
    std::map<std::string, Gains> gainSchedule;

    FlagAccumulator *accumulator;

    Timer *doneTimer;

    float manualElevatorSpeed;

    FlagAccumulator *toteAccumulator;
    bool muchoTotes;

    float elevatorIncrement;
};

}

#endif
