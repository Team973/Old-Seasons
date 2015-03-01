#include "WPILib.h"
#include "sauropod.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/trapProfile.hpp"
#include "../lib/flagAccumulator.hpp"
#include "../lib/rampedOutput.hpp"
#include "../constants.hpp"
#include <math.h>

namespace frc973 {

Sauropod::Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_, Solenoid *finger_) {
    elevatorMotor = elevatorMotor_;
    armMotor = armMotor_;
    elevatorEncoder = elevatorEncoder_;
    armEncoder = armEncoder_;
    finger = finger_;

    doneTimer = new Timer();
    doneTimer->Start();

    pdp = new PowerDistributionPanel();

    kArmVelFF = Constants::getConstant("kArmVelFF")->getFloat();

    armPID = new PID(
            Constants::getConstant("kArmP")->getDouble(),
            Constants::getConstant("kArmI")->getDouble(),
            Constants::getConstant("kArmD")->getDouble());
    armPID->setBounds(-.5,.5);
    armPID->start();
    elevatorPID = new PID(0,0,0);
    elevatorPID->setBounds(-1,1);
    elevatorPID->start();


    addPreset("hardStop", 0, 0);
    addPreset("stow", 0, 0);
    addPreset("loadHigh", 0, 20);
    addPreset("loadLow", 0, 1);
    addPreset("containerLoad", 0, 0);
    addPreset("scoreHeight", 11, 6);
    addPreset("scoreProjection", 11, 6);
    addPreset("capHeight", 0, 20);
    addPreset("capExtension", 38, 17);
    addPreset("capPlace", 30, 76.2);
    addPreset("rest",0,6);

    setPreset("hardStop");


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

    currPreset = "hardStop";
    currGains = "empty";
    currPath = NONE;

    clearQueue();

    ramp = new RampedOutput(.3,.5);

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    muchoTotes = false;
    toteAccumulator = new FlagAccumulator();
    toteAccumulator->setThreshold(5);
}

void Sauropod::setFingerContainer(bool fingering) {
    finger->Set(fingering);
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
        currPreset = preset;
    }
    
}

void Sauropod::createPath(int dest) {
    if (dest != currPath || sequenceDone()) {
        clearQueue();
        switch(dest) {
            case PLATFORM:
                addToQueue("rest");
                addToQueue("scoreProjection");
                addToQueue("scoreHeight");
                currPath = PLATFORM;
                break;
            case PICKUP:
                if (currPath != READY) {
                    addToQueue("loadHigh");
                }
                addToQueue("containerLoad");
                currPath = PICKUP;
                break;
            case CONTAINER:
                //addToQueue("containerLoad");
                addToQueue("containerLoad");
                currPath = CONTAINER;
                break;
            case CAP:
                addToQueue("capExtension");
                break;
            case DROP_CAP:
                addToQueue("capPlace");
                break;
            case READY:
                addToQueue("loadHigh");
                currPath = READY;
                break;
            case RESTING:
                addToQueue("rest");
                break;
            case RETURN:
                addToQueue("scoreHeight");
                addToQueue("scoreProjection");
                addToQueue("rest");
                currPath = RETURN;
                break;
            case IDLE:
                addToQueue("stow");
                currPath = IDLE;
                break;
        }
    }
}

int Sauropod::getCurrPath() {
    return currPath;
}

// this has no way of telling the caller whether or not the gain was found
void Sauropod::setGain(std::string name) {
    if (gainSchedule.find(name) != gainSchedule.end()) {
        currGains = name;
    }
}

void Sauropod::setTarget(Preset target) {
    float switchThreshold = 21; //inches
    float h = 39.25; //inches
    float projection = h*(sin(degreesToRadians(getArmAngle())));
    float e = sqrt((h*h)+(projection*projection));
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

    elevatorTarget = target.height - deltaY;
    if (elevatorTarget < 0) {
        elevatorTarget = 0;
    } else if (deltaY > target.height) {
        elevatorTarget = target.height;
    }

    SmartDashboard::PutNumber("Elevator Target: ", elevatorTarget);

    armPID->setTarget(armTarget);
    elevatorPID->setTarget(elevatorTarget);
    ramp->setTarget(armTarget,getArmAngle());
}

void Sauropod::setElevatorManual(float speed) {
    manualElevatorSpeed = speed;
}

bool Sauropod::sequenceDone() {
    return atTarget() && waypointQueue.empty();
}

bool Sauropod::atTarget() {
    Preset p = presets[currPreset];

    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float projection = h*(sin(degreesToRadians(getArmAngle())));
    if (getArmAngle() < 0) {
        projection = 0;
    }
    float e = sqrt((h*h)+(projection*projection));
    float height = 0;

    if (p.height > switchThreshold) {
        height = ((h-e)+(e*2)) + getElevatorHeight();
    } else {
        height = (h-e) + getElevatorHeight();
    }

    SmartDashboard::PutNumber("current height: ", height);
    SmartDashboard::PutNumber("current projection: ", projection);

    SmartDashboard::PutNumber("height error: ", fabs(p.height-height));
    SmartDashboard::PutNumber("projection error: ", fabs(p.projection-projection));


    if ((fabs(p.height - height) <= 1 && fabs(p.projection - projection) <= 1)) {
        if (accumulator->update(fabs(getElevatorVelocity()) < .8 && fabs(getArmVelocity()) < .8)) {
            return true;
        }
    } else if (doneTimer->Get() > 5) {
        return true;
    }

    return false;
}

