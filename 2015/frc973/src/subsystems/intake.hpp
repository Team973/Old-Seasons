#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, VictorSP* humanFeederIntakeMotor_, Solenoid* floorSolenoid_, Solenoid* humanFeederSolenoid_);
    void update();
    void setIntake(float indicatedSpeed);
    void actuateFloorSolenoids(bool actuate);
    void actuateHumanFeederSolenoids(bool actuate);
private:
    float motorSpeed;
    bool feederSolenoidCondtion;
    bool floorSolenoidCondtion;
    VictorSP *rightIntakeMotor;
    VictorSP *leftIntakeMotor;
    VictorSP *humanFeederIntakeMotor;
    Solenoid *floorSolenoid;
    Solenoid *humanFeederSolenoid;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
