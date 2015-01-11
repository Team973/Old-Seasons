#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "WPILib.h"

namespace frc973 {

class Locator {
public:
    struct Point {
        float x = 0;
        float y = 0;
        float angle= 0;
    };

    Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_);
    void resetAll();
    void resetGyro();
    void resetEncoders();
    float getMovedDistance();
    float normalizeAngle(float theta);
    float getAngle();
    Locator::Point* getPoint();
    void storeCalculations();
    void update();

private:
    float getDistance(Encoder *encoder);

    Point *currPoint;

    Encoder *leftEncoder;
    Encoder *rightEncoder;
    Encoder *gyro;

    float currGyro;
    float currTheta;
    float theta;
    float currLeft;
    float currRight;
    float magnitude;
    float deltaX;
    float deltaY;
    float currX;
    float currY;

    float prevGyro;
    float prevTheta;
    float prevLeft;
    float prevRight;
    float prevX;
    float prevY;
};

}

#endif
