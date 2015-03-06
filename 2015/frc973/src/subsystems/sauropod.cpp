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

Sauropod::Sauropod(VictorSP* elevatorMotor_, Encoder* elevatorEncoder_, Solenoid *clawClamp_, Solenoid *clawBrake_) {
    elevatorMotor = elevatorMotor_;
    elevatorEncoder = elevatorEncoder_;
    clawClamp = clawClamp_;
    clawBrake = clawBrake_;

    doneTimer = new Timer();
    doneTimer->Start();

    pdp = new PowerDistributionPanel();

    elevatorPID = new PID(0,0,0);
    elevatorPID->setBounds(-1,1);
    elevatorPID->start();

    addPreset("hardStop", 0);
    addPreset("stow", 0);
    addPreset("loadHigh", 20);
    addPreset("loadLow", 1);
    addPreset("containerLoad", 0);
    addPreset("rest", 6);

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

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    elevatorIncrement = 0.0;

    muchoTotes = false;
    toteAccumulator = new FlagAccumulator();
    toteAccumulator->setThreshold(5);
}

void Sauropod::addGain(std::string name, Gains gain) {
    gainSchedule[name] = gain;
};

void Sauropod::addPreset(std::string name, float height) {
    Preset p = {height};
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
        elevatorIncrement = 0.0;
        switch(dest) {
            case PICKUP:
                addToQueue("containerLoad");
                currPath = PICKUP;
                break;
            case CONTAINER:
                addToQueue("containerLoad");
                currPath = CONTAINER;
                break;
            case READY:
                addToQueue("loadHigh");
                currPath = READY;
                break;
            case RESTING:
                addToQueue("rest");
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
    float elevatorTarget = target.height;

    elevatorPID->setTarget(elevatorTarget + elevatorIncrement);
}

void Sauropod::incrementElevator(float increment) {
    elevatorIncrement += increment;
}

void Sauropod::setElevatorManual(float speed) {
    manualElevatorSpeed = speed;
}

bool Sauropod::sequenceDone() {
    return atTarget() && waypointQueue.empty();
}

bool Sauropod::atTarget() {
    Preset p = presets[currPreset];

    float height = getElevatorHeight();

    if ((fabs(p.height - height) <= 2)) {
        if (accumulator->update(fabs(getElevatorVelocity()) < .8)) {
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

bool Sauropod::inCradle() {
    return getElevatorHeight() < 4;
}

bool Sauropod::lotsoTotes() {
    return muchoTotes;
}

void Sauropod::update() {
    executeQueue();

    SmartDashboard::PutString("curr preset: ", currPreset);
    Preset currTarget = presets[currPreset];
    setTarget(currTarget);

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

    float elevatorInput = 0;

    if (getElevatorHeight() < 5 && getElevatorHeight() > 2 && fabs(getElevatorVelocity()) > 1) {
        elevatorInput += 0.3;
    } else if (getElevatorHeight() > 18 && getElevatorHeight() < 21) {
        elevatorInput += -0.2;
    } else if (getElevatorHeight() < 2 && currTarget.height == 0) {
        elevatorInput += -0.01;
    }

    if (fabs(getElevatorVelocity()) < .1) {
        muchoTotes = toteAccumulator->update(fabs(getElevatorCurrent() > 4.1));
    } else {
        toteAccumulator->reset();
    }

    SmartDashboard::PutNumber("Mucho Totes: ", muchoTotes);
    SmartDashboard::PutNumber("Num Flags: ", accumulator->getFlagCount());

    pdp->UpdateTable();
    SmartDashboard::PutNumber("Elevator Input:", elevatorInput);

    SmartDashboard::PutNumber("Elevator Velocity: ", getElevatorVelocity());

    SmartDashboard::PutNumber("Elevator Height:", getElevatorHeight());
    SmartDashboard::PutNumber("Elevator Current:", pdp->GetCurrent(3));
    SmartDashboard::PutNumber("Elevator Output:", limit(elevatorInput));
    SmartDashboard::PutNumber("Total Voltage:", pdp->GetVoltage());
    SmartDashboard::PutNumber("Elevator Error:", elevatorPID->getTarget()-getElevatorHeight());

    SmartDashboard::PutNumber("Elevator Increment: ", elevatorIncrement);

    if (manualElevatorSpeed == 0) {
        elevatorMotor->Set(-limit(epido));
    } else {
        elevatorMotor->Set(manualElevatorSpeed);
    }
}

}
