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

Sauropod::Sauropod(VictorSP* elevatorMotor_, VictorSP* armMotor_, Encoder* elevatorEncoder_, Encoder* armEncoder_) {
    elevatorMotor = elevatorMotor_;
    armMotor = armMotor_;
    elevatorEncoder = elevatorEncoder_;
    armEncoder = armEncoder_;

    armProfile = new TrapProfile(0,0,0,0);
    loopTimer = new Timer();
    loopTimer->Start();

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
    addPreset("loadHigh", 0, 18);
    addPreset("loadLow", 0, 1);
    addPreset("scoreHeight", 15, 3);
    addPreset("scoreProjection", 15, 2);
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
    currPath = IDLE;

    clearQueue();

    ramp = new RampedOutput(.3,.5);

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(1);

    numTotes = 0;
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
                addToQueue("loadLow");
                addToQueue("loadHigh");
                currPath = PICKUP;
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

// this has no way of telling the caller whether or not the gain was found
void Sauropod::setGain(std::string name) {
    if (gainSchedule.find(name) != gainSchedule.end()) {
        currGains = name;
    }
}

void Sauropod::setTarget(Preset target) {
    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float projection = h*(sin(degreesToRadians(getArmAngle())));
    float e = sqrt((h*h)+(projection*projection));
    float deltaY = e - h;
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
    SmartDashboard::PutNumber("Elevator Target: ", elevatorTarget);
    if (elevatorTarget < 0) {
        elevatorTarget = 0;
    } else if (elevatorTarget > switchThreshold) {
        elevatorTarget = switchThreshold;
    }

    loopTimer->Reset();

    armPID->setTarget(armTarget);
    elevatorPID->setTarget(elevatorTarget);
    ramp->setTarget(armTarget,getArmAngle());
    //armProfile = new TrapProfile(armTarget - getArmAngle(), 10, 10, 10);
}

bool Sauropod::sequenceDone() {
    return atTarget() && waypointQueue.empty();
}

bool Sauropod::atTarget() {
    Preset p = presets[currPreset];

    float switchThreshold = 20; //inches
    float h = 39.25; //inches
    float projection = h*(sin(degreesToRadians(getArmAngle())));
    float e = sqrt((h*h)+(projection*projection));
    float height = 0;

    if (p.height > switchThreshold) {
        height = ((e-h)+(e*2)) + getElevatorHeight();
    } else {
        height = (e-h) + getElevatorHeight();
    }

    SmartDashboard::PutNumber("current height: ", height);
    SmartDashboard::PutNumber("current projection: ", projection);
        accumulator->reset();


    //if ((fabs(p.height - height) <= .6 && fabs(p.projection - projection) <= .6) || (accumulator->update(getElevatorVelocity() < 1 && getArmVelocity() < 1))) {
    if (getElevatorVelocity() < 1 && getArmVelocity() < 1) {
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
        setPreset(waypointQueue.front());
        SmartDashboard::PutString("Curr Preset: ", waypointQueue.front());
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
    return -(armEncoder->Get()/(encoderTicks*gearRatio)*360);
}

float Sauropod::getArmVelocity() {
    float encoderTicks = 360;
    float gearRatio = 5;
    return armEncoder->GetRate() / (encoderTicks*gearRatio) * M_PI / 12;
}

bool Sauropod::inCradle() {
    return getArmAngle() < 1 && getElevatorHeight() < 4;
}

void Sauropod::update() {

    executeQueue();

    Preset currTarget = presets[currPreset];
    setTarget(currTarget);

    float elevatorInput, armInput;

    float currTime = loopTimer->Get();

    std::vector<float> armStep = armProfile->getProfile(currTime);

    float voltageFF;

    if (currTarget.height < getElevatorHeight()) {
        elevatorPID->setGains(gainSchedule[currGains].down);
        voltageFF = -0.02;
    } else {
        elevatorPID->setGains(gainSchedule[currGains].up);
        voltageFF = 0.1;
    }

    float epido = elevatorPID->update(getElevatorHeight()) + voltageFF;
    float apido = armPID->update(getArmAngle());

    if (inCradle() && currTarget.projection > 0) {
        elevatorInput = epido;
        armInput = -0.1;
    } else if (!inCradle() && currTarget.height < 4 && getArmAngle() > 1.5) {
        elevatorInput = 0.1;
        armInput = apido;
    } else {
        elevatorInput = epido;
        armInput = apido;
    }

    if (getArmAngle() < 1.5 && currTarget.projection == 0) {
        armInput += -0.3;
    }

    if (getElevatorHeight() < 5 && getElevatorHeight() > 2 && fabs(getElevatorVelocity()) > 1) {
        elevatorInput += 0.07;
    } else if (getElevatorHeight() > 18 && getElevatorHeight() < 21) {
        elevatorInput += -0.2;
    }

    if (getElevatorCurrent() > 5.2) {
        numTotes = 6;
    } else if (getElevatorCurrent() > 4.2) {
        numTotes = 5;
    } else if (getElevatorCurrent() > 3.2) {
        numTotes = 4;
    } else {
        numTotes = 0;
    }
    SmartDashboard::PutNumber("Num Totes: ", numTotes);
    SmartDashboard::PutNumber("Num Flags: ", accumulator->getFlagCount());

    pdp->UpdateTable();
    SmartDashboard::PutNumber("Elevator Input:", elevatorInput);
    SmartDashboard::PutNumber("Arm Input:", armInput);

    SmartDashboard::PutNumber("Arm Error: ", armPID->getTarget()-getArmAngle());

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
    elevatorMotor->Set(-limit(elevatorInput));
}

}
