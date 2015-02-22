#ifndef DRIVE_HPP
#define DRIVE_HPP

namespace frc973 {

class XYManager;

class Drive {
public:
    Drive(VictorSP *left_, VictorSP *right_);
    void controlInterface(double throttle, double wheel, bool highGear, bool quickTurn);
    void arcade(float move_, float rotate_);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:
    void setDriveMotors(float left, float right);
    float signSquare(float x);

    VictorSP *leftMotor;
    VictorSP *rightMotor;

    XYManager *xyManager;

    //Cheesy Drive Variables
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
};

}
#endif
