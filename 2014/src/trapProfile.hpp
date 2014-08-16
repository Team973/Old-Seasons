
#ifndef TRAP_PROFILE_H
#define TRAP_PROFILE_H

class TrapProfile
{
public:
    TrapProfile(float xTarget_, float vMax_, float aMax_, float dMax_);
    TrapProfile();
    std::vector<float> getProfile(float loopTime);
private:
    float xTarget;
    float vMax;
    float aMax;
    float dMax;

    bool fake;

    std::vector<float> profile;
};

#endif
