#include "WPILib.h"
#include "sauropod.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/trapProfile.hpp"
#include "../lib/flagAccumulator.hpp"
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

    currPreset = "hardStop";

    addPreset("hardStop", 0, 0);
    addPreset("stow", 0, 6);
    addPreset("currHeight", 0,0);
    addPreset("currProjection", 0,0);
    addPreset("requested",0,0);
    addPreset("test1", 10, 12);
    addPreset("test2", 12, 20);
    addPreset("test3", 4, 6);
    addPreset("test4", 12, 15);

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

    clearQueue();

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    profile = new TrapProfile(0,0,0,0);
    loopTimer = new Timer();
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

// this has no way of telling the caller whether or not the gain was found
void Sauropod::setGain(std::string name) {
    if (gainSchedule.find(name) != gainSchedule.end()) {
        currGains = name;
    }
}

void Sauropod::setTarget(Preset target) {
    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float e = sqrt((h*h)+(target.projection*target.projection));
    float deltaY = h - e;
    float elevatorTarget, armTarget;

    if (target.height > switchThreshold) {
        if ((armTarget = 180 - (radiansToDegrees(asin(target.projection/h)))) > 100) {
            armTarget = 100;
            e = h*(radiansToDegrees(sin(degreesToRadians(80)))); // the 80 is the angle from the arm to the elevator from the top of the arm --> \^| <--
        }

        deltaY += (e*2);
    } else {
        armTarget = radiansToDegrees(asin(target.projection/h));
    }

    if ((elevatorTarget = target.height - deltaY)<0) {
        elevatorTarget = 0;
    }

    armPID->setTarget(armTarget);
    elevatorPID->setTarget(elevatorTarget);
    //profile = new TrapProfile(elevatorTarget-getElevatorHeight(), 0,0,0);
}

bool Sauropod::atTarget() {
    Preset p = presets[currPreset];

    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float projection = h*(sin(degreesToRadians(getArmAngle())));
    float e = sqrt((h*h)+(projection*projection));
    float height = 0;

    if (p.height > switchThreshold) {
        height = ((h-e)+(e*2)) + getElevatorHeight();
    } else {
        height = (h-e) + getElevatorHeight();
    }
    
    SmartDashboard::PutNumber("current height: ", height);
    SmartDashboard::PutNumber("current projection: ", projection);


    return fabs(p.height - height) <= .5 && fabs(p.projection - projection) <= .5;
}

void Sauropod::clearQueue() {
    std::queue<std::string> dummy;
    swap(waypointQueue, dummy);
}

void Sauropod::addToQueue(std::string preset) {
    if (waypointQueue.empty()) {
        setPreset(preset);
    } else {
        waypointQueue.push(preset);
    }
}

void Sauropod::executeQueue() {
    if (waypointQueue.empty()) {
        return;
    }

    if (atTarget()) {
        setPreset(waypointQueue.front());
        waypointQueue.pop();
        accumulator->reset();
    }
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
    float gearRatio = 5;
    return -(armEncoder->Get()/(encoderTicks*gearRatio)*360);
}

bool Sauropod::inCradle() {
    return getArmAngle() < 1 && getElevatorHeight() < 4;
}

void Sauropod::update() {
    Preset currTarget = presets[currPreset];

    float elevatorInput, armInput;

    float epido = elevatorPID->update(getElevatorHeight());
    float apido = armPID->update(getArmAngle());

    //float currTime = loopTimer->Get();

    //std::vector<float> step = profile->getProfile(currTime);

    if (currTarget.height < getElevatorHeight()) {
        elevatorPID->setGains(gainSchedule[currGains].down);
    } else {
        elevatorPID->setGains(gainSchedule[currGains].up);
    }

    if (inCradle() && currTarget.projection > 0) {
        elevatorInput = epido;
        armInput = -0.1;
    } else if (!inCradle() && currTarget.height < 4) {
        if (getArmAngle() > 0) {
            elevatorInput = 0.3;
            armInput = apido;
        }
    } else {
        elevatorInput = epido;
        armInput = apido;
    }

    executeQueue();

    pdp->UpdateTable();
    SmartDashboard::PutNumber("Elevator Height:", getElevatorHeight());
    SmartDashboard::PutNumber("Elevator Current:", pdp->GetCurrent(3));
    SmartDashboard::PutNumber("Elevator Output:", -limit(elevatorPID->update(getElevatorHeight())));
    SmartDashboard::PutNumber("Total Voltage:", pdp->GetVoltage());
    SmartDashboard::PutNumber("Elevator Error:", elevatorPID->getTarget()-getElevatorHeight());
    SmartDashboard::PutNumber("Arm Angle:", getArmAngle());
    SmartDashboard::PutNumber("Arm Target:", armPID->getTarget());

    armMotor->Set(-armInput);
    elevatorMotor->Set(-elevatorInput);
}

}
