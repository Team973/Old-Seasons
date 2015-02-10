#ifndef INTAKE_HPP_
#define INTAKE_HPP_

namespace frc973 {

class Intake {
public:
    Intake(VictorSP *leftMotor_, VictorSP *rightMotor_);
    void update();
    void setIntake(float indicatedSpeed);
private:
    float motorSpeed;
    VictorSP *rightMotor;
    VictorSP *leftMotor;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
