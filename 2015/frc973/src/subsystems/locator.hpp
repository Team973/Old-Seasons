#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "WPILib.h"

namespace frc973 {

class Locator {
public:
    struct Point {
        float angle = 0;
        float distance = 0;
    };

    Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_);
    void resetAll();
    void resetGyro();
    void resetEncoders();
    float getMovedDistance();
    float normalizeAngle(float theta);
    float getAngle();
    Locator::Point* getPoint();
    void update();

private:
    float getDistance(Encoder *encoder);

    Point *currPoint;

    Encoder *leftEncoder;
    Encoder *rightEncoder;
    Encoder *gyro;
};

}

#endif
