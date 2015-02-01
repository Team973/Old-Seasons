#include "WPILib.h"
#include "sauropod.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/trapProfile.hpp"
#include <math.h>

namespace frc973 {

Sauropod::Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_) {
    elevatorMotor = elevatorMotor_;
    armMotor = armMotor_;
    elevatorEncoder = elevatorEncoder_;
    armEncoder = armEncoder_;

    armPID = new PID(0,0,0);
    elevatorPID = new PID(0,0,0);

    currPreset = "hardStop";

    addPreset("hardStop", 0, 0);
    addPreset("test1", 2, 3);
    addPreset("test2", 1, 1.5);
}

void Sauropod::addPreset(std::string name, float horiz, float height) {
    Preset p = {horiz, height};
    presets[name] = p;
}

void Sauropod::setPreset(std::string preset) {
    if (presets.find(preset) != presets.end()) {
        setTarget(presets[preset]);
        currPreset = preset;
    } else {
        setTarget(presets[currPreset]);
    }
    
}

void Sauropod::setTarget(Preset target) {
    float switchThreshold = 5; //feet
    float h = 3.27;
    float e = sqrt((h*h)+(target.horizProjection*target.horizProjection));
    float deltaY = h - e;
    float elevatorTarget, armTarget;

    if (target.height > switchThreshold) {
        if ((armTarget = 180 - (asin(target.horizProjection/h)*(180/M_PI))) > 160) {
            e = h*sin(20);
            deltaY += (e*2);
        }
    } else {
        armTarget = asin(target.horizProjection/h)*(180/M_PI);
    }

    if ((elevatorTarget = target.height - deltaY)<0) {
        elevatorTarget = 0;
    }

    armPID->setTarget(armTarget);
    elevatorPID->setTarget(elevatorTarget);
}

float Sauropod::getElevatorHeight() {
    float encoderTicks = 360;
    float diameter = 1.5;
    float gearRatio = 1;
    float distancePerRevolution = diameter * M_PI;
    return (elevatorEncoder->Get()/(encoderTicks*gearRatio))*distancePerRevolution;
}

float Sauropod::getArmAngle() {
    float encoderTicks = 360;
    float gearRatio = 1;
    return armEncoder->Get()/(encoderTicks*gearRatio)*360;
}

void Sauropod::update() {
}

}
