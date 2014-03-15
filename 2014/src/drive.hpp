#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

// Auto waypoints
#define FIRE_POINT 9
#define FAR_LEFT -6
#define MID_LEFT -4
#define CLOSE_LEFT -2
#define FAR_RIGHT 6
#define MID_RIGHT 4
#define CLOSE_RIGHT 2

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

    Solenoid *shifters;
    Solenoid *kickUp;

public:
    Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_);
    void update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn, bool isAuto=false);
    void dashboardUpdate();

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    void resetDriveEncoders();
    //TODO(oliver): Add in the reset gyro function
    float getGyroAngle();
    void resetDrive();
    void setLowGear(bool lowGear);
    void setKickUp(bool kick);


    // Drive calculation variables
    void calculateDrive();
    void storeDriveCalculations();
    float getX();
    float getY();
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

#endif
