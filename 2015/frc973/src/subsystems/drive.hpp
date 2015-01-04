#ifndef DRIVE_HPP
#define DRIVE_HPP
namespace frc973 {

class Drive {
public:
    Drive(Talon *leftFrontMotor_, Talon *rightFrontMotor_, Talon *leftBackMotor_, Talon *rightBackMotor_);
    void arcade(float move_, float rotate_);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:
    void setDriveMotors(float left, float right);

    //Cheesy Drive Variables
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
};

}
#endif
