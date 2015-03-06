
#ifndef TRAP_PROFILE_H
#define TRAP_PROFILE_H

class TrapProfile
{
public:
    TrapProfile(float xTarget_, float vMax_, float aMax_, float dMax_);
    TrapProfile();
    float getTarget();
    std::vector<float> getProfile(float loopTime);
private:
    float realTarget;
    float xTarget;
    float vMax;
    float aMax;
    float dMax;

    float directionFlag;

    bool fake;

    std::vector<float> profile;
};

#endif
