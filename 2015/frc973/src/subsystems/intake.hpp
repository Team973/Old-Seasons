#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, VictorSP* humanFeederIntakeMotor_, VictorSP *whipMotor_, Solenoid* floorSolenoid_, Solenoid* humanFeederSolenoid_, AnalogInput *whipPot_, DigitalInput *toteSensor_);
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
    VictorSP *whipMotor;

    Solenoid *floorSolenoid;
    Solenoid *humanFeederSolenoid;

    AnalogInput *whipPot;
    DigitalInput *toteSensor;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
