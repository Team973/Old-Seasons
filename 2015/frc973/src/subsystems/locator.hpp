#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "WPILib.h"

namespace frc973 {

class Locator {
public:
    Locator(Encoder *encoderX_, Encoder *encoderY_, Encoder *gyro_);
    void resetAll();
    void resetGyro();
    void resetXY();
    float getCurrX();
    float getCurrY();
    float normalizeAngle(float theta);
    float getAngle();
    void update();
private:
    float getDistance(Encoder *encoder);

    Encoder *encoderX;
    Encoder *encoderY;
    Encoder *gyro;
};

}

#endif
