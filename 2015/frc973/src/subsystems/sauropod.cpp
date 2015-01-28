#include "sauropod.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/trapProfile.hpp"
#include <math.h>

namespace frc973 {

Sauropod::Sauropod() {
    armPID = new PID(0,0,0);
    elevatorPID = new PID(0,0,0);

    currPreset = "test1";

    presets["test1"];
    presets["test2"];
}

void Sauropod::setPreset(std::string preset) {
    if (presets.find(preset) != presets.end()) {
        setTarget(presets[preset]);
    } else {
        setTarget(presets[currPreset]);
    }
    
}

void Sauropod::setTarget(Preset target) {
    float h = 3.27;
    float e = sqrt((h*h)+(target.horizProjection*target.horizProjection));
    float deltaY = h - e;
    armPID->setTarget(asin(target.horizProjection/h)*(180/M_PI));
    elevatorPID->setTarget(target.height - deltaY);
}

void Sauropod::update() {
}

}
