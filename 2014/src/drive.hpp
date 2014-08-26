#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

#define LINEAR 21
#define POINT 22
#define TURN 23
#define BLOCK 24

class PID;
class DataLog;
class TrapProfile;

class Drive //keep space between the class of the file and the classes in the file
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    float signSquare(float x);

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

    PID *drivePID;
    PID *rotatePID;
    bool deadPID;

    DataLog *encoderPosLog;
    DataLog *encoderVelLog;
    DataLog *gyroLog;

    TrapProfile *linearGenerator;
    TrapProfile *angularGenerator;

    Timer *loopTimer;

    //XXX destroy us when you are done testing
    Joystick *tuneStick;
    Timer *buttonTimer;

    bool doWeProfile;

    float leftPower;
    float rightPower;

    float getVelocity(Encoder *e);

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

    float prevAngle;

    float driveInput;
    float turnInput;
    float driveTargetX;
    float driveTargetY;

public:
    Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_, Gyro *testGyro_);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void arcade(float move_, float rotate_);
    void update(bool isAuto=false);
    void dashboardUpdate();
    void killPID(bool death);
    void setLinear(TrapProfile *linearGenerator);
    void setAngular(TrapProfile *angularGenerator);

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    float getVelocity();
    void resetDriveEncoders();
    void resetGyro();
    float getGyroAngle();
    void resetDrive();
    void setLowGear(bool lowGear);
    void setKickUp(bool kick);

    float generateDistanceTime(float x);

    float getX();
    float getY();
};

#endif
