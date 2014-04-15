#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

// Auto waypoints
#define FIRE_POINT 9
#define FAR 1
#define MID 2
#define CLOSE 3

class PID;
class Drive
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    float signSquare(float x);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void arcade(float move_, float rotate_);

    float quickStopAccumulator;
    float negInertiaAccumulator;
    float oldWheel;
    float leftDist;
    float rightDist;
    double M_PI; // Apparently not included in math.h

    float normalizeAngle(float theta);

    // Talons
    Talon *leftDrive;
    Talon *rightDrive;

    // Encoders
    Encoder *leftEncoder;
    Encoder *rightEncoder;

    Encoder *gyro;
    Gyro *testGyro;

    Solenoid *shifters;
    Solenoid *kickUp;

    Timer *brakeTimer;
    bool isBraking;

    // Drive calculation variables
    void calculateDrive();
    void storeDriveCalculations();
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

    float point;
    float endPoint;

    float AUTO_END_Y;
    float destination;

public:
    Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_, Gyro *testGyro_);
    void update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn, bool isAuto=false);
    void dashboardUpdate();

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    void resetDriveEncoders();
    void resetGyro();
    //TODO(oliver): Add in the reset gyro function
    float getGyroAngle();
    void resetDrive();
    void setLowGear(bool lowGear);
    void setKickUp(bool kick);

    float generateDriveTime();
    float generateDistanceTime(float x);

    void brake();
    void brakeUpdate();

    float getWaypoint();
    float generateTurnWaypoint();
    void setWaypoint(int dist);
    void goLeft();
    void goRight();
    float getFinalY();
    float getX();
    float getY();
};

#endif
