#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, Solenoid* intakeSolenoid_, Solenoid* funnel_, Solenoid* foot_, DigitalInput *toteSensor_);
    void update();
    void setIntake(float indicatedSpeed);
    void setIntakeLeftRight(float left, float right);
    void actuateIntake(bool a);
    void actuateFunnel(bool a);
    void actuateFoot(bool actuate);
    bool gotTote();

private:
    float leftSpeed;
    float rightSpeed;
    
    VictorSP *rightIntakeMotor;
    VictorSP *leftIntakeMotor;

    Solenoid *intakeSolenoid;
    Solenoid *funnel;
    Solenoid *foot;

    DigitalInput *toteSensor;

    Timer *toteTimer;
    bool hasTote;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
