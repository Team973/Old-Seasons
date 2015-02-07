#ifndef DRIVE_HPP
#define DRIVE_HPP

namespace frc973 {

class XYManager;

class Drive {
public:
    Drive(VictorSP *leftFrontMotor_, VictorSP *rightFrontMotor_, VictorSP *leftBackMotor_, VictorSP *rightBackMotor_);
    void arcade(float move_, float rotate_);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:
    void setDriveMotors(float left, float right);

    VictorSP *leftFrontMotor;
    VictorSP *rightFrontMotor;
    VictorSP *leftBackMotor;
    VictorSP *rightBackMotor;

    XYManager *xyManager;

    //Cheesy Drive Variables
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
};

}
#endif
