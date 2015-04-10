#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "WPILib.h"
#include "lib/gyro.hpp"

namespace frc973 {

class SPIGyro;

class Locator {
public:
    struct Point {
        float angle = 0;
        float distance = 0;
    };

    Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, SPIGyro *gyro_, Encoder *colinGyro_);
    void resetAll();
    void resetGyro();
    void resetEncoders();
    float getMovedDistance();
    float getDistance(Encoder *encoder);
    float getLinearVelocity();
    float normalizeAngle(float theta);
    float getEncoderGyro();
    float getAngle();
    Locator::Point getPoint();
    void update();

private:
    float getVelocity(Encoder *encoder);

    Point *currPoint;

    Encoder *leftEncoder;
    Encoder *rightEncoder;
    SPIGyro *gyro;
    Encoder *colinGyro;
};

}

#endif
