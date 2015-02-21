#ifndef INTAKE_HPP
#define INTAKE_HPP

namespace frc973 {

class PID;

class Intake {

public:
    Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, VictorSP* humanFeederIntakeMotor_, VictorSP *whipMotor_, Solenoid* floorSolenoid_, Solenoid* humanFeederSolenoid_, AnalogInput *whipPot_, DigitalInput *toteSensor_);
    void update();
    void setIntake(float indicatedSpeed);
    void actuateFloorSolenoids(bool actuate);
    void actuateHumanFeederSolenoids(bool actuate);
    void extendWhip();
    void retractWhip();
    float getWhipAngle(); 
    bool gotTote();
private:
    float intakeMotorSpeed;
    float whipMotorSpeed;
    bool isFeederSolenoidExtended;
    bool isFloorSolenoidExtended;
    bool isRetracted;

    VictorSP *rightIntakeMotor;
    VictorSP *leftIntakeMotor;
    VictorSP *humanFeederIntakeMotor;
    VictorSP *whipMotor;

    Solenoid *floorSolenoid;
    Solenoid *humanFeederSolenoid;

    AnalogInput *whipPot;
    DigitalInput *toteSensor;

    PID *whipPID;

    Timer *toteTimer;
    bool hasTote;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
