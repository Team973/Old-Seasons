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
    Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_, Solenoid *finger_);
    void setPreset(std::string preset);
    void setGain(std::string name);
    void setTarget(Preset target);
    bool sequenceDone();
    bool atTarget();
    void addToQueue(std::string preset);
    void setElevatorManual(float speed);
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
    void setFingerContainer(bool fingering);

    const static int NONE = -1;
    const static int IDLE = 1;
    const static int PICKUP = 2;
    const static int PLATFORM = 3;
    const static int READY = 4;
    const static int RESTING = 5;
    const static int RETURN = 6;
    const static int CONTAINER = 7;
    const static int CAP = 8;
    const static int DROP_CAP = 9;

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
    Solenoid *finger;

    PID *armPID;
    PID *elevatorPID;

    std::string currPreset;
    std::string currGains;
    int currPath;

    std::map<std::string, Preset> presets;
    std::map<std::string, Gains> gainSchedule;

    std::queue<std::string> waypointQueue;

    FlagAccumulator *accumulator;

    RampedOutput *ramp;

    float kArmVelFF;

    Timer *doneTimer;

    float manualElevatorSpeed;

    FlagAccumulator *toteAccumulator;
    bool muchoTotes;
};

}

#endif
