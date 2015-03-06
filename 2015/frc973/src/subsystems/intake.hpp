#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, Solenoid* floorSolenoid_, DigitalInput *toteSensor_);
    void update();
    void setIntake(float indicatedSpeed);
    void actuateFloorSolenoids(bool actuate);
    bool gotTote();

private:
    float intakeMotorSpeed;
    float whipMotorSpeed;
    bool isFloorSolenoidExtended;
    
    VictorSP *rightIntakeMotor;
    VictorSP *leftIntakeMotor;

    Solenoid *floorSolenoid;

    DigitalInput *toteSensor;

    Timer *toteTimer;
    bool hasTote;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
