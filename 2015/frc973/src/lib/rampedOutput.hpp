#ifndef RAMPED_OUTPUT_HPP
#define RAMPED_OUTPUT_HPP

namespace frc973 {

class RampedOutput {
public:
    RampedOutput(float min_=.1, float max_=1);
    void setTarget(float target, float start);
    float update(float actual, float input);
private:
    float min;
    float max;
    float upper;
    float lower;
    float rampGain;
    int direction;
    float prev;
};

}

#endif
