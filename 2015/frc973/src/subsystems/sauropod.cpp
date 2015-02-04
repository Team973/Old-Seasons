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
    armPID->setBounds(-1,1);
    armPID->start();
    elevatorPID = new PID(.1,0,0);
    elevatorPID->setBounds(-1,1);
    elevatorPID->start();

    inCradle = false;

    currPreset = "hardStop";

    addPreset("hardStop", 0.1, 0.1);
    addPreset("test1", 0, 6);
    addPreset("test2", 0, 3);
    addPreset("test3", 0, 12);

    setPreset("hardStop");
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
    float switchThreshold = 60; //inches
    float h = 39.25; //inches
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

// in feet
float Sauropod::getElevatorHeight() {
    float encoderTicks = 360;
    float diameter = 1.27; //inches
    float gearRatio = 1;
    float distancePerRevolution = diameter * M_PI;
    return ((elevatorEncoder->Get()/(encoderTicks*gearRatio))*distancePerRevolution);
}

float Sauropod::getArmAngle() {
    float encoderTicks = 360;
    float gearRatio = 1;
    return armEncoder->Get()/(encoderTicks*gearRatio)*360;
}

bool Sauropod::isPackSafe() {
    return getElevatorHeight() >= .3;//feet
}

bool Sauropod::isDropSafe() {
    return getArmAngle() < 2 && getElevatorHeight() > .5;
}

void Sauropod::update() {
    //Preset p = presets[currPreset];

    /*
    if (p.horizProjection == 0 && p.height < .3) {
        if (inCradle && !isPackSafe()) {
            Preset target = {0, .3};
            setTarget(target);
        } else if (inCradle && !isDropSafe()) {
            Preset target = {.3, p.height};
            setTarget(target);
        } else if (!isPackSafe() || !isDropSafe()) {
            Preset target = {0,.3};
            setTarget(target);
        } else {
            setTarget(p);
        }
    }
    */

    //armMotor->Set(armPID->update(getArmAngle()));
    elevatorMotor->Set(0.5);//-limit(elevatorPID->update(getArmAngle())));
}

}
