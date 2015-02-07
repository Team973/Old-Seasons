#include "WPILib.h"
#include "sauropod.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/trapProfile.hpp"
#include "../constants.hpp"
#include <math.h>

namespace frc973 {

Sauropod::Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_) {
    elevatorMotor = elevatorMotor_;
    armMotor = armMotor_;
    elevatorEncoder = elevatorEncoder_;
    armEncoder = armEncoder_;

    pdp = new PowerDistributionPanel();

    armPID = new PID(
            Constants::getConstant("kArmP")->getDouble(),
            Constants::getConstant("kArmI")->getDouble(),
            Constants::getConstant("kArmD")->getDouble());
    armPID->setBounds(-1,1);
    armPID->start();
    elevatorPID = new PID(0,0,0);
    elevatorPID->setBounds(-1,1);
    elevatorPID->start();

    inCradle = false;

    currPreset = "hardStop";

    addPreset("hardStop", 0.1, 0.1);
    addPreset("test1", 0, 6);
    addPreset("test2", 0, 3);
    addPreset("test3", 0, 12);
    addPreset("test4", 0, 20);

    setPreset("hardStop");

    currGains = "empty";

    Gains empty = {
        {Constants::getConstant("kUpElevatorP")->getFloat(),
         Constants::getConstant("kUpElevatorI")->getFloat(),
         Constants::getConstant("kUpElevatorD")->getFloat()},

        {Constants::getConstant("kDownElevatorP")->getFloat(),
         Constants::getConstant("kDownElevatorI")->getFloat(),
         Constants::getConstant("kDownElevatorD")->getFloat()}
    };
    Gains oneTote = {
        {0,0,0},
        {0,0,0}
    };

    addGain("empty", empty);
    addGain("oneTote", oneTote);

    setGain("empty");
}

void Sauropod::addGain(std::string name, Gains gain) {
    gainSchedule[name] = gain;
};

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

// this has no way of telling the caller wether or not the gain was found
void Sauropod::setGain(std::string name) {
    if (gainSchedule.find(name) != gainSchedule.end()) {
        currGains = name;
    }
}

void Sauropod::setTarget(Preset target) {
    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float e = sqrt((h*h)+(target.horizProjection*target.horizProjection));
    float deltaY = h - e;
    float elevatorTarget, armTarget;

    if (target.height > switchThreshold) {
        if ((armTarget = 180 - (asin(target.horizProjection/h)*(180/M_PI))) > 160) {
            e = h*(sin(20)*(180/M_PI));
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
    Preset p = presets[currPreset];

    if (p.height < getElevatorHeight()) {
        elevatorPID->setGains(gainSchedule[currGains].down);
    } else {
        elevatorPID->setGains(gainSchedule[currGains].up);
    }

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
    pdp->UpdateTable();
    SmartDashboard::PutNumber("Elevator Height:", getElevatorHeight());
    SmartDashboard::PutNumber("Elevator Current:", pdp->GetCurrent(3));
    SmartDashboard::PutNumber("Elevator Output:", -limit(elevatorPID->update(getElevatorHeight())));
    SmartDashboard::PutNumber("Total Voltage:", pdp->GetVoltage());
    SmartDashboard::PutNumber("Elevator Error:", elevatorPID->getTarget()-getElevatorHeight());
    elevatorMotor->Set(-limit(elevatorPID->update(getElevatorHeight())));
}

}
