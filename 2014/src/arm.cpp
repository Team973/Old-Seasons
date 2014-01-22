#include "WPILib.h"
#include "arm.hpp"

Arm::Arm(Talon *motor_, Encoder *sensorA_, Encoder *sensorB_, Encoder *sensorC_)
{
    motor = motor_;
    sensorA = sensorA_;
    sensorB = sensorB_;
    sensorC = sensorC_;

    armPID = new PID(0, 0, 0);
    armPID->setBounds(-1.0, 1.0);
    armPID->start();
}

void Arm::setPreset(int preset)
{
    switch (preset)
    {
        case TEST:
            setTarget(10);
            break;
        case TEST2:
            setTarget(30);
            break;
        default:
            setTarget(sensorA->Get());
            break;
    }
}

void Arm::setTarget(float target)
{
    armPID->setTarget(target);
}

float Arm::getTarget()
{
    return armPID->getTarget();
}

void Arm::update()
{
    motor->Set(armPID->update(sensorA->Get()));
}

void Arm::dashboardUpdate()
{
    SmartDashboard::PutNumber("Arm Angle: ", sensorA->Get());
}
