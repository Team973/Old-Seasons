#include "rampedOutput.hpp"
#include <math.h>

namespace frc973 {

RampedOutput::RampedOutput(float min_, float max_) {
    min = min_;
    max = max_;

    rampGain = min;
    prev = 0;
}

void RampedOutput::setTarget(float target, float start) {
    float theta = target - start;
    float absTheta = fabs(theta);

    rampGain = min;

    if (theta > 0) {
        lower = target - (absTheta - (absTheta/4));
        upper = target - (absTheta - (absTheta*.75));
    } else {
        lower = start - (absTheta - (absTheta/4));
        upper = start - (absTheta - (absTheta*.75));
        direction = -1;
    }
}

float RampedOutput::update(float actual, float input) {

    float inputDirection = 1;
    if (input < 0) {
        inputDirection = -1;
    }

    input = fabs(input);

    if (rampGain < min) {
        rampGain = min;
    } else if (rampGain > max) {
        rampGain = max;
    }


    if (input > 0.3) {
        if (actual < lower) {
            input = rampGain; 
            rampGain += .1 * direction;
        } else if (actual > upper) {
            input = rampGain;
            rampGain -= .1 * direction;
        } else {
            rampGain = input;
        }
    }

    return input*inputDirection;
}

}
