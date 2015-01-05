#ifndef DRIVE_HPP
#define DRIVE_HPP
namespace frc973 {

class Drive {
public:
    Drive(Talon *leftFrontMotor_, Talon *rightFrontMotor_, Talon *leftBackMotor_, Talon *rightBackMotor_, Talon *strafeMotor_);
    void arcade(float move_, float rotate_);
    void strafe(float strafe);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:
    void setDriveMotors(float left, float right);

    Talon *leftFrontMotor;
    Talon *rightFrontMotor;
    Talon *leftBackMotor;
    Talon *rightBackMotor;

    Talon *strafeMotor;

    //Cheesy Drive Variables
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
};

}
#endif