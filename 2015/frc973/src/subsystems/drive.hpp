#ifndef DRIVE_HPP
#define DRIVE_HPP
namespace frc973 {

class Drive {
public:
    Drive();
    void arcade(float move_, float rotate_);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:
    void setDriveMotors(float left, float right);
};

}
#endif
