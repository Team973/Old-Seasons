#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "WPILib.h"

namespace frc973 {

class Locator {
public:
    Locator(Encoder *encoderX_, Encoder *encoderY_, Encoder *gyro_);
    void reset();
    float getCurrX();
    float getCurrY();
    void update();
private:
    float getDistance(Encoder *encoder);

    Encoder *encoderX;
    Encoder *encoderY;
    Encoder *gyro;
};

}

#endif
