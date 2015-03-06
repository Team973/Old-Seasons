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
    void setPreset(std::string preset);
    void setGain(std::string name);
    void setTarget(Preset target);
    void incrementElevator(float increment);
    bool sequenceDone();
    bool atTarget();
    void addToQueue(std::string preset);
    void setElevatorManual(float speed);
    void clearQueue();
    float getElevatorHeight();
    float getElevatorVelocity();
    float getElevatorCurrent();
    void update();
    void createPath(int dest);
    int getCurrPath();
    bool inCradle();
    bool lotsoTotes();

    const static int NONE = -1;
    const static int IDLE = 1;
    const static int PICKUP = 2;
    const static int READY = 4;
    const static int RESTING = 5;
    const static int CONTAINER = 7;

private:

    void addPreset(std::string name, float height);
    void addGain(std::string name, Gains gain);

    void executeQueue();

    PowerDistributionPanel *pdp;

    VictorSP *elevatorMotor;
    Encoder *elevatorEncoder;
    Solenoid *clawClamp;
    Solenoid *clawBrake;

    PID *elevatorPID;

    std::string currPreset;
    std::string currGains;
    int currPath;

    std::map<std::string, Preset> presets;
    std::map<std::string, Gains> gainSchedule;

    std::queue<std::string> waypointQueue;

    FlagAccumulator *accumulator;

    Timer *doneTimer;

    float manualElevatorSpeed;

    FlagAccumulator *toteAccumulator;
    bool muchoTotes;

    float elevatorIncrement;
};

}

#endif
