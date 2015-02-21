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
        float projection;
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
    bool sequenceDone();
    bool atTarget();
    void addToQueue(std::string preset);
    void clearQueue();
    float getElevatorHeight();
    float getElevatorVelocity();
    float getElevatorCurrent();
    float getArmAngle();
    float getArmVelocity();
    void update();
    void createPath(int dest);
    int getCurrPath();
    bool inCradle();
    bool lotsoTotes();
    void setNumTotes(int num);

    const static int NONE = -1;
    const static int IDLE = 1;
    const static int PICKUP = 2;
    const static int PLATFORM = 3;
    const static int READY = 4;
    const static int RESTING = 5;
    const static int RETURN = 6;

private:

    bool equal(Preset lhs, Preset rhs) {
        return (lhs.projection == rhs.projection) && (lhs.height == rhs.height);
    }

    void addPreset(std::string name, float horiz, float height);
    void addGain(std::string name, Gains gain);

    void executeQueue();

    PowerDistributionPanel *pdp;

    VictorSP *elevatorMotor;
    VictorSP *armMotor;
    Encoder *elevatorEncoder;
    Encoder *armEncoder;

    PID *armPID;
    PID *elevatorPID;

    std::string currPreset;
    std::string currGains;
    int currPath;

    std::map<std::string, Preset> presets;
    std::map<std::string, Gains> gainSchedule;

    bool forceNewTarget;
    std::queue<std::string> waypointQueue;

    FlagAccumulator *accumulator;

    TrapProfile *armProfile;

    RampedOutput *ramp;

    float kArmVelFF;

    Timer *loopTimer;
    Timer *doneTimer;

    FlagAccumulator *toteAccumulator;
    int numTotes;
    bool muchoTotes;
};

}

#endif