void Sauropod::clearQueue() {
    std::queue<std::string> dummy;
    waypointQueue.swap(dummy);
}

void Sauropod::addToQueue(std::string preset) {
    waypointQueue.push(preset);
}

void Sauropod::executeQueue() {
    if (waypointQueue.empty()) {
        return;
    }

    if (atTarget()) {
        accumulator->reset();
        doneTimer->Reset();
        setPreset(waypointQueue.front());
        waypointQueue.pop();
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

float Sauropod::getElevatorVelocity() {
    float diameter = 1.27;
    float encoderTicks = 360;
    return elevatorEncoder->GetRate() / encoderTicks * M_PI / 12 * diameter;
}

float Sauropod::getElevatorCurrent() {
    return pdp->GetCurrent(3);
}

float Sauropod::getArmAngle() {
    float encoderTicks = 360;
    float gearRatio = 5;
    return (armEncoder->Get()/(encoderTicks*gearRatio)*360);
}

float Sauropod::getArmVelocity() {
    float encoderTicks = 360;
    float gearRatio = 5;
    return armEncoder->GetRate() / (encoderTicks*gearRatio) * M_PI / 12;
}

bool Sauropod::inCradle() {
    return getArmAngle() < 2 && getElevatorHeight() < 4;
}

bool Sauropod::lotsoTotes() {
    return muchoTotes;
}

void Sauropod::update() {
    executeQueue();

    SmartDashboard::PutString("curr preset: ", currPreset);
    Preset currTarget = presets[currPreset];
    setTarget(currTarget);

    /*
    if (currTarget.projection > 4 && getArmAngle() < 1 && elevatorPID->getTarget() < 5) {
        elevatorPID->setTarget(5);
    } else if (currTarget.projection == 0 && getArmAngle() > 4 && elevatorPID->getTarget() < 5) {
        elevatorPID->setTarget(5);
    }
    */


    float voltageFF = 0;

    if (currTarget.height < getElevatorHeight()) {
        elevatorPID->setGains(gainSchedule[currGains].down);
        elevatorPID->setBounds(-1,1);
        //voltageFF = -0.02;
    } else {
        elevatorPID->setGains(gainSchedule[currGains].up);
        elevatorPID->setBounds(-1,1);
        //voltageFF = 0.1;
    }

    float epido = elevatorPID->update(getElevatorHeight()) + voltageFF;
    float apido = armPID->update(getArmAngle());

    float elevatorInput = 0;
    float armInput = apido;

    /*
    if (inCradle() && currTarget.projection > 0) {
        elevatorInput = epido;
        armInput = -0.1;
    } else if (!inCradle() && currTarget.height < 4 && getArmAngle() > 2) {
        elevatorInput = 0.1;
        armInput = apido;
    } else {
        elevatorInput = epido;
        armInput = apido;
    }
    */

    if (currTarget.projection == 0) {
        armInput += -0.5;
    }

    /*
    if (getElevatorHeight() < 5 && getElevatorHeight() > 2 && fabs(getElevatorVelocity()) > 1) {
        elevatorInput += 0.3;
    } else if (getElevatorHeight() > 18 && getElevatorHeight() < 21) {
        elevatorInput += -0.2;
    } else if (getElevatorHeight() < 2 && currTarget.height == 0) {
        elevatorInput += -0.01;
    }
    */

    if (fabs(getElevatorVelocity()) < .1) {
        muchoTotes = toteAccumulator->update(fabs(getElevatorCurrent() > 4.1));
    } else {
        toteAccumulator->reset();
    }

    if (currPreset == "containerLoad" && getElevatorVelocity() < .1 && getElevatorHeight() > 2 && !sequenceDone()) {
        elevatorInput += -0.9;
    }

    SmartDashboard::PutNumber("Mucho Totes: ", muchoTotes);
    SmartDashboard::PutNumber("Num Flags: ", accumulator->getFlagCount());

    pdp->UpdateTable();
    SmartDashboard::PutNumber("Elevator Input:", elevatorInput);
    SmartDashboard::PutNumber("Arm Input:", armInput);

    SmartDashboard::PutNumber("Arm Error: ", armPID->getTarget() - getArmAngle());

    SmartDashboard::PutNumber("Arm Velocity: ", getArmVelocity());
    SmartDashboard::PutNumber("Elevator Velocity: ", getElevatorVelocity());

    SmartDashboard::PutNumber("Elevator Height:", getElevatorHeight());
    SmartDashboard::PutNumber("Elevator Current:", pdp->GetCurrent(3));
    SmartDashboard::PutNumber("Elevator Output:", limit(elevatorInput));
    SmartDashboard::PutNumber("Total Voltage:", pdp->GetVoltage());
    SmartDashboard::PutNumber("Elevator Error:", elevatorPID->getTarget()-getElevatorHeight());
    SmartDashboard::PutNumber("Arm Angle:", getArmAngle());
    SmartDashboard::PutNumber("Arm Target:", armPID->getTarget());

    armMotor->Set(-ramp->update(getArmAngle(),armInput));
    //armMotor->Set(-armInput);
    if (manualElevatorSpeed == 0) {
        elevatorMotor->Set(-limit(epido));
    } else {
        elevatorMotor->Set(manualElevatorSpeed);
    }
}

}
