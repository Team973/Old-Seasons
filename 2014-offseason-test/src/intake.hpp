#ifndef INTAKE_HPP
#define INTAKE_HPP
class Intake
{
public:
    Intake(Victor *motor_, Solenoid *claw_);
    void setBehavior(float speed, bool open);
    void setFangs(bool open);
    void setSpeed(float speed);
    void update();
private:
    Victor *motor;
    Solenoid *claw;

    float intakeSpeed;
};
#endif
