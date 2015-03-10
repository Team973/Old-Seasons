#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, Solenoid* solenoidA_, Solenoid* solenoidB_, DigitalInput *toteSensor_);
    void update();
    void setIntake(float indicatedSpeed);
    void setIntakeLeftRight(float left, float right);
    void actuateIntake(bool a, bool b);
    bool gotTote();

private:
    float leftSpeed;
    float rightSpeed;
    float whipMotorSpeed;
    
    VictorSP *rightIntakeMotor;
    VictorSP *leftIntakeMotor;

    Solenoid *solenoidA;
    Solenoid *solenoidB;

    DigitalInput *toteSensor;

    Timer *toteTimer;
    bool hasTote;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
